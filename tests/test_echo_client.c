/*
 * test_echo_server.c - udp echo server
 */

#include <stdio.h>

#include <base/log.h>
#include <net/ping.h>
#include <runtime/runtime.h>
#include <runtime/timer.h>
#include <runtime/udp.h>
#include <runtime/net.h>

#define MAX_PAYLOAD 80

static void main_handler(void *arg)
{
	struct netaddr laddr, raddr;
	laddr.ip = 2154748472;
	laddr.port = 8081;

	raddr.ip = 2154748472;
	raddr.port = 8082;
	
	printf("DEBUG 1\n");
	udpconn_t** c;
	c = (udpconn_t **)malloc(sizeof(udpconn_t *));
	if(udp_dial(laddr, raddr, c) == 0) {
		printf("dial successful\n");
	} else printf("dial unsuccessful\n");
	
	char buff[6] = "hello";
	int n = 0;
	//buff = (char *)malloc(MAX_PAYLOAD*sizeof(int));

	//while ((buff[n++] = getchar()) != '\n');
	printf("message: %s  size: %d \n", buff, sizeof(buff));
	ssize_t size_sent = udp_write_to(*c, (void*)&buff , sizeof(buff), &raddr);
	//ssize_t size_sent_again = udp_send((void *)&buff, sizeof(buff), laddr, raddr);

	if(size_sent < 0) {
		printf("write to failed\n");
	} else printf("sent %ld bytes \n", size_sent);

	//if(size_sent_again < 0) {
	//	printf("write to failed again\n");
	//} else printf("sent %ld bytes agaun \n", size_sent_again);


}

int main(int argc, char *argv[])
{
	int ret;

	if (argc < 2) {
		printf("arg must be config file\n");
		return -EINVAL;
	}

	ret = runtime_init(argv[1], main_handler, NULL);
	if (ret) {
		printf("failed to start runtime\n");
		return ret;
	}

	return 0;
}

