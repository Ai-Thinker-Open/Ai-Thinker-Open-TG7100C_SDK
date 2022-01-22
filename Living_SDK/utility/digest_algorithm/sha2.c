/*
 * FILE:    sha2.c
 * AUTHOR:  Aaron D. Gifford - http://www.aarongifford.com/
 *
 * Copyright (c) 2000-2001, Aaron D. Gifford
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTOR(S) ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTOR(S) BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id: sha2.c,v 1.1 2001/11/08 00:01:51 adg Exp adg $
 */
#include <stdint.h>
#include <string.h>

extern uint32_t os_be32toh(uint32_t data);
extern uint32_t os_htobe32(uint32_t data);
extern uint64_t os_be64toh(uint64_t data);
extern uint64_t os_htobe64(uint64_t data);

/*** SHA-256/384/512 Various Length Definitions ***********************/
/*** SHA-256/384/512 Various Length Definitions ***********************/
#define SHA256_BLOCK_LENGTH     64
#define SHA256_DIGEST_LENGTH    32
#define SHA256_DIGEST_STRING_LENGTH (SHA256_DIGEST_LENGTH * 2 + 1)
#define SHA384_BLOCK_LENGTH     128
#define SHA384_DIGEST_LENGTH    48
#define SHA384_DIGEST_STRING_LENGTH (SHA384_DIGEST_LENGTH * 2 + 1)
#define SHA512_BLOCK_LENGTH     128
#define SHA512_DIGEST_LENGTH    64
#define SHA512_DIGEST_STRING_LENGTH (SHA512_DIGEST_LENGTH * 2 + 1)

/* NOTE: Most of these are in sha2.h */
#define SHA256_SHORT_BLOCK_LENGTH   (SHA256_BLOCK_LENGTH - 8)
#define SHA384_SHORT_BLOCK_LENGTH   (SHA384_BLOCK_LENGTH - 16)
#define SHA512_SHORT_BLOCK_LENGTH   (SHA512_BLOCK_LENGTH - 16)

typedef uint8_t sha2_byte;          /* Exactly 1 byte */
typedef uint32_t sha2_word32;       /* Exactly 4 bytes */
typedef uint64_t sha2_word64;       /* Exactly 8 bytes */

typedef struct _SHA256_CTX {
    uint32_t state[8];
    uint64_t bitcount;
    uint8_t buffer[SHA256_BLOCK_LENGTH];
} SHA256_CTX;

typedef struct _SHA512_CTX {
    uint64_t state[8];
    uint64_t bitcount[2];
    uint8_t buffer[SHA512_BLOCK_LENGTH];
} SHA512_CTX;

typedef SHA512_CTX SHA384_CTX;

static void SHA256_Init(SHA256_CTX *);
static void SHA256_Update(SHA256_CTX *, const uint8_t *, uint32_t);
static void SHA256_Final(uint8_t[SHA256_DIGEST_LENGTH], SHA256_CTX *);

static void SHA384_Init(SHA384_CTX *);
static void SHA384_Update(SHA384_CTX *, const uint8_t *, uint32_t);
static void SHA384_Final(uint8_t[SHA384_DIGEST_LENGTH], SHA384_CTX *);

static void SHA512_Init(SHA512_CTX *);
static void SHA512_Update(SHA512_CTX *, const uint8_t *, uint32_t);
static void SHA512_Final(uint8_t[SHA512_DIGEST_LENGTH], SHA512_CTX *);


/*
 * Macro for incrementally adding the unsigned 64-bit integer n to the
 * unsigned 128-bit integer (represented using a two-element array of
 * 64-bit words):
 */
#define ADDINC128(w,n)  { \
    (w)[0] += (sha2_word64)(n); \
    if ((w)[0] < (n)) { \
        (w)[1]++; \
    } \
}

#define MEMSET_BZERO(p,l)   memset((p), 0, (l))
#define MEMCPY_BCOPY(d,s,l) memcpy((d), (s), (l))

