/* tcp.c - TCP specific code for echo client */

/*
 * Copyright (c) 2017 Intel Corporation.
 * Copyright (c) 2018 Nordic Semiconductor ASA.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <logging/log.h>
LOG_MODULE_DECLARE(net_echo_client_sample, LOG_LEVEL_DBG);

#include <zephyr.h>
#include <errno.h>
#include <stdio.h>

#include <net/socket.h>

#include "common.h"

#define RECV_BUF_SIZE 128

static ssize_t sendall(const void *buf, size_t len)
{
	while (len) {
		ssize_t out_len = zsock_send(conf.sock, buf, len, 0);

		if (out_len < 0) {
			return out_len;
		}
		buf = (const char *)buf + out_len;
		len -= out_len;
	}

	return 0;
}

int send_tcp_data(void)
{
	int ret;

	do {
		conf.expecting = sys_rand32_get() % ipsum_len;
	} while (conf.expecting == 0);

	conf.received = 0U;

	ret =  sendall(lorem_ipsum, conf.expecting);

	if (ret < 0)
		LOG_ERR("TCP: Failed to send data, errno %d", errno);
	else
		LOG_DBG("TCP: Sent %d bytes", conf.expecting);

	return ret;
}

static int compare_tcp_data(const char *buf, u32_t received)
{
	if (conf.received + received > conf.expecting) {
		LOG_ERR("Too much data received: TCP");
		return -EIO;
	}

	if (memcmp(buf, lorem_ipsum + conf.received, received) != 0) {
		LOG_ERR("Invalid data received: TCP");
		return -EIO;
	}

	return 0;
}

static int start_tcp_proto(struct sockaddr *addr,
			   socklen_t addrlen)
{
	int ret;

	conf.sock = zsock_socket(addr->sa_family, SOCK_STREAM, IPPROTO_TCP);
	if (conf.sock < 0) {
		LOG_ERR("Failed to create TCP socket: %d", errno);
		return -errno;
	}

	ret = zsock_connect(conf.sock, addr, addrlen);
	if (ret < 0) {
		LOG_ERR("Cannot connect to TCP remote: %d", errno);
		ret = -errno;
	}

	return ret;
}

int process_tcp(void)
{
	int ret, received;
	char buf[RECV_BUF_SIZE];

	do {
		received = zsock_recv(conf.sock, buf, sizeof(buf),
		ZSOCK_MSG_DONTWAIT);

		/* No data or error. */
		if (received == 0) {
			ret = -EIO;
			continue;
		} else if (received < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				ret = 0;
			} else {
				ret = -errno;
			}
			continue;
		}

		ret = compare_tcp_data(buf, received);
		if (ret != 0) {
			break;
		}

		/* Successful comparison. */
		conf.received += received;
		if (conf.received < conf.expecting) {
			continue;
		}

		/* Response complete */
		LOG_DBG("TCP: Received and compared %d bytes, all ok",
			conf.received);


		if (++conf.counter % 1000 == 0) {
			LOG_INF("TCP: Exchanged %u packets", conf.counter);
		}
		k_sleep(1000);
		ret = send_tcp_data();
		break;
	} while (received > 0);

	return ret;
}

int start_tcp(void)
{
	int ret = 0;
	struct sockaddr_in6 addr6;

	addr6.sin6_family = AF_INET6;
	addr6.sin6_port = htons(PEER_PORT);
	ret = zsock_inet_pton(AF_INET6, CONFIG_NET_CONFIG_PEER_IPV6_ADDR,
			&addr6.sin6_addr);
	if ( ret <= 0)
		return -EFAULT;

	ret = start_tcp_proto((struct sockaddr *)&addr6,
				sizeof(addr6));
	if (ret < 0) {
		return ret;
	}

	return ret;
}

void stop_tcp(void)
{
	if (conf.sock >= 0) {
		(void)zsock_close(conf.sock);
	}
}
