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
        int id;
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

        memset(buf, 0xAA, payload_len);
        raddr.port = NETPERF_PORT + args->id;
        ret = tcp_dial(laddr, raddr, &c);
        timer_sleep(1*ONE_SECOND);
        if (ret) {
                log_err("tcp_dial() failed, ret = %ld", ret);
                goto done;
        }

        while (microtime() < stop_us) {
                unsigned char read_buf[BUF_SIZE];
                while (budget) {
                        ret = tcp_write(c, buf, payload_len);
                        if (ret != payload_len) {
                                printf("error in writing \n");
                                log_err("tcp_write() failed, ret = %ld", ret);
                                break;
                        }
                        budget--;
                }
                ret = tcp_read(c, read_buf, payload_len * depth);

                if(read_buf[0] != buf[0]) {
                        printf("client_worker: read and write buf are not same\n");
                }
                //else {
                //      printf("client_worker: they don't match\n");
                //}

                //printf("client_worker: %d\n", read_buf[0]);
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

        arg_tbl = calloc(nworkers*10, sizeof(*arg_tbl));
        BUG_ON(!arg_tbl);

        waitgroup_init(&wg);
        waitgroup_add(&wg, nworkers*10);
        stop_us = microtime() + seconds * ONE_SECOND;
        for (i = 0; i < nworkers; i++) {
                for(int j=0;j<10;j++) {
                        int k = i*10 + j;
                        arg_tbl[k].wg = &wg;
                        arg_tbl[k].reqs = 0;
                        arg_tbl[k].id = i;
                        ret = thread_spawn(client_worker, &arg_tbl[k]);
                        timer_sleep(1*ONE_SECOND);
                        BUG_ON(ret);
                }
        }

        waitgroup_wait(&wg);

        for (i = 0; i < nworkers*10; i++)
                reqs += arg_tbl[i].reqs;

        log_info("measured %f reqs/s", (double)reqs / seconds);
}

static void do_client_poll(int id)
{
        int i;
        uint64_t reqs = 0;

        tcpconn_t **c;
        c = (tcpconn_t **)malloc(10*sizeof(tcpconn_t *));
        struct netaddr laddr;
        ssize_t ret;
        int budget = depth;

        /* local IP + ephemeral port */
        laddr.ip = 0;
        laddr.port = 0;


        poll_waiter_t *w;
        ret = create_waiter(&w);

	for(int j=0;j<10;j++) { // Connections per thread
		raddr.port = NETPERF_PORT + id;
		ret = tcp_dial(laddr, raddr, &c[j]);
		timer_sleep(ONE_SECOND);
		tcp_set_nonblocking(c[j], 1);

		//Creating a trigger for this socket
		poll_trigger_t *t;
		ret = create_trigger(&t);

		struct list_head *h;
		h = tcp_get_triggers(c[j]);

		unsigned char buf[BUF_SIZE];
		memset(buf, 0xAA, payload_len);

		tcp_read_arg_t *read_arg;
		read_arg = (tcp_read_arg_t *)malloc(sizeof(tcp_read_arg_t)); 
		read_arg->c = c[j];
		read_arg->buf = buf;
		read_arg->len = BUF_SIZE;

		sh_event_callback_fn cb = &tcp_read_poll;

		//register trigger with a waiter
		poll_arm_w_sock(w, h, t, SEV_READ, cb, read_arg, c[j], -7);

		ret = tcp_write(c[j], buf, payload_len);
		if (ret != payload_len) {
			printf("error in writing \n");
			log_err("tcp_write() failed, ret = %ld", ret);
			break;
		}

	}

        stop_us = microtime() + seconds * ONE_SECOND;
        while (microtime() < stop_us) {
		poll_cb_once(w);
	}
	for(int i=0;i<10;i++) {
        	tcp_abort(c[i]);
        	tcp_close(c[i]);
        	log_info("close port %hu", tcp_local_addr(c[i]).port);
	}
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

		if(ret == 0) {
			read = tcp_read(c, buf, BUF_SIZE);
		}
			
		if(read > 0 ) {
			//timer_sleep(5*ONE_SECOND);
			write = tcp_write(c, buf, read);
			read = 0;
		} 
		t->triggered = false;	
	}

	tcp_close(c);
}

static void do_server(int id)
{
	struct netaddr laddr;
	tcpqueue_t *q;
	int ret;

	laddr.ip = 0;
	laddr.port = NETPERF_PORT + id;

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

	// Passing tcp_accept as a call back function	
	tcpconn_t *c;
	tcparg_t *accept_arg;

	accept_arg = (tcparg_t *)malloc(sizeof(tcparg_t)); 
	accept_arg->q = q;
	accept_arg->c = &c;
	
	sh_event_callback_fn cb = &tcp_accept_poll;

        //register trigger with a waiter
        poll_arm_w_queue(w, h, t, SEV_READ, cb, accept_arg, q, -7);

	while (true) {
		c = NULL;
		ret = poll_cb_once(w); 
		if(ret == 0) {
			if(c != NULL) {
				// Won't work if the server gets 2 connections 
				// together. 
				printf("Connection Accepted\n");
				printf("thread_id: %d\n", id);
				tcpconn_t *c_tmp = c;
				//ret = thread_spawn(server_worker, c_tmp);
				
				//continue;		
				tcp_set_nonblocking(c_tmp, 1);
				poll_trigger_t *t_tmp;
				ret = create_trigger(&t_tmp);
	
				h = tcp_get_triggers(c_tmp);
				
				unsigned char buf[BUF_SIZE];

				tcp_read_arg_t *read_arg;
				read_arg = (tcp_read_arg_t *)malloc(sizeof(tcp_read_arg_t)); 
				read_arg->c = c_tmp;
				read_arg->buf = buf;
				read_arg->len = BUF_SIZE;

				sh_event_callback_fn cb = &tcp_read_poll;

				//register trigger with a waiter
				poll_arm_w_sock(w, h, t_tmp, SEV_READ, cb, read_arg, c_tmp, -7);

			}
			//ret = tcp_accept(q, &c);
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

void start_server_threads(void *arg) {
	for(int i=0;i<nworkers;i++) {
		int ret = thread_spawn(do_server, i);
	}

	while(true);
}

void start_client_threads(void *arg) {
	for(int i=0;i<nworkers;i++) {
		int ret = thread_spawn(do_client_poll, i);
	}

	while(true);
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
		//fn = do_client_poll;
		fn = start_client_threads;
	} else if (!strcmp(argv[2], "SERVER")) {
		//fn = do_server;
		fn = start_server_threads;
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