/*** THE SIX LOGICAL FUNCTIONS ****************************************/
/*
 * Bit shifting and rotation (used by the six SHA-XYZ logical functions:
 *
 *   NOTE:  The naming of R and S appears backwards here (R is a SHIFT and
 *   S is a ROTATION) because the SHA-256/384/512 description document
 *   (see http://csrc.nist.gov/cryptval/shs/sha256-384-512.pdf) uses this
 *   same "backwards" definition.
 */
/* Shift-right (used in SHA-256, SHA-384, and SHA-512): */
#define R(b,x)      ((x) >> (b))
/* 32-bit Rotate-right (used in SHA-256): */
#define _S32(b,x)   (((x) >> (b)) | ((x) << (32 - (b))))
/* 64-bit Rotate-right (used in SHA-384 and SHA-512): */
#define S64(b,x)    (((x) >> (b)) | ((x) << (64 - (b))))

/* Two of six logical functions used in SHA-256, SHA-384, and SHA-512: */
#define Ch(x,y,z)   (((x) & (y)) ^ ((~(x)) & (z)))
#define Maj(x,y,z)  (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

/* Four of six logical functions used in SHA-256: */
#define Sigma0_256(x)   (_S32(2,  (x)) ^ _S32(13, (x)) ^ _S32(22, (x)))
#define Sigma1_256(x)   (_S32(6,  (x)) ^ _S32(11, (x)) ^ _S32(25, (x)))
#define sigma0_256(x)   (_S32(7,  (x)) ^ _S32(18, (x)) ^ R(3 ,   (x)))
#define sigma1_256(x)   (_S32(17, (x)) ^ _S32(19, (x)) ^ R(10,   (x)))

/* Four of six logical functions used in SHA-384 and SHA-512: */
#define Sigma0_512(x)   (S64(28, (x)) ^ S64(34, (x)) ^ S64(39, (x)))
#define Sigma1_512(x)   (S64(14, (x)) ^ S64(18, (x)) ^ S64(41, (x)))
#define sigma0_512(x)   (S64( 1, (x)) ^ S64( 8, (x)) ^ R( 7,   (x)))
#define sigma1_512(x)   (S64(19, (x)) ^ S64(61, (x)) ^ R( 6,   (x)))

/*** INTERNAL FUNCTION PROTOTYPES *************************************/
/* NOTE: These should not be accessed directly from outside this
 * library -- they are intended for private internal visibility/use
 * only.
 */
static void SHA512_Last(SHA512_CTX *);
static void SHA256_Transform(SHA256_CTX *, const sha2_word32 *);
static void SHA512_Transform(SHA512_CTX *, const sha2_word64 *);

/*** SHA-XYZ INITIAL HASH VALUES AND CONSTANTS ************************/
/* Hash constant words K for SHA-256: */
static const sha2_word32 K256[64] = {
    0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL,
    0x3956c25bUL, 0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL,
    0xd807aa98UL, 0x12835b01UL, 0x243185beUL, 0x550c7dc3UL,
    0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL, 0xc19bf174UL,
    0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL,
    0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL,
    0x983e5152UL, 0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL,
    0xc6e00bf3UL, 0xd5a79147UL, 0x06ca6351UL, 0x14292967UL,
    0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL, 0x53380d13UL,
    0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
    0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL,
    0xd192e819UL, 0xd6990624UL, 0xf40e3585UL, 0x106aa070UL,
    0x19a4c116UL, 0x1e376c08UL, 0x2748774cUL, 0x34b0bcb5UL,
    0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL, 0x682e6ff3UL,
    0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
    0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
};

/* Initial hash value H for SHA-256: */
static const sha2_word32 sha256_initial_hash_value[8] = {
    0x6a09e667UL,
    0xbb67ae85UL,
    0x3c6ef372UL,
    0xa54ff53aUL,
    0x510e527fUL,
    0x9b05688cUL,
    0x1f83d9abUL,
    0x5be0cd19UL
};

