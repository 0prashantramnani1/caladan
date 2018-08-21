/*
 * tcp_in.c - the ingress datapath for TCP
 *
 * Based on RFC 793 and RFC 1122 (errata).
 *
 * FIXME: We do too little to prevent heavy fragmentation in the out-of-order
 * RX queue.
 */

#include <base/stddef.h>
#include <runtime/smalloc.h>
#include <net/ip.h>
#include <net/tcp.h>

#include "tcp.h"
#include "defs.h"

/* four cases for the acceptability test for an incoming segment */
static bool is_acceptable(tcpconn_t *c, uint32_t len, uint32_t seq)
{
	assert_spin_lock_held(&c->lock);

	if (len == 0 && c->pcb.rcv_wnd == 0) {
		return seq == c->pcb.rcv_nxt;
	} else if (len == 0 && c->pcb.rcv_wnd > 0) {
		return wraps_lte(c->pcb.rcv_nxt, seq) &&
		       wraps_lt(seq, c->pcb.rcv_nxt + c->pcb.rcv_wnd);
	} else if (len > 0 && c->pcb.rcv_wnd == 0) {
		return false;
	}

	/* (len > 0 && c->rcv_wnd > 0) */
	return (wraps_lte(c->pcb.rcv_nxt, seq) &&
		wraps_lt(seq, c->pcb.rcv_nxt + c->pcb.rcv_wnd)) ||
	       (wraps_lte(c->pcb.rcv_nxt, seq + len - 1) &&
		wraps_lt(seq + len - 1, c->pcb.rcv_nxt + c->pcb.rcv_wnd));
}

/* is the TX window full? */
static bool is_snd_full(tcpconn_t *c)
{
	assert_spin_lock_held(&c->lock);

	return c->pcb.snd_una + c->pcb.snd_wnd <= c->pcb.snd_nxt;
}

/* see reset generation (RFC 793) */
static void send_rst(tcpconn_t *c, bool acked, uint32_t seq, uint32_t ack,
		     uint32_t len)
{
	if (acked) {
		tcp_tx_raw_rst(c->e.laddr, c->e.raddr, ack);
		return;
	}
	tcp_tx_raw_rst_ack(c->e.laddr, c->e.raddr, 0, seq + len);
}

static void tcp_rx_append_text(tcpconn_t *c, struct mbuf *m)
{
	uint32_t len;

	assert_spin_lock_held(&c->lock);

	/* verify assumptions enforced by acceptability testing */
	assert(wraps_lte(m->seg_seq, c->pcb.rcv_nxt));
	assert(wraps_gt(m->seg_end, c->pcb.rcv_nxt));

	/* does the next receive octet clip the head of the text? */
	if (wraps_lt(m->seg_seq, c->pcb.rcv_nxt)) {
		len = c->pcb.rcv_nxt - m->seg_seq;
		mbuf_pull(m, len);
		m->seg_seq += len;
	}

	/* does the receive window clip the tail of the text? */
	if (wraps_lt(c->pcb.rcv_nxt + c->pcb.rcv_wnd, m->seg_end)) {
		len = m->seg_end - (c->pcb.rcv_nxt + c->pcb.rcv_wnd);
		mbuf_trim(m, len);
		m->seg_end = c->pcb.rcv_nxt + c->pcb.rcv_wnd;
	}

	/* enqueue the text */
	c->pcb.rcv_nxt = m->seg_end;
	assert(c->pcb.rcv_wnd >= m->seg_end - m->seg_seq);
	c->pcb.rcv_wnd -= m->seg_end - m->seg_seq;
	list_add_tail(&c->rxq, &m->link);
}

