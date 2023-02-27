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

#define NETPERF_PORT	8014


/* experiment parameters */
static struct netaddr raddr;
static int nthreads;
static int nflows;
static int seconds;
static uint64_t stop_us;
static size_t payload_len;
static int depth;
static int nruntimes;

#define BUF_SIZE	32768

waitgroup_t wg;


static void do_server(int id)
{
	struct netaddr laddr;
	tcpqueue_t *q;
	tcpconn_t *c[nflows];

	unsigned long ret;
	
	laddr.ip = 0;
	laddr.port = NETPERF_PORT + id;

	ret = tcp_listen(laddr, 4096, &q);

	for(int i=0;i<nflows;i++) {
		ret = tcp_accept(q, &c[i]);
		printf("%d Connection Accepted - thread_id: %d - raddr_port: %d \n", i+1, id, tcp_get_raddr_port(c[i]));
		tcp_set_nonblocking(c[i], 1);
	}

	unsigned long total_bytes = 0;

	unsigned long bytes_read[nflows];
	for(int i=0;i < nflows; i++) bytes_read[i] = 0;

	unsigned char buf[BUF_SIZE];

	stop_us = microtime() + 10 * ONE_SECOND;
	while (microtime() < stop_us) {
		for(int i=0;i<nflows;i++) {
			ret = tcp_read(c[i], buf, BUF_SIZE);
			// if(ret < 0) {
			// 	printf("ERROR IN READING\n");
			// }
			total_bytes += ret;
			bytes_read[i] += ret;
		}
	}
	
	printf("Flow data: \n");
	for(int i=0;i<nflows;i++) {
		printf("index: %d - bytes_read %ld\n", i, bytes_read[i]);
	}
	printf("TOTAL_BYTES %ld\n", total_bytes);
	printf("Throughput: %ld Gbps\n", (total_bytes*8)/((unsigned long)1024*1024*1024*10));

	waitgroup_done(&wg);
}

void start_server_threads(void *arg) {
	printf("Runtime id: %d\n", *((int *)arg));
	waitgroup_init(&wg);
	waitgroup_add(&wg, nthreads);

	// for(int i=0;i<nthreads;i++) {
	int ret = thread_spawn(do_server, *((int *)arg));
	// }

	// while(true);
	waitgroup_wait(&wg);
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

	if (argc < 5) {
		printf("%s: [config_file_path] [nruntimes] [nflows] [nthreads] [payload_len]\n", argv[0]);
		return -EINVAL;
	}
	
	fn = start_server_threads;

	ret = str_to_long(argv[2], &tmp);
	if (ret) {
		printf("couldn't parse [nruntimes] '%s'\n", argv[3]);
		return -EINVAL;
	}
	nruntimes = tmp;

	ret = str_to_long(argv[3], &tmp);
	if (ret) {
		printf("couldn't parse [nthreads] '%s'\n", argv[3]);
		return -EINVAL;
	}
	nflows = tmp;

	ret = str_to_long(argv[4], &tmp);
	if (ret) {
		printf("couldn't parse [payload_len] '%s'\n", argv[6]);
		return -EINVAL;
	}
	nthreads = tmp;

	ret = str_to_long(argv[5], &tmp);
	if (ret) {
		printf("couldn't parse [payload_len] '%s'\n", argv[6]);
		return -EINVAL;
	}
	payload_len = tmp;

	int runtime = 0;

	// for (int i = 0; i < nruntimes; i++) {
	// 	int pid = fork();
	// 	BUG_ON(pid == -1);

	// 	if (pid == 0) {
	// 		ret = runtime_init(argv[1], fn, &i);
	// 		BUG_ON(ret < 0);
	// 	}
	// 	runtime ++;
	// 	sleep(1);
	// }

	ret = runtime_init(argv[1], fn, &runtime);
	if (ret) {
		printf("failed to start runtime\n");
		return ret;
	}

	return 0;
}
