/******************************************************************************
 * Copyright (C) 2014-2018 Zhifeng Gong <gozfree@163.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with libraries; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 ******************************************************************************/
#ifndef LIBRPC_H
#define LIBRPC_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * rpc_packet define (little endian)
 * [rpc_header][rpc_payload]
 *
 * rpc_header define
 * +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 * |7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                      destination_uuid=32                      |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                         source_uuid=32                        |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                         message_id=32                         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                                                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+ time_stamp=64 +-+-+-+-+-+-+-+-+-+-+-+-+
 * |                                                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                         payload_len=32                        |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                         checksum=32                           |
 * +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 *
 * uuid is hash_of(connect_ip_info), generated by rpc server
 *
 * connect_ip_info define
 * +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 * |7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                          socket_fd=32                         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                           ip_addr=32                          |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |            ip_port=16         |x x x x x x x x x x x x x x x x|
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * destination_uuid is message send to
 * source_uuid is message send from
 *
 * message_id define
 * +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 * |7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |  group_id=7 |unused=5 |R|D|P=2|         cmd_id=16             |
 * +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 *  [31~25]: group id
 *         - max support 128 group, can be used to service group
 *  [24~20]: unused
 *  [   19]: return indicator
 *         - 0: no need return
 *         - 1: need return
 *  [   18]: direction
 *         - 0: UP client to server
 *         - 1: DOWN server to client
 *  [17~16]: parser of payload message
 *         - 0 json
 *         - 1 protobuf
 *         - 2 unused
 *         - 3 unused
 *  [15~ 0]: cmd id, defined in librpc_stub.h
 *         - 0 ~ 7 inner cmd
 *         - 8 ~ 255 user cmd
 *
 * Note: how to add a new bit define, e.g. foo:
 *       1. add foo define in this commet;
 *       2. define RPC_FOO_BIT and RPC_FOO_MASK, and add into BUILD_RPC_MSG_ID;
 *       3. define GET_RPC_FOO;
 *       4. define enum of foo value;
 ******************************************************************************/

typedef struct rpc_header {
    uint32_t uuid_dst;
    uint32_t uuid_src;
    uint32_t msg_id;
    uint64_t time_stamp;
    uint32_t payload_len;
    uint32_t checksum;
} rpc_header_t;

typedef struct rpc_packet {
    struct rpc_header header;
    void *payload;
} rpc_packet_t;

enum rpc_state {
    rpc_inited,
    rpc_connected,
    rpc_disconnect,
};

typedef struct rpc {
    int fd;
    struct rpc_packet send_pkt;
    struct rpc_packet recv_pkt;
    struct gevent_base *evbase;
    struct gevent *ev;
    struct thread *dispatch_thread;
    enum rpc_state state;
} rpc_t;

typedef int (*rpc_callback)(struct rpc *r, void *arg, int len);

typedef struct msg_handler {
    uint32_t msg_id;
    rpc_callback cb;
} msg_handler_t;

struct rpc *rpc_create(const char *host, uint16_t port);
int rpc_set_cb(struct rpc *r,
        void (*on_read)(int fd, void *arg),
        void (*on_write)(int fd, void *arg),
        void (*on_error)(int fd, void *arg),  void *arg);
int rpc_echo(struct rpc *r, const void *buf, size_t len);
int rpc_call(struct rpc *r, uint32_t cmd_id,
            const void *in_arg, size_t in_len,
            void *out_arg, size_t out_len);
int rpc_peer_call(struct rpc *r, uint32_t uuid, uint32_t cmd_id,
            const void *in_arg, size_t in_len,
            void *out_arg, size_t out_len);


int rpc_send(struct rpc *r, const void *buf, size_t len);
struct iovec *rpc_recv_buf(struct rpc *r);
int rpc_dispatch(struct rpc *r);
void rpc_destroy(struct rpc *r);
int rpc_packet_parse(struct rpc *r);
msg_handler_t *find_msg_handler(uint32_t msg_id);
int process_msg(struct rpc *r, struct iovec *buf);
void dump_buffer(void *buf, int len);
void dump_packet(struct rpc_packet *r);
void print_packet(struct rpc_packet *r);


int register_msg_map(msg_handler_t *map, int num_entry);

#define RPC_REGISTER_MSG_MAP(map_name)             \
    register_msg_map(__msg_action_map##map_name,  \
                     (sizeof(__msg_action_map##map_name )/sizeof(msg_handler_t)));


#define BEGIN_RPC_MAP(map_name)  \
    static msg_handler_t  __msg_action_map##map_name[] = {
#define RPC_MAP(x, y) {x, y},
#define END_RPC_MAP() };


#define RPC_MSG_ID_MASK             0xFFFFFFFF

#define RPC_GROUP_BIT               (25)
#define RPC_GROUP_MASK              0x07

#define RPC_RET_BIT                 (19)
#define RPC_RET_MASK                0x01

#define RPC_DIR_BIT                 (18)
#define RPC_DIR_MASK                0x01

#define RPC_PARSE_BIT               (16)
#define RPC_PARSE_MASK              0x03

#define RPC_CMD_BIT                 (0)
#define RPC_CMD_MASK                0xFF

#define BUILD_RPC_MSG_ID(group, ret, dir, parse, cmd) \
    (((((uint32_t)group) & RPC_GROUP_MASK) << RPC_GROUP_BIT) | \
     ((((uint32_t)ret) & RPC_RET_MASK) << RPC_RET_BIT) | \
     ((((uint32_t)dir) & RPC_DIR_MASK) << RPC_DIR_BIT) | \
     ((((uint32_t)parse) & RPC_PARSE_MASK) << RPC_PARSE_BIT) | \
     ((((uint32_t)cmd) & RPC_CMD_MASK) << RPC_CMD_BIT))

#define IS_RPC_MSG_NEED_RETURN(cmd) \
        (((cmd & RPC_MSG_ID_MASK)>>RPC_RET_BIT) & RPC_RET_MASK)

#define GET_RPC_MSG_GROUP(cmd) \
        (((cmd & RPC_MSG_ID_MASK)>>RPC_GROUP_BIT) & RPC_GROUP_MASK)

#define GET_RPC_MSG_DIR(cmd) \
        (((cmd & RPC_MSG_ID_MASK)>>RPC_DIR_BIT) & RPC_DIR_MASK)

#define GET_RPC_MSG_PARSE(cmd) \
        (((cmd & RPC_MSG_ID_MASK)>>RPC_PARSE_BIT) & RPC_PARSE_MASK)


enum rpc_direction {
    _RPC_DIR_UP = 0,
    _RPC_DIR_DOWN = 1,
};

enum rpc_parser {
    _RPC_PARSE_JSON = 0,
    _RPC_PARSE_PROTOBUF = 1,
};

enum rpc_return {
    _RPC_NO_RETURN = 0,
    _RPC_NEED_RETURN = 1,
};

enum rpc_cmd_inner {
    _RPC_INNER_0    = 0,
    _RPC_INNER_1    = 1,
    _RPC_INNER_2    = 2,
    _RPC_INNER_3    = 3,
    _RPC_INNER_4    = 4,
    _RPC_INNER_5    = 5,
    _RPC_INNER_6    = 6,
    _RPC_INNER_7    = 7,
    _RPC_USER_BASE  = 8,
};


#ifdef __cplusplus
}
#endif
#endif
