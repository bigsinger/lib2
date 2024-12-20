/*
 * Copyright (C) 2006 Michael Niedermayer (michaelni@gmx.at)
 * Copyright (C) 2003-2005 by Christopher R. Hertel (crh@ubiqx.mn.org)
 *
 * References:
 *  IETF RFC 1321: The MD5 Message-Digest Algorithm
 *       Ron Rivest. IETF, April, 1992
 *
 * based on http://ubiqx.org/libcifs/source/Auth/MD5.c
 *          from Christopher R. Hertel (crh@ubiqx.mn.org)
 * Simplified, cleaned and IMO redundant comments removed by michael.
 *
 * If you use gcc, then version 4.1 or later and -fomit-frame-pointer is
 * strongly recommended.
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

#include <string.h>
#include "md5.h"

#ifndef bswap_32
static __inline const uint32_t bswap_32(uint32_t x)
{
    x= ((x<<8)&0xFF00FF00) | ((x>>8)&0x00FF00FF);
    x= (x>>16) | (x<<16);
    return x;
}
#endif
#ifndef bswap_64
static __inline uint64_t const bswap_64(uint64_t x)
{
#if 0
    x= ((x<< 8)&0xFF00FF00FF00FF00ULL) | ((x>> 8)&0x00FF00FF00FF00FFULL);
    x= ((x<<16)&0xFFFF0000FFFF0000ULL) | ((x>>16)&0x0000FFFF0000FFFFULL);
    return (x>>32) | (x<<32);
#else
    union {
        uint64_t ll;
        uint32_t l[2];
    } w, r;
    w.ll = x;
    r.l[0] = bswap_32 (w.l[1]);
    r.l[1] = bswap_32 (w.l[0]);
    return r.ll;
#endif
}
#endif
#define le2me_32(x) bswap_32(x)
#define le2me_64(x) bswap_64(x)



typedef struct AVMD5{
    uint64_t len;
    uint8_t  block[64];
    uint32_t ABCD[4];
} AVMD5;

const int av_md5_size= sizeof(AVMD5);

static const uint8_t S[4][4] = {
    { 7, 12, 17, 22 },  /* Round 1 */
    { 5,  9, 14, 20 },  /* Round 2 */
    { 4, 11, 16, 23 },  /* Round 3 */
    { 6, 10, 15, 21 }   /* Round 4 */
};

static const uint32_t T[64] = { // T[i]= fabs(sin(i+1)<<32)
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,   /* Round 1 */
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,

    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,   /* Round 2 */
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,

    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,   /* Round 3 */
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,

    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,   /* Round 4 */
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391,
};

#define CORE(i, a, b, c, d) \
        t = S[i>>4][i&3];\
        a += T[i];\
\
        if(i<32){\
            if(i<16) a += (d ^ (b&(c^d))) + X[      i &15 ];\
            else     a += (c ^ (d&(c^b))) + X[ (1+5*i)&15 ];\
        }else{\
            if(i<48) a += (b^c^d)         + X[ (5+3*i)&15 ];\
            else     a += (c^(b|~d))      + X[ (  7*i)&15 ];\
        }\
        a = b + (( a << t ) | ( a >> (32 - t) ));

static void body(uint32_t ABCD[4], uint32_t X[16]){

    int t;
//    int i;
    unsigned int a= ABCD[3];
    unsigned int b= ABCD[2];
    unsigned int c= ABCD[1];
    unsigned int d= ABCD[0];

#ifdef WORDS_BIGENDIAN
    for(i=0; i<16; i++)
        X[i]= bswap_32(X[i]);
#endif

#ifdef CONFIG_SMALL
    for( i = 0; i < 64; i++ ){
        CORE(i,a,b,c,d)
        t=d; d=c; c=b; b=a; a=t;
    }
#else
#define CORE2(i) CORE(i,a,b,c,d) CORE((i+1),d,a,b,c) CORE((i+2),c,d,a,b) CORE((i+3),b,c,d,a)
#define CORE4(i) CORE2(i) CORE2((i+4)) CORE2((i+8)) CORE2((i+12))
CORE4(0) CORE4(16) CORE4(32) CORE4(48)
#endif

    ABCD[0] += d;
    ABCD[1] += c;
    ABCD[2] += b;
   // ABCD[3] += a;
}

void av_md5_init(AVMD5 *ctx){
    ctx->len    = 0;

    ctx->ABCD[0] = 0x9e3779b9;
    ctx->ABCD[1] = 0x98badcfe;
    ctx->ABCD[2] = 0xefcdab89;
    ctx->ABCD[3] = 0x20090501;
}

void av_md5_update(AVMD5 *ctx, const uint8_t *src, const int len){
    int i, j;

    j= ctx->len & 63;
    ctx->len += len;

    for( i = 0; i < len; i++ ){
        ctx->block[j++] = src[i];
        if( 64 == j ){
            body(ctx->ABCD, (uint32_t*) ctx->block);
            j = 0;
        }
    }
}

void av_md5_final(AVMD5 *ctx, uint8_t *dst){
    int i;
    uint64_t finalcount= le2me_64(ctx->len<<3);

    av_md5_update(ctx, "\200", 1);
    while((ctx->len & 63)<56)
        av_md5_update(ctx, "", 1);

    av_md5_update(ctx, (uint8_t*)&finalcount, 8);

    for(i=0; i<4; i++)
        ((uint32_t*)dst)[i]= le2me_32(ctx->ABCD[3-i]);
}

void av_md5_sum(uint8_t *dst, const uint8_t *src, const int len){
    AVMD5 ctx[1];

    av_md5_init(ctx);
    av_md5_update(ctx, src, len);
    av_md5_final(ctx, dst);
}

#ifdef TEST
#include <stdio.h>
#undef printf
int main(void){
    uint64_t md5val;
    int i;
    uint8_t in[1000];

    for(i=0; i<1000; i++) in[i]= i*i;
    av_md5_sum( (uint8_t*)&md5val, in,  1000); printf("%"PRId64"\n", md5val);
    av_md5_sum( (uint8_t*)&md5val, in,  63); printf("%"PRId64"\n", md5val);
    av_md5_sum( (uint8_t*)&md5val, in,  64); printf("%"PRId64"\n", md5val);
    av_md5_sum( (uint8_t*)&md5val, in,  65); printf("%"PRId64"\n", md5val);
    for(i=0; i<1000; i++) in[i]= i % 127;
    av_md5_sum( (uint8_t*)&md5val, in,  999); printf("%"PRId64"\n", md5val);

    return 0;
}
#endif
