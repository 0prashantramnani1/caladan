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

#define NETPERF_PORT    8000

/* experiment parameters */
static struct netaddr raddr;
static int nflows;
static int nthreads;
static int seconds;
static uint64_t stop_us;
static size_t payload_len;
static int depth;
long long int reqs_threads[1000];

#define BUF_SIZE        32768

struct client_rr_args {
        waitgroup_t *wg;
        uint64_t reqs;
        int id;
};

waitgroup_t wg;


static void do_client_poll(int id)
{
        int i;
        uint64_t reqs = 0;

        tcpconn_t **c;
	//TODO: Removing this "10" hard coding
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

		//TODO: Ask prof about this timer
		// Required to synn up connections
                timer_sleep(ONE_SECOND);

                tcp_set_nonblocking(c[j], 1);

                //Creating a trigger for this socket
                poll_trigger_t *t;
                ret = create_trigger(&t);

                struct list_head *h;
                h = tcp_get_triggers(c[j]);

                unsigned char buf[BUF_SIZE];
                memset(buf, 0xAA, payload_len);

		// Defining the inpute to tcp_read_pol
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
                        log_err("tcp_write() failed, ret = %ld", ret);
                        break;
                }

        }

        stop_us = microtime() + seconds * ONE_SECOND;

        while (microtime() < stop_us) {
                poll_cb_once(w);
        }
	
        for(int i=0;i<10;i++) {
		reqs_threads[id] = tcp_get_reqs(c[i]);
                tcp_abort(c[i]);
                tcp_close(c[i]);
		
		waitgroup_done(&wg);

                log_info("close port %hu", tcp_local_addr(c[i]).port);
        }
}
void start_client_threads(void *args) {
	waitgroup_init(&wg);
	waitgroup_add(&wg, nthreads);

	for(int i=0;i<nthreads;i++) {
                int ret = thread_spawn(do_client_poll, i);
		BUG_ON(ret);
        }

	waitgroup_wait(&wg);

	printf("STATS\n");
	long long int total_reqs = 0;
	for(int i=0;i<nthreads;i++) {
		total_reqs += reqs_threads[i];
	}
	printf("Total reqs processed: %lld", total_reqs);
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
                printf("%s: [config_file_path] [nflows] [nthreads] [ip] [time] "
                       "[payload_len] [depth]\n", argv[0]);
                return -EINVAL;
        }

        fn = start_client_threads;

        ret = str_to_long(argv[2], &tmp);
        if (ret) {
                printf("couldn't parse [nflows] '%s'\n", argv[3]);
                return -EINVAL;
        }
        nflows = tmp;

        ret = str_to_long(argv[3], &tmp);
        if (ret) {
                printf("couldn't parse [nthreads] '%s'\n", argv[3]);
                return -EINVAL;
        }
        nthreads = tmp;

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