/* Hash constant words K for SHA-384 and SHA-512: */
static const sha2_word64 K512[80] = {
    0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL,
    0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL,
    0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL,
    0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL,
    0xd807aa98a3030242ULL, 0x12835b0145706fbeULL,
    0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
    0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL,
    0x9bdc06a725c71235ULL, 0xc19bf174cf692694ULL,
    0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL,
    0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
    0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL,
    0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
    0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL,
    0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL,
    0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
    0x06ca6351e003826fULL, 0x142929670a0e6e70ULL,
    0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL,
    0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
    0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL,
    0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
    0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL,
    0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL,
    0xd192e819d6ef5218ULL, 0xd69906245565a910ULL,
    0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
    0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL,
    0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL,
    0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL,
    0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL,
    0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL,
    0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
    0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL,
    0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL,
    0xca273eceea26619cULL, 0xd186b8c721c0c207ULL,
    0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL,
    0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL,
    0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
    0x28db77f523047d84ULL, 0x32caab7b40c72493ULL,
    0x3c9ebe0a15c9bebcULL, 0x431d67c49c100d4cULL,
    0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL,
    0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL
};

/* Initial hash value H for SHA-384 */
static const sha2_word64 sha384_initial_hash_value[8] = {
    0xcbbb9d5dc1059ed8ULL,
    0x629a292a367cd507ULL,
    0x9159015a3070dd17ULL,
    0x152fecd8f70e5939ULL,
    0x67332667ffc00b31ULL,
    0x8eb44a8768581511ULL,
    0xdb0c2e0d64f98fa7ULL,
    0x47b5481dbefa4fa4ULL
};

/* Initial hash value H for SHA-512 */
static const sha2_word64 sha512_initial_hash_value[8] = {
    0x6a09e667f3bcc908ULL,
    0xbb67ae8584caa73bULL,
    0x3c6ef372fe94f82bULL,
    0xa54ff53a5f1d36f1ULL,
    0x510e527fade682d1ULL,
    0x9b05688c2b3e6c1fULL,
    0x1f83d9abfb41bd6bULL,
    0x5be0cd19137e2179ULL
};
static inline int os_is_big_endian(void)
{
    uint32_t data = 0xFF000000;

    if (0xFF == *(uint8_t *) & data) {
        return 1;                                   //big endian
    }

    return 0;                                       //little endian
}

//reverse byte order
static inline uint32_t reverse_32bit(uint32_t data)
{
    data = (data >> 16) | (data << 16);
    return ((data & 0xff00ff00UL) >> 8) | ((data & 0x00ff00ffUL) << 8);
}

uint32_t os_htole32(uint32_t data)
{
    if (os_is_big_endian()) {
        return reverse_32bit(data);
    }

    return data;
}

static inline uint64_t reverse_64bit(uint64_t data)
{
    data = (data >> 32) | (data << 32);
    data = ((data & 0xff00ff00ff00ff00ULL) >> 8) | ((data & 0x00ff00ff00ff00ffULL)
                                                    << 8);

    return ((data & 0xffff0000ffff0000ULL) >> 16) | ((data & 0x0000ffff0000ffffULL)
                                                     << 16);
}

//big endian to host byte order
uint32_t os_be32toh(uint32_t data)
{
    return os_htobe32(data);
}

//host byte order to big endian
uint32_t os_htobe32(uint32_t data)
{
    if (os_is_big_endian()) {
        return data;
    }

    return reverse_32bit(data);
}

//host to big endian
uint64_t os_htobe64(uint64_t data)
{
    if (os_is_big_endian()) {
        return data;
    }

    return reverse_64bit(data);
}

//big endian to host
uint64_t os_be64toh(uint64_t data)
{
    return os_htobe64(data);
}

/*** SHA-256: *********************************************************/
static void SHA256_Init(SHA256_CTX *context)
{
    if (context == (SHA256_CTX *) 0) {
        return;
    }
    MEMCPY_BCOPY(context->state, sha256_initial_hash_value, SHA256_DIGEST_LENGTH);
    MEMSET_BZERO(context->buffer, SHA256_BLOCK_LENGTH);
    context->bitcount = 0;
}