/* process RX text segments, returning true if @m is used for text */
static bool tcp_rx_text(tcpconn_t *c, struct mbuf *m, bool *wake)
{
	struct mbuf *pos;

	assert_spin_lock_held(&c->lock);

	/* don't accept any text if the receive window is zero */
	if (c->pcb.rcv_wnd == 0)
		return false;

	if (wraps_lte(m->seg_seq, c->pcb.rcv_nxt)) {
		/* we got the next in-order segment */
		if (m->push)
			*wake = true;
		tcp_rx_append_text(c, m);
	} else {
		/* we got an out-of-order segment */
		list_for_each(&c->rxq_ooo, pos, link) {
			if (wraps_lt(m->seg_seq, pos->seg_seq)) {
				list_add_before(&pos->link, &m->link);
				goto drain;
			} else if (m->seg_seq == pos->seg_seq) {
				return false;
			}
		}
 		list_add_tail(&c->rxq_ooo, &m->link);
	}

drain:
	/* attempt to drain the out-of-order RX queue */
	while (true) {
		pos = list_top(&c->rxq_ooo, struct mbuf, link);
		if (!pos)
			break;

		/* has the segment been fully received already? */
		if (wraps_lte(pos->seg_end, c->pcb.rcv_nxt)) {
			list_del(&pos->link);
			mbuf_free(pos);
			continue;
		}

		/* is the segment still out-of-order? */
		if (wraps_gt(pos->seg_seq, c->pcb.rcv_nxt))
			break;

		/* we got the next in-order segment */
		list_del(&pos->link);
		if (pos->push)
			*wake = true;
		tcp_rx_append_text(c, pos);
	}

	return true;
}

