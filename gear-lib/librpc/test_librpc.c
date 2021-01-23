/******************************************************************************
 * Copyright (C) 2014-2020 Zhifeng Gong <gozfree@163.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/
#include "librpc.h"
#include "librpc.h"
#include "librpc_stub.h"
#include <libthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

static struct thread *g_rpc_thread;
static struct rpc *g_rpc;
static struct rpcs *g_rpcs;

#define MAX_UUID_LEN                (21)

static int on_get_connect_list(struct rpc_session *r, void *arg, int len)
{
#if 0
    void *ptr;
    int num = 0;
    struct iovec *buf = CALLOC(1, struct iovec);
    key_list *tmp, *uuids;
    dict_get_key_list(r->dict_uuid2fd, &uuids);
    for (num = 0, tmp = uuids; tmp; tmp = tmp->next, ++num) {
    }
    uuids = NULL;
    buf->iov_len = num * MAX_UUID_LEN;
    buf->iov_base = calloc(1, buf->iov_len);
    for (ptr = buf->iov_base, tmp = uuids; tmp; tmp = tmp->next, ++num) {
        logi("uuid list: %s\n", (tmp->key));
        len = MAX_UUID_LEN;
        memcpy(ptr, tmp->key, len);
        ptr += len;
    }
    r->send_pkt.header.msg_id = RPC_GET_CONNECT_LIST;
    r->send_pkt.header.payload_len = buf->iov_len;
    logi("rpc_send len = %d, buf = %s\n", buf->iov_len, buf->iov_base);
    rpc_send(r, buf->iov_base, buf->iov_len);
#endif
    return 0;
}

static int on_get_connect_list_resp(struct rpc_session *r, void *arg, int len)
{
#if 0
    char *ptr;
    int num = 0;
    len = r->recv_pkt.header.payload_len;
    printf("strlen = %zu, %s\n", strlen((const char *)arg), (char *)arg);
    printf("on_get_connect_list, len = %d\n", r->recv_pkt.header.payload_len);
    num = len / MAX_UUID_LEN;
    printf("\n");
    for (ptr = (char *)arg; num > 0; --num) {
        printf("uuid list: %s\n", (char *)ptr);
        len = MAX_UUID_LEN;
        ptr += len;
    }
#endif
    return 0;
}

static int on_test(struct rpc_session *r, void *arg, int len)
{
    printf("on_test\n");
    return 0;
}

static int on_test_resp(struct rpc_session *r, void *arg, int len)
{
    printf("on_test_resp\n");
    return 0;
}

static int on_peer_post_msg(struct rpc_session *r, void *arg, int len)
{
#if 0
    char uuid_src[9];
    char uuid_dst[9];
    snprintf(uuid_src, sizeof(uuid_src), "%x", r->recv_pkt.header.uuid_src);
    snprintf(uuid_dst, sizeof(uuid_dst), "%x", r->recv_pkt.header.uuid_dst);

    printf("post msg from %s to %s\n", uuid_src, uuid_dst);
    char *valfd = (char *)hash_get(r->dict_uuid2fd, uuid_dst);
    if (!valfd) {
        printf("hash_get failed: key=%08x\n", r->send_pkt.header.uuid_dst);
        return -1;
    }
    int dst_fd = strtol(valfd, NULL, 16);
    //printf("dst_fd = %d\n", dst_fd);
    r->fd = dst_fd;
    r->send_pkt.header.msg_id = RPC_PEER_POST_MSG;
    return rpc_send(r, arg, len);
#else
    return 0;
#endif
}

static int on_peer_post_msg_resp(struct rpc_session *r, void *arg, int len)
{
    //printf("on_peer_post_msg_resp len = %d\n", len);
//    printf("msg from %x:\n%s\n", r->send_pkt.header.uuid_src, (char *)arg);
    return 0;
}

static int on_shell_help(struct rpc_session *r, void *arg, int len)
{
#if 0
    int ret;
    char buf[1024];
    char *cmd = (char *)arg;
    printf("on_shell_help cmd = %s\n", cmd);
    memset(buf, 0, sizeof(buf));
    ret = system_with_result(cmd, buf, sizeof(buf));
    printf("send len = %d, buf: %s\n", strlen(buf), buf);
    ret = rpc_send(r, buf, strlen(buf));
    loge("ret = %d, errno = %d\n", ret, errno);
#endif
    return 0;
}

static int on_shell_help_resp(struct rpc_session *r, void *arg, int len)
{
 //   printf("msg from %x:\n%s\n", r->send_pkt.header.uuid_src, (char *)arg);
    return 0;
}


BEGIN_RPC_MAP(RPC_CLIENT_API)
RPC_MAP(RPC_TEST, on_test_resp)
RPC_MAP(RPC_GET_CONNECT_LIST, on_get_connect_list_resp)
RPC_MAP(RPC_PEER_POST_MSG, on_peer_post_msg_resp)
RPC_MAP(RPC_SHELL_HELP, on_shell_help_resp)
END_RPC_MAP()

BEGIN_RPC_MAP(RPC_SERVER_API)
RPC_MAP(RPC_TEST, on_test)
RPC_MAP(RPC_GET_CONNECT_LIST, on_get_connect_list)
RPC_MAP(RPC_PEER_POST_MSG, on_peer_post_msg)
RPC_MAP(RPC_SHELL_HELP, on_shell_help)
END_RPC_MAP()


typedef struct rpc_connect {
//    char uuid[MAX_UUID_LEN];

} rpc_connect_t;


static int rpc_get_connect_list(struct rpc *r, struct rpc_connect *list, int *num)
{
    int len = 100;
    char *buf = (char *)calloc(1, len);
    memset(buf, 0xA5, len);
    rpc_call(r, RPC_GET_CONNECT_LIST, buf, len, NULL, 0);
    //printf("func_id = %x\n", RPC_GET_CONNECT_LIST);
    //dump_packet(&r->packet);
    return 0;
}

static int rpc_shell_help(struct rpc *r, void *buf, size_t len)
{
    char res[1024] = {0};
    rpc_call(r, RPC_SHELL_HELP, buf, len, res, sizeof(res));
    printf("return buffer\n%s", res);
    //dump_packet(&r->packet);
    return 0;
}

static int rpc_peer_post_msg(struct rpc *r, void *buf, size_t len)
{
    rpc_call(r, RPC_PEER_POST_MSG, buf, len, NULL, 0);
    //printf("func_id = %x\n", RPC_PEER_POST_MSG);
    //dump_packet(&r->packet);
    return 0;
}

static void usage(void)
{
    fprintf(stderr, "./test_libskt -s <port>\n");
    fprintf(stderr, "./test_libskt -c <ip> <port>\n");
    fprintf(stderr, "e.g. ./test_libskt 116.228.149.106 12345\n");
}

static void cmd_usage(void)
{
    printf("====rpc cmd====\n"
            "a: get all connect list\n"
            "p: post message to peer\n"
            "s: remote shell help\n"
            "q: quit\n"
            "\n");
}

static void *rpc_client_thread(struct thread *t, void *arg)
{
    struct rpc *r = (struct rpc *)arg;
    uint32_t uuid_dst;
    char cmd[512];
    int loop = 1;
    int i;
    int len = 1024;
    char ch;
    char *buf = (char *)calloc(1, len);
    for (i = 0; i < len; i++) {
        buf[i] = i;
    }

    RPC_REGISTER_MSG_MAP(RPC_CLIENT_API);

    while (loop) {
        memset(buf, 0, len);
        printf("input cmd> ");
        ch = getchar();
        switch (ch) {
        case 'a':
            rpc_get_connect_list(r, NULL, NULL);
            break;
        case 'p':
            printf("input uuid_dst> ");
            scanf("%x", &uuid_dst);
            printf("uuid_dst = %x\n", uuid_dst);
            //r->send_pkt.header.uuid_dst = uuid_dst;
            sprintf(buf, "%s", "hello world");
            rpc_peer_post_msg(r, buf, 12);
            break;
        case 'q':
            loop = 0;
            rpc_client_destroy(r);
            break;
        case 's':
            printf("input shell cmd> ");
            scanf("%c", &ch);
            scanf("%[^\n]", cmd);
            rpc_shell_help(r, cmd, sizeof(cmd));
            break;
        case 'h':
            cmd_usage();
            break;
        default:
            break;
        }
        //dump_buffer(buf, len);
    }

    exit(0);
    return NULL;
}

static int rpc_client_test(char *ip, uint16_t port)
{
    g_rpc = rpc_client_create(ip, port);
    if (!g_rpc) {
        printf("rpc_client_create failed\n");
        return -1;
    }
    g_rpc_thread = thread_create(rpc_client_thread, g_rpc);
    while (1) {
        sleep(1);
    }
    return 0;
}

static int rpc_server_test(uint16_t port)
{
    g_rpcs = rpc_server_create(NULL, port);
    if (g_rpcs == NULL) {
        printf("rpc_server_create failed!\n");
        return -1;
    }
    RPC_REGISTER_MSG_MAP(RPC_SERVER_API);
    while (1) {
        sleep(1);
    }
    return 0;
}

int main(int argc, char **argv)
{
    uint16_t port;
    char *ip;
    if (argc < 2) {
        usage();
        exit(0);
    }
    if (!strcmp(argv[1], "-s") && argc > 2) {
        port = atoi(argv[2]);
        rpc_server_test(port);
    } else if (!strcmp(argv[1], "-c") && argc > 3) {
        ip = argv[2];
        port = atoi(argv[3]);
        rpc_client_test(ip, port);
    } else {
        usage();
        exit(0);
    }
    return 0;
}

