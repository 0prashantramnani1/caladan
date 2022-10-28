/*
 * netperf.c - a client similar to netperf
 */

#include <stdio.h>
#include <string.h>
#include <limits.h>

#include <base/stddef.h>
#include <base/log.h>
#include <base/time.h>
#include <base/list.h>
#include <base/thread.h>
#include <net/ip.h>
#include <runtime/runtime.h>
#include <runtime/sync.h>
#include <runtime/tcp.h>
#include <runtime/poll.h>

#define NETPERF_PORT	8000

/* experiment parameters */
static struct netaddr raddr;
static int nworkers;
static int seconds;
static uint64_t stop_us;
static size_t payload_len;
static int depth;

#define BUF_SIZE	32768

struct client_rr_args {
	waitgroup_t *wg;
	uint64_t reqs;
};

static void client_worker(void *arg)
{
	unsigned char buf[BUF_SIZE];
	struct client_rr_args *args = (struct client_rr_args *)arg;
	tcpconn_t *c;
	struct netaddr laddr;
	ssize_t ret;
	int budget = depth;

	/* local IP + ephemeral port */
	laddr.ip = 0;
	laddr.port = 0;

	memset(buf, 0xAB, payload_len);

	ret = tcp_dial(laddr, raddr, &c);
	if (ret) {
		log_err("tcp_dial() failed, ret = %ld", ret);
		goto done;
	}

	while (microtime() < stop_us) {
		while (budget) {
			ret = tcp_write(c, buf, payload_len);
			if (ret != payload_len) {
				log_err("tcp_write() failed, ret = %ld", ret);
				break;
			}
			budget--;
		}

		ret = tcp_read(c, buf, payload_len * depth);
		if (ret <= 0 || ret % payload_len != 0) {
			log_err("tcp_read() failed, ret = %ld", ret);
			break;
		}

		budget += ret / payload_len;
		args->reqs += ret / payload_len;
	}

	log_info("close port %hu", tcp_local_addr(c).port);
	tcp_abort(c);
	tcp_close(c);
done:
	waitgroup_done(args->wg);
}

static void do_client(void *arg)
{
	waitgroup_t wg;
	struct client_rr_args *arg_tbl;
	int i, ret;
	uint64_t reqs = 0;

	log_info("client-mode TCP: %d workers, %ld bytes, %d seconds %d depth",
		 nworkers, payload_len, seconds, depth);

	arg_tbl = calloc(nworkers, sizeof(*arg_tbl));
	BUG_ON(!arg_tbl);

	waitgroup_init(&wg);
	waitgroup_add(&wg, nworkers);
	stop_us = microtime() + seconds * ONE_SECOND;
	for (i = 0; i < nworkers; i++) {
		arg_tbl[i].wg = &wg;
		arg_tbl[i].reqs = 0;
		ret = thread_spawn(client_worker, &arg_tbl[i]);
		BUG_ON(ret);
	}

	waitgroup_wait(&wg);

	for (i = 0; i < nworkers; i++)
		reqs += arg_tbl[i].reqs;

	log_info("measured %f reqs/s", (double)reqs / seconds);
}

static void server_worker(void *arg)
{
	printf("new server thread started\n");
	unsigned char buf[BUF_SIZE];
	tcpconn_t *c = (tcpconn_t *)arg;
	ssize_t ret;
	tcp_set_nonblocking(c, 1);

	
	//Creating a waiter for this thread	
	poll_waiter_t *w;
	ret = create_waiter(&w);

	//Creating a trigger for this socket
	poll_trigger_t *t;
	ret = create_trigger(&t);
	
	struct list_head *h;
      	h = tcp_get_triggers(c);

	//register trigger with a waiter
	poll_arm_w_sock(w, h, t, SEV_READ, NULL, NULL, c, -7);
	
	/* echo the data back */
	int read = 0;
	int write = 0;
	printf("server_worker: entering while loop\n");
	while (true) {
		// TODO: Need a fix. Won't work if packet from client arrives before
		// trigger is set. Will be stuck in poll_wait
		int ret = poll_cb_once_nonblock(w); 
		//printf("server_worker: poll_cb_once data: %d\n", ret);
		if(ret == 0) {
			printf("Server_worker: EVent triggered, going into read \n");
			read = tcp_read(c, buf, BUF_SIZE);
		}
		if(read > 0) {
			printf("Server_worker: read %d bytes %d\n", read, thread_id);
		} 
		//else {
	//		printf("CLOSING THE CONNECTION\n");
	//		break;
	//	}

			
		if(read > 0 ) {
			//timer_sleep(5*ONE_SECOND);
			printf("server_worker: going for tcp_write \n");
			write = tcp_write(c, buf, read);
			printf("server_worker: tcp_write done\n");
			read = 0;
		} 
		t->triggered = false;	
	}

	tcp_close(c);
}

