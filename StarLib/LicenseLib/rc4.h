/*
 * RC4 encryption/decryption/pseudo-random number generator
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef AVUTIL_RC4_H
#define AVUTIL_RC4_H

#include "stdint.h"

//typedef unsigned __int8   uint8_t;

struct AVRC4 {
    uint8_t state[256];
    int x, y;
};

/**
 * \brief Initializes an AVRC4 context.
 *
 * \param key_bits must be a multiple of 8
 * \param decrypt 0 for encryption, 1 for decryption, currently has no effect
 */
int av_rc4_init(struct AVRC4 *d, const uint8_t *key, int key_bits, int decrypt);

/**
 * \brief Encrypts / decrypts using the RC4 algorithm.
 *
 * \param count number of bytes
 * \param dst destination array, can be equal to src
 * \param src source array, can be equal to dst, may be NULL
 * \param iv not (yet) used for RC4, should be NULL
 * \param decrypt 0 for encryption, 1 for decryption, not (yet) used
 */
void av_rc4_crypt(struct AVRC4 *d, uint8_t *dst, const uint8_t *src, int count, uint8_t *iv, int decrypt);

#endif /* AVUTIL_RC4_H */
