/*
 * DES encryption/decryption
 * Copyright (c) 2007 Reimar Doeffinger
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

#ifndef AVUTIL_DES_H
#define AVUTIL_DES_H

#include "stdint.h"

struct AVDES {
    uint64_t round_keys[3][16];
    int triple_des;
};

/**
 * \brief Initializes an AVDES context.
 *
 * \param key_bits must be 64 or 192
 * \param decrypt 0 for encryption, 1 for decryption
 */
int av_des_init(struct AVDES *d, const uint8_t *key, int key_bits, int decrypt);

/**
 * \brief Encrypts / decrypts using the DES algorithm.
 *
 * \param count number of 8 byte blocks
 * \param dst destination array, can be equal to src, must be 8-byte aligned
 * \param src source array, can be equal to dst, must be 8-byte aligned, may be NULL
 * \param iv initialization vector for CBC mode, if NULL then ECB will be used,
 *           must be 8-byte aligned
 * \param decrypt 0 for encryption, 1 for decryption
 */
void av_des_crypt(struct AVDES *d, uint8_t *dst, const uint8_t *src, int count, uint8_t *iv, int decrypt);

#endif /* AVUTIL_DES_H */