static void do_server(void *arg)
{
	struct netaddr laddr;
	tcpqueue_t *q;
	int ret;

	laddr.ip = 0;
	laddr.port = NETPERF_PORT;

	ret = tcp_listen(laddr, 4096, &q);
	tcpqueue_set_nonblocking(q, 1);
	BUG_ON(ret);

        //Creating a waiter for this thread
        poll_waiter_t *w;
        ret = create_waiter(&w);

	//Creating a trigger for this socket
        poll_trigger_t *t;
        ret = create_trigger(&t);

        struct list_head *h;
        h = tcpqueue_get_triggers(q);

        //register trigger with a waiter
        poll_arm_w_queue(w, h, t, SEV_READ, NULL, NULL, q, -7);

	while (true) {
		tcpconn_t *c;

		ret = poll_cb_once_nonblock(w); 
		if(ret == 0) {
			printf("Server_worker: EVent triggered, going into read \n");
			ret = tcp_accept(q, &c);
			BUG_ON(ret);
			printf("Connection Accepted\n");
			ret = thread_spawn(server_worker, c);
			BUG_ON(ret);
		}
		
		/*
		ret = tcp_accept(q, &c);
		BUG_ON(ret);
		printf("Connection Accepted\n");
		ret = thread_spawn(server_worker, c);
		BUG_ON(ret);
		*/
	}
}

static int str_to_ip(const char *str, uint32_t *addr)
{
	uint8_t a, b, c, d;
	if(sscanf(str, "%hhu.%hhu.%hhu.%hhu", &a, &b, &c, &d) != 4) {
		return -EINVAL;
	}

	*addr = MAKE_IP_ADDR(a, b, c, d);
	return 0;
}

static int str_to_long(const char *str, long *val)
{
	char *endptr;

	*val = strtol(str, &endptr, 10);
	if (endptr == str || (*endptr != '\0' && *endptr != '\n') ||
	    ((*val == LONG_MIN || *val == LONG_MAX) && errno == ERANGE))
		return -EINVAL;
	return 0;
}

int main(int argc, char *argv[])
{
	int ret;
	long tmp;
	uint32_t addr;
	thread_fn_t fn;

	if (argc < 8) {
		printf("%s: [config_file_path] [mode] [nworkers] [ip] [time] "
		       "[payload_len] [depth]\n", argv[0]);
		return -EINVAL;
	}

	if (!strcmp(argv[2], "CLIENT")) {
		fn = do_client;
	} else if (!strcmp(argv[2], "SERVER")) {
		fn = do_server;
	} else {
		printf("invalid mode '%s'\n", argv[2]);
		return -EINVAL;
	}

	ret = str_to_long(argv[3], &tmp);
	if (ret) {
		printf("couldn't parse [nworkers] '%s'\n", argv[3]);
		return -EINVAL;
	}
	nworkers = tmp;

	ret = str_to_ip(argv[4], &addr);
	if (ret) {
		printf("couldn't parse [ip] '%s'\n", argv[4]);
		return -EINVAL;
	}
	raddr.ip = addr;
	raddr.port = NETPERF_PORT;

	ret = str_to_long(argv[5], &tmp);
	if (ret) {
		printf("couldn't parse [time] '%s'\n", argv[5]);
		return -EINVAL;
	}
	seconds = tmp;

	ret = str_to_long(argv[6], &tmp);
	if (ret) {
		printf("couldn't parse [payload_len] '%s'\n", argv[6]);
		return -EINVAL;
	}
	payload_len = tmp;

	ret = str_to_long(argv[7], &tmp);
	if (ret) {
		printf("couldn't parse [depth] '%s'\n", argv[7]);
		return -EINVAL;
	}
	depth = tmp;

	ret = runtime_init(argv[1], fn, NULL);
	if (ret) {
		printf("failed to start runtime\n");
		return ret;
	}

	return 0;
}
