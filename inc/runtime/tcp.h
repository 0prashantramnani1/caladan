/*
 * tcp.h - TCP sockets
 */

#pragma once

#include <runtime/net.h>
#include <runtime/thread.h>
#include <sys/uio.h>
#include <sys/socket.h>

struct tcpqueue;
typedef struct tcpqueue tcpqueue_t;
struct tcpconn;
typedef struct tcpconn tcpconn_t;

struct tcparg {
        tcpqueue_t *q;
        tcpconn_t **c;
};
typedef struct tcparg tcparg_t;

struct tcp_read_arg {
        tcpconn_t	**c;
	void		*buf;
	size_t		len;
};
typedef struct tcp_read_arg tcp_read_arg_t;

// Caraousel
// extern uint64_t conns_timeout_ordering[2000];

extern int tcp_dial(struct netaddr laddr, struct netaddr raddr,
		    tcpconn_t **c_out);
extern int tcp_dial_affinity(uint32_t affinity, struct netaddr raddr,
		    tcpconn_t **c_out);
extern int tcp_dial_conn_affinity(tcpconn_t *in, struct netaddr raddr,
		    tcpconn_t **c_out);
extern int tcp_listen(struct netaddr laddr, int backlog, tcpqueue_t **q_out);
extern int tcp_accept(tcpqueue_t *q, tcpconn_t **c_out);
extern int tcp_accept_poll(tcparg_t *arg);
extern void tcp_qshutdown(tcpqueue_t *q);
extern void tcp_qclose(tcpqueue_t *q);
extern struct netaddr tcp_local_addr(tcpconn_t *c);
extern struct netaddr tcp_remote_addr(tcpconn_t *c);
extern ssize_t tcp_read(tcpconn_t *c, void *buf, size_t len);
extern ssize_t tcp_read_poll(tcp_read_arg_t *arg);
extern ssize_t tcp_write(tcpconn_t *c, const void *buf, size_t len);
extern ssize_t tcp_readv(tcpconn_t *c, const struct iovec *iov, int iovcnt);
extern ssize_t tcp_writev(tcpconn_t *c, const struct iovec *iov, int iovcnt);
extern int tcp_shutdown(tcpconn_t *c, int how);
extern void tcp_abort(tcpconn_t *c);
extern void tcp_close(tcpconn_t *c);
extern void tcp_set_nonblocking(tcpconn_t *c, bool nonblocking);
extern void tcp_init_uthread(tcpconn_t *c, thread_t* t);
extern int tcp_get_raddr_port(tcpconn_t *c);
extern void tcpqueue_set_nonblocking(tcpqueue_t *c, bool nonblocking);
extern struct list_head *tcp_get_triggers(tcpconn_t *c);
extern void tcp_put_table_id(tcpconn_t *c, int id);
extern int tcp_get_table_id(tcpconn_t *c);
extern struct list_head *tcpqueue_get_triggers(tcpqueue_t *q);
extern void tcpqueue_check_triggers(tcpqueue_t *q);
extern void tcpconn_check_triggers(tcpconn_t *c);
extern bool is_tcp_rx_empty(tcpconn_t *c);

extern long long int tcp_get_reqs(tcpconn_t *c);
extern int tcpqueue_get_num_connections_accepted(tcpqueue_t *q);

