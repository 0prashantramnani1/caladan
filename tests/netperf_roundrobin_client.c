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

#define NETPERF_PORT    8013

/* experiment parameters */
static struct netaddr raddr;
static int nflows;
static int nthreads;
static int seconds;
static uint64_t stop_us;
static size_t payload_len;
long long int reqs_threads[1000];
int ports;


#define BUF_SIZE        32768

struct client_rr_args {
        waitgroup_t *wg;
        uint64_t reqs;
        int id;
};

waitgroup_t wg;


static void do_client_poll(int id) {
    int i;
    uint64_t reqs = 0;

    tcpconn_t **c;
    c = (tcpconn_t **)malloc(nflows*sizeof(tcpconn_t *));
    struct netaddr laddr;
    ssize_t ret;

    /* local IP + ephemeral port */
    laddr.ip = 0;
    laddr.port = 0;

    for(int j=0;j<nflows;j++) {// Connections per thread
        raddr.port = NETPERF_PORT;//j%ports;
        printf("Dialing on port: %d\n", raddr.port);
        ret = tcp_dial(laddr, raddr, &c[j]);
        if(ret) {
            printf("Error in dialing\n");
            exit(1);
        }
        // usleep(100);
        // sleep(3);
    }

//     for(int j=0;j<nflows/2;j++) {// Connections per thread
//         raddr.port = NETPERF_PORT + 1;//j%ports;
//         printf("Dialing on port: %d\n", raddr.port);
//         ret = tcp_dial(laddr, raddr, &c[j]);
//         if(ret) {
//             printf("Error in dialing\n");
//             exit(1);
//         }
//         // usleep(100);
//         // sleep(1);
//     }

    // Required to synn up connections
    timer_sleep(ONE_SECOND);

    unsigned char buf[BUF_SIZE];
    memset(buf, 0xAA, payload_len);


    

    stop_us = microtime() + seconds * ONE_SECOND;
//     struct timespec sleeptime = {.tv_nsec=500};

    while (microtime() < stop_us) {
        for(int i=0;i<nflows;i++) {
            ret = tcp_write(c[i], buf, payload_len);
        //     nanosleep(100);    
        //     usleep(10);
        //     if (ret != payload_len) {
        //         log_err("index: %d tcp_write() failed, ret = %ld",i, ret);
        //         // break;
        //     }
        }
    }

    for(int i=0;i<nflows;i++) {
        tcp_abort(c[i]);
        tcp_close(c[i]);
        log_info("close port %hu", tcp_local_addr(c[i]).port);
    }
	waitgroup_done(&wg);
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
	// long long int total_reqs = 0;
	// for(int i=0;i<nthreads;i++) {
	// 	total_reqs += reqs_threads[i];
	// }
	// printf("Total reqs processed: %lld\n", total_reqs);
	// printf("Through Put: %lld\n", total_reqs/seconds);
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
	memset(reqs_threads, 0, 1000*sizeof(long long int));

    if (argc < 8) {
            printf("%s: [config_file_path] [nflows] [nthreads] [ip] [time] "
                    "[payload_len] [num_ports]\n", argv[0]);
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
            printf("couldn't parse [num_ports] '%s'\n", argv[6]);
            return -EINVAL;
    }
    ports = tmp;

    ret = runtime_init(argv[1], fn, NULL);
    if (ret) {
            printf("failed to start runtime\n");
            return ret;
    }

    return 0;
}
