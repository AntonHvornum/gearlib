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
#ifndef LIBPOSIX4RTTHREAD_H
#define LIBPOSIX4RTTHREAD_H

#include <stdbool.h>
#include "kernel_list.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * basic types
 ******************************************************************************/
//typedef int                       bool;

struct iovec {
    void *iov_base;
    size_t iov_len;
};

/*
 * below defined iovec to solve redefinition of 'struct iovec' of lwip
 */
//#define iovec iovec
ssize_t writev(int fd, const struct iovec *iov, int iovcnt);

/******************************************************************************
 * I/O string APIs
 ******************************************************************************/

/******************************************************************************
 * time APIs
 ******************************************************************************/

/******************************************************************************
 * pthread APIs
 ******************************************************************************/

/******************************************************************************
 * memory APIs
 ******************************************************************************/

#ifdef __cplusplus
}
#endif
#endif
