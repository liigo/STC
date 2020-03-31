// MIT License
//
// Copyright (c) 2020 Tyge Løvset, NORCE, www.norceresearch.no
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef CDEFS__H__
#define CDEFS__H__

#include <stdint.h>
#include <stdbool.h>

// Macro overloading feature support: https://rextester.com/ONP80107
#define c_CAT( A, B ) A ## B
#define c_EXPAND(...) __VA_ARGS__
#define c_VA_ARG_SIZE(...) c_EXPAND(c_APPLY_ARG_N((__VA_ARGS__, c_RSEQ_N)))
#define c_APPLY_ARG_N(ARGS) c_EXPAND(c_ARG_N ARGS)
#define c_ARG_N(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, N,...) N
#define c_RSEQ_N 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define c_OVERLOAD_SELECT(NAME, NUM) c_CAT( NAME ## _, NUM)

#define c_MACRO_OVERLOAD(NAME, ...) c_OVERLOAD_SELECT(NAME, c_VA_ARG_SIZE(__VA_ARGS__))(__VA_ARGS__)


#define c_new(...)      c_MACRO_OVERLOAD(c_new, __VA_ARGS__)
#define c_new_1(T)      ((T *) malloc(sizeof(T)))
#define c_new_2(T, n)   ((T *) malloc(sizeof(T) * (n)))

#define c_struct(S)     typedef struct S S; struct S
#define                 c_npos         ((size_t) -1)
#define                 c_max_alloca   (1000)
#define                 c_swap(T, x, y) { T __t = x; x = y; y = __t; }

#define                 c_defaultInitRaw(x) (x)
#define                 c_defaultGetRaw(x) (x)
#define                 c_defaultCompare(x, y)  (*(x) == *(y) ? 0 : *(x) < *(y) ? -1 : 1)
#define                 c_defaultEquals(x, y)  (memcmp(x, y, sizeof(*(y))) == 0)
static inline void      c_defaultDestroy(void* value) {}

#define                 c_foreach(it, ctag, con) \
                            for (ctag##_iter_t it = ctag##_begin(con); it.item != ctag##_end(con).item; it = ctag##_next(it))
                                
// One-byte-at-a-time hash based on Murmur's mix
static inline uint32_t  c_defaultHash(const void *data, size_t len) {
    const uint8_t *key = (const uint8_t *) data;
    uint32_t h = 0xc613fc15;
    while (len--) {
        h ^= *key++;
        h *= 0x5bd1e995;
        h ^= h >> 15;
    }
    return h;
}


#endif