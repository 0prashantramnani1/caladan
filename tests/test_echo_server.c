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
        laddr.port = 8082;

        //raddr.ip = 2154748472;
        //raddr.port = 8081;

        printf("DEBUG 1\n");
        udpconn_t** c;
        c = (udpconn_t **)malloc(sizeof(udpconn_t *));
        if(udp_listen(laddr, c) == 0) {
                printf("listen successful\n");
        } else printf("listen unsuccessful\n");

        char buff[6];
        //buff = (char *)malloc(MAX_PAYLOAD*sizeof(int));

        ssize_t size_sent = udp_read_from(*c, (void*)&buff , sizeof(buff), &raddr);

        if(size_sent < 0) {
                printf("read from failed\n");
        } else {
		printf("read %ld bytes \n", size_sent);
		printf("%s \n", buff);
	}
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
