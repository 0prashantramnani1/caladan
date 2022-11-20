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

#define NETPERF_PORT	8013

/* experiment parameters */
static struct netaddr raddr;
static int nthreads;
static int seconds;
static uint64_t stop_us;
static size_t payload_len;
static int depth;

#define BUF_SIZE	32768

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

	//Creating a trigger for this queue
        poll_trigger_t *t;
        ret = create_trigger(&t);

        struct list_head *h;
        h = tcpqueue_get_triggers(q);

	// Passing tcp_accept as a call back function	
	tcparg_t *accept_arg;
	tcpconn_t *c;

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
			// printf("RET IS NOT 0\n");
			if(c != NULL) {
				// Won't work if the server gets 2 connections 
				// together. 
				printf("Connection Accepted - thread_id: %d - raddr_port: %d \n", id, tcp_get_raddr_port(c));
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
		}
	}
}

void start_server_threads(void *arg) {

	for(int i=0;i<nthreads;i++) {
		int ret = thread_spawn(do_server, i);
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

	if (argc < 4) {
		printf("%s: [config_file_path] [nthreads] [payload_len]\n", argv[0]);
		return -EINVAL;
	}
	
	fn = start_server_threads;

	ret = str_to_long(argv[2], &tmp);
	if (ret) {
		printf("couldn't parse [nthreads] '%s'\n", argv[3]);
		return -EINVAL;
	}
	nthreads = tmp;

	ret = str_to_long(argv[3], &tmp);
	if (ret) {
		printf("couldn't parse [payload_len] '%s'\n", argv[6]);
		return -EINVAL;
	}
	payload_len = tmp;

	ret = runtime_init(argv[1], fn, NULL);
	if (ret) {
		printf("failed to start runtime\n");
		return ret;
	}

	return 0;
}
