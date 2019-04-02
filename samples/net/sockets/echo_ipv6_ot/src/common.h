/*
 * Copyright (c) 2017 Intel Corporation.
 * Copyright (c) 2018 Nordic Semiconductor ASA.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Value of 0 will cause the IP stack to select next free port */
#define MY_PORT 0

#define PEER_PORT 4242

struct data {
	int sock;
	u32_t expecting;
	u32_t received;
	u32_t counter;
};

extern const char lorem_ipsum[];
extern const int ipsum_len;
extern struct data conf;

int start_udp(void);
int process_udp(void);
void stop_udp(void);

int start_tcp(void);
int process_tcp(void);
void stop_tcp(void);

int send_tcp_data(void);
