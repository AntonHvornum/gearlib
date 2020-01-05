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
#ifndef AUDIO_DEF_H
#define AUDIO_DEF_H

#include <stdint.h>

/**
 * This file reference to ffmpeg and obs define
 */

#ifdef __cplusplus
extern "C" {
#endif

enum audio_format {
    AUDIO_FORMAT_UNKNOWN,

    AUDIO_FORMAT_U8BIT,
    AUDIO_FORMAT_16BIT,
    AUDIO_FORMAT_32BIT,
    AUDIO_FORMAT_FLOAT,

    AUDIO_FORMAT_U8BIT_PLANAR,
    AUDIO_FORMAT_16BIT_PLANAR,
    AUDIO_FORMAT_32BIT_PLANAR,
    AUDIO_FORMAT_FLOAT_PLANAR,
};

/**
 * This structure describes decoded (raw) audio data.
 */
#define AUDIO_MAX_CHANNELS 8
struct audio_frame {
    uint8_t *data[AUDIO_MAX_CHANNELS];
    uint32_t linesize[AUDIO_MAX_CHANNELS];
    int      format;
    int      nb_samples;
    uint64_t timestamp;//ns
    uint64_t size;
    uint64_t id;
    uint8_t **extended_data;
    void    *opaque;
};

enum audio_encode_format {
    AUDIO_ENCODE_AAC,

};

struct audio_packet {
    uint8_t                  *data;
    int                      size;
    enum audio_encode_format format;
    uint64_t                 pts;
};

struct audio_packet *audio_packet_create(void *data, size_t len);
void audio_packet_destroy(struct audio_packet *packet);

#ifdef __cplusplus
}
#endif
#endif