/* handles ingress packets for TCP connections */
void tcp_rx_conn(struct trans_entry *e, struct mbuf *m)
{
	tcpconn_t *c = container_of(e, tcpconn_t, e);
	struct list_head q;
	const struct ip_hdr *iphdr;
	const struct tcp_hdr *tcphdr;
	uint32_t seq, ack, len, snd_nxt;
	uint16_t win;
	bool do_ack = false, do_drop = false;
	int ret;

	assert_preempt_disabled();

	list_head_init(&q);
	snd_nxt = load_acquire(&c->pcb.snd_nxt);

	/* find header offsets */
	iphdr = mbuf_network_hdr(m, *iphdr);
	tcphdr = mbuf_pull_hdr_or_null(m, *tcphdr);
	if (unlikely(!tcphdr)) {
		mbuf_free(m);
		return;
	}

	/* parse header */
	seq = ntoh32(tcphdr->seq);
	ack = ntoh32(tcphdr->ack);
	win = ntoh16(tcphdr->win);
	len = ntoh16(iphdr->len) - sizeof(*iphdr) - sizeof(*tcphdr);
	if (unlikely(len != mbuf_length(m))) {
		mbuf_free(m);
		return;
	}

	spin_lock_np(&c->lock);

	if (c->pcb.state == TCP_STATE_CLOSED) {
		do_drop = true;
		goto done;
	}

	if (c->pcb.state == TCP_STATE_SYN_SENT) {
		if ((tcphdr->flags & TCP_ACK) > 0) {
			if (wraps_lte(ack, c->pcb.iss) ||
			    wraps_gt(ack, snd_nxt)) {
				send_rst(c, false, seq, ack, len);
				do_drop = true;
				goto done;
			}
			if ((tcphdr->flags & TCP_RST) > 0) {
				/* check if the ack is valid */
				if (wraps_lte(c->pcb.snd_una, ack) &&
				    wraps_lte(ack, snd_nxt)) {
					tcp_conn_fail(c, ECONNRESET);
					do_drop = true;
					goto done;
				}
			}
		} else if ((tcphdr->flags & TCP_RST) > 0) {
			do_drop = true;
			goto done;
		}
		if ((tcphdr->flags & TCP_SYN) > 0) {
			c->pcb.rcv_nxt = seq + 1;
			c->pcb.irs = seq;
			if ((tcphdr->flags & TCP_ACK) > 0) {
				c->pcb.snd_una = ack;
				tcp_conn_ack(c, &q);
			}
			if (wraps_gt(c->pcb.snd_una, c->pcb.iss)) {
				do_ack = true;
				c->pcb.snd_wnd = win;
				c->pcb.snd_wl1 = seq;
				c->pcb.snd_wl2 = ack;
				tcp_conn_set_state(c, TCP_STATE_ESTABLISHED);
			} else {
				ret = tcp_tx_ctl(c, TCP_SYN | TCP_ACK);
				if (unlikely(ret)) {
					do_drop = true;
					goto done; /* feign packet loss */
				}
				tcp_conn_set_state(c, TCP_STATE_SYN_RECEIVED);
			}
		}
		goto done;
	}

	/*
	 * TCP_STATE_SYN_RECEIVED || TCP_STATE_ESTABLISHED ||
	 * TCP_STATE_FIN_WAIT1 || TCP_STATE_FIN_WAIT2 ||
	 * TCP_STATE_CLOSE_WAIT || TCP_STATE_CLOSING ||
	 * TCP_STATE_LAST_ACK || TCP_STATE_TIME_WAIT
	 */

	/* step 1 - acceptability testing */
	if (!is_acceptable(c, len, seq)) {
		do_ack = (tcphdr->flags & TCP_RST) == 0;
		do_drop = true;
		goto done;
	}

	/* step 2 - RST */
	if ((tcphdr->flags & TCP_RST) > 0) {
		tcp_conn_fail(c, ECONNRESET);
		do_drop = true;
		goto done;
	}

	/* step 3 - security checks skipped */

	/* step 4 - SYN */
	if ((tcphdr->flags & TCP_SYN) > 0) {
		send_rst(c, (tcphdr->flags & TCP_ACK) > 0, seq, ack, len);
		tcp_conn_fail(c, ECONNRESET);
		do_drop = true;
		goto done;
	}

	/* step 5 - ACK */
	if ((tcphdr->flags & TCP_ACK) == 0) {
		do_drop = true;
		goto done;
	}
	if (c->pcb.state == TCP_STATE_SYN_RECEIVED) {
		if (!(wraps_lte(c->pcb.snd_una, ack) &&
		      wraps_lte(ack, snd_nxt))) {
			send_rst(c, true, seq, ack, len);
			do_drop = true;
			goto done;
		}
		c->pcb.snd_wnd = win;
		c->pcb.snd_wl1 = seq;
		c->pcb.snd_wl2 = ack;
		tcp_conn_set_state(c, TCP_STATE_ESTABLISHED);
	}
	if (wraps_lte(c->pcb.snd_una, ack) &&
	    wraps_lte(ack, snd_nxt)) {
		bool snd_was_full = is_snd_full(c);
		c->pcb.snd_una = ack;
		tcp_conn_ack(c, &q);
		/* should we update the send window? */
		if (wraps_lt(c->pcb.snd_wl1, seq) ||
		    (c->pcb.snd_wl1 == seq &&
		     wraps_lte(c->pcb.snd_wl2, ack))) {
			c->pcb.snd_wnd = win;
			c->pcb.snd_wl1 = seq;
			c->pcb.snd_wl2 = ack;
		}
		if (snd_was_full && !is_snd_full(c))
			waitq_signal_locked(&c->tx_wq, &c->lock);
	} else if (wraps_gt(ack, snd_nxt)) {
		do_ack = true;
		do_drop = true;
		goto done;
	}
	if (c->pcb.state == TCP_STATE_FIN_WAIT1 &&
	    c->pcb.snd_una == snd_nxt) {
		tcp_conn_set_state(c, TCP_STATE_FIN_WAIT2);
	} else if (c->pcb.state == TCP_STATE_CLOSING &&
		   c->pcb.snd_una == snd_nxt) {
		tcp_conn_set_state(c, TCP_STATE_TIME_WAIT);
	} else if (c->pcb.state == TCP_STATE_LAST_ACK &&
		   c->pcb.snd_una == snd_nxt) {
		tcp_conn_set_state(c, TCP_STATE_CLOSED);
		tcp_conn_put(c); /* safe because RCU + preempt is disabled */
		goto done;
	}

	/* step 6 - URG support skipped */

	/* step 7 - segment text */
	if (len > 0 &&
	    (c->pcb.state == TCP_STATE_ESTABLISHED ||
	     c->pcb.state == TCP_STATE_FIN_WAIT1 ||
	     c->pcb.state == TCP_STATE_FIN_WAIT2)) {
		bool wake = false;
		m->seg_seq = seq;
		m->seg_end = seq + len;
		m->push = (tcphdr->flags & TCP_PUSH) > 0;
		do_drop = !tcp_rx_text(c, m, &wake);
		if (wake) {
			assert(!list_empty(&c->rxq));
			assert(do_drop == false);
			waitq_signal_locked(&c->rx_wq, &c->lock);
		}
		do_ack = true;
	}

	/* step 8 - FIN */
	if ((tcphdr->flags & TCP_FIN) == 0)
		goto done;
	tcp_conn_shutdown_rx(c);
	if (c->pcb.state == TCP_STATE_SYN_RECEIVED ||
	    c->pcb.state == TCP_STATE_ESTABLISHED) {
		tcp_conn_set_state(c, TCP_STATE_CLOSE_WAIT);
	} else if (c->pcb.state == TCP_STATE_FIN_WAIT1) {
		assert(c->pcb.snd_una != snd_nxt);
		tcp_conn_set_state(c, TCP_STATE_CLOSING);
	} else if (c->pcb.state == TCP_STATE_FIN_WAIT2) {
		/* TODO: enable time-wait timer and disable other timers */
		tcp_conn_set_state(c, TCP_STATE_TIME_WAIT);
	}

done:
	spin_unlock_np(&c->lock);

	/* deferred work (delayed until after the lock was dropped) */
	mbuf_list_free(&q);
	if (do_ack)
		tcp_tx_ack(c);
	if (do_drop)
		mbuf_free(m);
}