static void SHA256_Transform(SHA256_CTX *context, const sha2_word32 *data)
{
    sha2_word32 a, b, c, d, e, f, g, h, s0, s1;
    sha2_word32 T1, T2, *W256;
    int j;

    W256 = (sha2_word32 *) context->buffer;

    /* Initialize registers with the prev. intermediate value */
    a = context->state[0];
    b = context->state[1];
    c = context->state[2];
    d = context->state[3];
    e = context->state[4];
    f = context->state[5];
    g = context->state[6];
    h = context->state[7];

    j = 0;
    do {
        /* Copy data while converting to host byte order */
        W256[j] = os_htobe32(*data++);

        /* Apply the SHA-256 compression function to update a..h */
        T1 = h + Sigma1_256(e) + Ch(e, f, g) + K256[j] + W256[j];

        T2 = Sigma0_256(a) + Maj(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + T1;
        d = c;
        c = b;
        b = a;
        a = T1 + T2;

        j++;
    } while (j < 16);

    do {
        /* Part of the message block expansion: */
        s0 = W256[(j + 1) & 0x0f];
        s0 = sigma0_256(s0);
        s1 = W256[(j + 14) & 0x0f];
        s1 = sigma1_256(s1);

        /* Apply the SHA-256 compression function to update a..h */
        T1 = h + Sigma1_256(e) + Ch(e, f,
                                    g) + K256[j] + (W256[j & 0x0f] += s1 + W256[(j + 9) & 0x0f] + s0);
        T2 = Sigma0_256(a) + Maj(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + T1;
        d = c;
        c = b;
        b = a;
        a = T1 + T2;

        j++;
    } while (j < 64);

    /* Compute the current intermediate hash value */
    context->state[0] += a;
    context->state[1] += b;
    context->state[2] += c;
    context->state[3] += d;
    context->state[4] += e;
    context->state[5] += f;
    context->state[6] += g;
    context->state[7] += h;

    /* Clean up */
    a = b = c = d = e = f = g = h = T1 = T2 = 0;
}


static void SHA256_Update(SHA256_CTX *context, const sha2_byte *data,
                          uint32_t len)
{
    unsigned int os_freespace, usedspace;

    if (len == 0) {
        /* Calling with no data is valid - we do nothing */
        return;
    }

    /* Sanity check: */
    if (context == NULL || data == NULL) {
        return;
    }
    usedspace = (context->bitcount >> 3) % SHA256_BLOCK_LENGTH;
    if (usedspace > 0) {
        /* Calculate how much os_free space is available in the buffer */
        os_freespace = SHA256_BLOCK_LENGTH - usedspace;

        if (len >= os_freespace) {
            /* Fill the buffer completely and process it */
            MEMCPY_BCOPY(&context->buffer[usedspace], data, os_freespace);
            context->bitcount += os_freespace << 3;
            len -= os_freespace;
            data += os_freespace;
            SHA256_Transform(context, (sha2_word32 *) context->buffer);
        } else {
            /* The buffer is not yet full */
            MEMCPY_BCOPY(&context->buffer[usedspace], data, len);
            context->bitcount += len << 3;
            /* Clean up: */
            usedspace = os_freespace = 0;
            return;
        }
    }
    while (len >= SHA256_BLOCK_LENGTH) {
        /* Process as many complete blocks as we can */
        SHA256_Transform(context, (sha2_word32 *) data);
        context->bitcount += SHA256_BLOCK_LENGTH << 3;
        len -= SHA256_BLOCK_LENGTH;
        data += SHA256_BLOCK_LENGTH;
    }
    if (len > 0) {
        /* There's left-overs, so save 'em */
        MEMCPY_BCOPY(context->buffer, data, len);
        context->bitcount += len << 3;
    }
    /* Clean up: */
    usedspace = os_freespace = 0;
}

static void SHA256_Final(sha2_byte digest[], SHA256_CTX *context)
{
    sha2_word32 *d = (sha2_word32 *) digest;
    unsigned int usedspace;

    /* Sanity check: */
    if (context == NULL) {
        return;
    }

    /* If no digest buffer is passed, we don't bother doing this: */
    if (digest != (sha2_byte *) 0) {
        usedspace = (context->bitcount >> 3) % SHA256_BLOCK_LENGTH;
        context->bitcount = os_htobe64(context->bitcount);
        if (usedspace > 0) {
            /* Begin padding with a 1 bit: */
            context->buffer[usedspace++] = 0x80;

            if (usedspace <= SHA256_SHORT_BLOCK_LENGTH) {
                /* Set-up for the last transform: */
                MEMSET_BZERO(&context->buffer[usedspace],
                             SHA256_SHORT_BLOCK_LENGTH - usedspace);
            } else {
                if (usedspace < SHA256_BLOCK_LENGTH) {
                    MEMSET_BZERO(&context->buffer[usedspace], SHA256_BLOCK_LENGTH - usedspace);
                }
                /* Do second-to-last transform: */
                SHA256_Transform(context, (sha2_word32 *) context->buffer);

                /* And set-up for the last transform: */
                MEMSET_BZERO(context->buffer, SHA256_SHORT_BLOCK_LENGTH);
            }
        } else {
            /* Set-up for the last transform: */
            MEMSET_BZERO(context->buffer, SHA256_SHORT_BLOCK_LENGTH);

            /* Begin padding with a 1 bit: */
            *context->buffer = 0x80;
        }
        /* Set the bit count: */
        *(sha2_word64 *) & context->buffer[SHA256_SHORT_BLOCK_LENGTH] =
            context->bitcount;

        /* Final transform: */
        SHA256_Transform(context, (sha2_word32 *) context->buffer);

        {
            /* Convert TO host byte order */
            int j;
            for (j = 0; j < 8; j++) {
                context->state[j] = os_be32toh(context->state[j]);
                *d++ = context->state[j];
            }
        }
    }

    /* Clean up state data: */
    MEMSET_BZERO(context, sizeof(*context));
    usedspace = 0;
}


/*** SHA-512: *********************************************************/
static void SHA512_Init(SHA512_CTX *context)
{
    if (context == (SHA512_CTX *) 0) {
        return;
    }
    MEMCPY_BCOPY(context->state, sha512_initial_hash_value, SHA512_DIGEST_LENGTH);
    MEMSET_BZERO(context->buffer, SHA512_BLOCK_LENGTH);
    context->bitcount[0] = context->bitcount[1] = 0;
}

static void SHA512_Transform(SHA512_CTX *context, const sha2_word64 *data)
{
    sha2_word64 a, b, c, d, e, f, g, h, s0, s1;
    sha2_word64 T1, T2, *W512 = (sha2_word64 *) context->buffer;
    int j;

    /* Initialize registers with the prev. intermediate value */
    a = context->state[0];
    b = context->state[1];
    c = context->state[2];
    d = context->state[3];
    e = context->state[4];
    f = context->state[5];
    g = context->state[6];
    h = context->state[7];

    j = 0;
    do {
        /* Convert TO host byte order */
        W512[j] = os_be64toh(*data++);
        T1 = h + Sigma1_512(e) + Ch(e, f, g) + K512[j] + W512[j];

        T2 = Sigma0_512(a) + Maj(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + T1;
        d = c;
        c = b;
        b = a;
        a = T1 + T2;

        j++;
    } while (j < 16);

    do {
        /* Part of the message block expansion: */
        s0 = W512[(j + 1) & 0x0f];
        s0 = sigma0_512(s0);
        s1 = W512[(j + 14) & 0x0f];
        s1 = sigma1_512(s1);

        /* Apply the SHA-512 compression function to update a..h */
        T1 = h + Sigma1_512(e) + Ch(e, f,
                                    g) + K512[j] + (W512[j & 0x0f] += s1 + W512[(j + 9) & 0x0f] + s0);
        T2 = Sigma0_512(a) + Maj(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + T1;
        d = c;
        c = b;
        b = a;
        a = T1 + T2;

        j++;
    } while (j < 80);

    /* Compute the current intermediate hash value */
    context->state[0] += a;
    context->state[1] += b;
    context->state[2] += c;
    context->state[3] += d;
    context->state[4] += e;
    context->state[5] += f;
    context->state[6] += g;
    context->state[7] += h;

    /* Clean up */
    a = b = c = d = e = f = g = h = T1 = T2 = 0;
}

static void SHA512_Update(SHA512_CTX *context, const sha2_byte *data,
                          uint32_t len)
{
    unsigned int os_freespace, usedspace;

    if (len == 0) {
        /* Calling with no data is valid - we do nothing */
        return;
    }

    /* Sanity check: */
    if (context == NULL || data == NULL) {
        return;
    }

    usedspace = (context->bitcount[0] >> 3) % SHA512_BLOCK_LENGTH;
    if (usedspace > 0) {
        /* Calculate how much os_free space is available in the buffer */
        os_freespace = SHA512_BLOCK_LENGTH - usedspace;

        if (len >= os_freespace) {
            /* Fill the buffer completely and process it */
            MEMCPY_BCOPY(&context->buffer[usedspace], data, os_freespace);
            ADDINC128(context->bitcount, os_freespace << 3);
            len -= os_freespace;
            data += os_freespace;
            SHA512_Transform(context, (sha2_word64 *) context->buffer);
        } else {
            /* The buffer is not yet full */
            MEMCPY_BCOPY(&context->buffer[usedspace], data, len);
            ADDINC128(context->bitcount, len << 3);
            /* Clean up: */
            usedspace = os_freespace = 0;
            return;
        }
    }
    while (len >= SHA512_BLOCK_LENGTH) {
        /* Process as many complete blocks as we can */
        SHA512_Transform(context, (sha2_word64 *) data);
        ADDINC128(context->bitcount, SHA512_BLOCK_LENGTH << 3);
        len -= SHA512_BLOCK_LENGTH;
        data += SHA512_BLOCK_LENGTH;
    }
    if (len > 0) {
        /* There's left-overs, so save 'em */
        MEMCPY_BCOPY(context->buffer, data, len);
        ADDINC128(context->bitcount, len << 3);
    }
    /* Clean up: */
    usedspace = os_freespace = 0;
}

static void SHA512_Last(SHA512_CTX *context)
{
    unsigned int usedspace;

    usedspace = (context->bitcount[0] >> 3) % SHA512_BLOCK_LENGTH;

    context->bitcount[0] = os_htobe64(context->bitcount[0]);
    context->bitcount[1] = os_htobe64(context->bitcount[1]);

    if (usedspace > 0) {
        /* Begin padding with a 1 bit: */
        context->buffer[usedspace++] = 0x80;

        if (usedspace <= SHA512_SHORT_BLOCK_LENGTH) {
            /* Set-up for the last transform: */
            MEMSET_BZERO(&context->buffer[usedspace],
                         SHA512_SHORT_BLOCK_LENGTH - usedspace);
        } else {
            if (usedspace < SHA512_BLOCK_LENGTH) {
                MEMSET_BZERO(&context->buffer[usedspace], SHA512_BLOCK_LENGTH - usedspace);
            }
            /* Do second-to-last transform: */
            SHA512_Transform(context, (sha2_word64 *) context->buffer);

            /* And set-up for the last transform: */
            MEMSET_BZERO(context->buffer, SHA512_BLOCK_LENGTH - 2);
        }
    } else {
        /* Prepare for final transform: */
        MEMSET_BZERO(context->buffer, SHA512_SHORT_BLOCK_LENGTH);

        /* Begin padding with a 1 bit: */
        *context->buffer = 0x80;
    }
    /* Store the length of input data (in bits): */
    *(sha2_word64 *) & context->buffer[SHA512_SHORT_BLOCK_LENGTH] =
        context->bitcount[1];
    *(sha2_word64 *) & context->buffer[SHA512_SHORT_BLOCK_LENGTH + 8] =
        context->bitcount[0];

    /* Final transform: */
    SHA512_Transform(context, (sha2_word64 *) context->buffer);
}

static void SHA512_Final(sha2_byte digest[], SHA512_CTX *context)
{
    sha2_word64 *d = (sha2_word64 *) digest;

    /* Sanity check: */
    if (context == NULL) {
        return;
    }

    /* If no digest buffer is passed, we don't bother doing this: */
    if (digest != (sha2_byte *) 0) {
        SHA512_Last(context);

        /* Save the hash data for output: */
        {
            /* Convert TO host byte order */
            int j;
            for (j = 0; j < 8; j++) {
                context->state[j] = os_be64toh(context->state[j]);
                *d++ = context->state[j];
            }
        }
    }

    /* Zero out state data */
    MEMSET_BZERO(context, sizeof(*context));
}


/*** SHA-384: *********************************************************/
static void SHA384_Init(SHA384_CTX *context)
{
    if (context == (SHA384_CTX *) 0) {
        return;
    }
    MEMCPY_BCOPY(context->state, sha384_initial_hash_value, SHA512_DIGEST_LENGTH);
    MEMSET_BZERO(context->buffer, SHA384_BLOCK_LENGTH);
    context->bitcount[0] = context->bitcount[1] = 0;
}

static void SHA384_Update(SHA384_CTX *context, const sha2_byte *data,
                          uint32_t len)
{
    SHA512_Update((SHA512_CTX *) context, data, len);
}

static void SHA384_Final(sha2_byte digest[], SHA384_CTX *context)
{
    sha2_word64 *d = (sha2_word64 *) digest;

    /* Sanity check: */
    if (context == NULL) {
        return;
    }

    /* If no digest buffer is passed, we don't bother doing this: */
    if (digest != (sha2_byte *) 0) {
        SHA512_Last((SHA512_CTX *) context);

        /* Save the hash data for output: */
        {
            /* Convert TO host byte order */
            int j;
            for (j = 0; j < 6; j++) {
                context->state[j] = os_be64toh(context->state[j]);
                *d++ = context->state[j];
            }
        }
    }

    /* Zero out state data */
    MEMSET_BZERO(context, sizeof(context));
}


#if 0
/*
 * Constant used by SHA256/384/512_End() functions for converting the
 * digest to a readable hexadecimal character string:
 */
static const char *sha2_hex_digits = "0123456789abcdef";

static char *SHA256_End(SHA256_CTX *context, char buffer[])
{
    sha2_byte digest[SHA256_DIGEST_LENGTH], *d = digest;
    int i;

    /* Sanity check: */
    OS_ASSERT(context != (SHA256_CTX *) 0, NULL);

    if (buffer != (char *)0) {
        SHA256_Final(digest, context);

        for (i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            *buffer++ = sha2_hex_digits[(*d & 0xf0) >> 4];
            *buffer++ = sha2_hex_digits[*d & 0x0f];
            d++;
        }
        *buffer = (char)0;
    } else {
        MEMSET_BZERO(context, sizeof(context));
    }
    MEMSET_BZERO(digest, SHA256_DIGEST_LENGTH);
    return buffer;
}

static char *SHA512_End(SHA512_CTX *context, char buffer[])
{
    sha2_byte digest[SHA512_DIGEST_LENGTH], *d = digest;
    int i;

    /* Sanity check: */
    OS_ASSERT(context != (SHA512_CTX *) 0, NULL);

    if (buffer != (char *)0) {
        SHA512_Final(digest, context);

        for (i = 0; i < SHA512_DIGEST_LENGTH; i++) {
            *buffer++ = sha2_hex_digits[(*d & 0xf0) >> 4];
            *buffer++ = sha2_hex_digits[*d & 0x0f];
            d++;
        }
        *buffer = (char)0;
    } else {
        MEMSET_BZERO(context, sizeof(context));
    }
    MEMSET_BZERO(digest, SHA512_DIGEST_LENGTH);
    return buffer;
}

static char *SHA384_End(SHA384_CTX *context, char buffer[])
{
    sha2_byte digest[SHA384_DIGEST_LENGTH], *d = digest;
    int i;

    /* Sanity check: */
    OS_ASSERT(context != (SHA384_CTX *) 0, NULL);

    if (buffer != (char *)0) {
        SHA384_Final(digest, context);

        for (i = 0; i < SHA384_DIGEST_LENGTH; i++) {
            *buffer++ = sha2_hex_digits[(*d & 0xf0) >> 4];
            *buffer++ = sha2_hex_digits[*d & 0x0f];
            d++;
        }
        *buffer = (char)0;
    } else {
        MEMSET_BZERO(context, sizeof(context));
    }
    MEMSET_BZERO(digest, SHA384_DIGEST_LENGTH);
    return buffer;
}

#endif