/* handles ingress packets for TCP listener queues */
tcpconn_t *tcp_rx_listener(struct netaddr laddr, struct mbuf *m)
{
	struct netaddr raddr;
	const struct ip_hdr *iphdr;
	const struct tcp_hdr *tcphdr;
	tcpconn_t *c;
	int ret;

	/* find header offsets */
	iphdr = mbuf_network_hdr(m, *iphdr);
	tcphdr = mbuf_pull_hdr_or_null(m, *tcphdr);
	if (unlikely(!tcphdr))
		return NULL;

	/* calculate local and remote network addresses */
	raddr.ip = ntoh32(iphdr->saddr);
	raddr.port = ntoh16(tcphdr->sport);

	/* do exactly what RFC 793 says */
	if ((tcphdr->flags & TCP_RST) > 0)
		return NULL;
	if ((tcphdr->flags & TCP_ACK) > 0) {
		tcp_tx_raw_rst(laddr, raddr, ntoh32(tcphdr->ack));
		return NULL;
	}
	if ((tcphdr->flags & TCP_SYN) == 0)
		return NULL;

	/* TODO: the spec requires us to enqueue but not post any data */
	if (ntoh16(iphdr->len) - sizeof(*iphdr) != sizeof(*tcphdr))
		return NULL;

	/* we have a valid SYN packet, initialize a new connection */
	c = tcp_conn_alloc();
	if (unlikely(!c))
		return NULL;
	c->pcb.irs = ntoh32(tcphdr->seq);
	c->pcb.rcv_nxt = c->pcb.irs + 1;

	/*
	 * attach the connection to the transport layer. From this point onward
	 * ingress packets can be dispatched to the connection.
	 */
	ret = tcp_conn_attach(c, laddr, raddr);
	if (unlikely(!ret)) {
		sfree(c);
		return NULL;
	}

	/* finally, send a SYN/ACK to the remote host */
	spin_lock_np(&c->lock);
	ret = tcp_tx_ctl(c, TCP_SYN | TCP_ACK);
	if (unlikely(!ret)) {
		spin_unlock_np(&c->lock);
		tcp_conn_destroy(c);
		return NULL;
	}
	tcp_conn_get(c); /* take a ref for the state machine */
	tcp_conn_set_state(c, TCP_STATE_SYN_RECEIVED);
	spin_unlock_np(&c->lock);

	return c;
}
