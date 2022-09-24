/* MIT License
 *
 * Copyright (c) 2022 Tyge Løvset, NORCE, www.norceresearch.no
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
 */
#ifndef CCOMMON_H_INCLUDED
#define CCOMMON_H_INCLUDED

#define _CRT_SECURE_NO_WARNINGS
#include <inttypes.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#if SIZE_MAX == UINT32_MAX
   typedef int32_t isize_t;
#  define ISIZE_MAX INT32_MAX
#elif SIZE_MAX == UINT64_MAX
   typedef int64_t isize_t;
#  define ISIZE_MAX INT64_MAX
#endif

#if defined(_MSC_VER)
#  pragma warning(disable: 4116 4996) // unnamed type definition in parentheses
#  define STC_FORCE_INLINE static __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#  define STC_FORCE_INLINE static inline __attribute((always_inline))
#else
#  define STC_FORCE_INLINE static inline
#endif
#define STC_INLINE static inline

/* Macro overloading feature support based on: https://rextester.com/ONP80107 */
#define c_MACRO_OVERLOAD(name, ...) \
    c_paste(name, c_numargs(__VA_ARGS__))(__VA_ARGS__)
#define c_concat(a, b) a ## b
#define c_paste(a, b) c_concat(a, b)
#define c_expand(...) __VA_ARGS__
#define c_numargs(...) _c_APPLY_ARG_N((__VA_ARGS__, _c_RSEQ_N))

#define _c_APPLY_ARG_N(args) c_expand(_c_ARG_N args)
#define _c_RSEQ_N 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define _c_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, \
                 _14, _15, _16, N, ...) N

#define c_static_assert(cond) \
    typedef char c_paste(_static_assert_line_, __LINE__)[(cond) ? 1 : -1]
#define c_unchecked_container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))
#if __STDC_VERSION__ >= 202300L || defined STC_CHECKED_CONTAINER_OF
#  define c_container_of(ptr, type, member) \
    (((type *)((char *)(ptr) - offsetof(type, member))) + \
     ((typeof(ptr))0 != (typeof(&((type *)0)->member))0))
#else
#  define c_container_of(p,t,m) c_unchecked_container_of(p,t,m)
#endif
#ifndef __cplusplus
#  define c_alloc(T)            c_malloc(sizeof(T))
#  define c_alloc_n(T, n)       c_malloc(sizeof(T)*(n))
#  define c_make(T)             (T)
#  define c_new(T, ...)         (T*)memcpy(c_alloc(T), (T[]){__VA_ARGS__}, sizeof(T))
#else
#  include <new>
#  define c_alloc(T)            static_cast<T*>(c_malloc(sizeof(T)))
#  define c_alloc_n(T, n)       static_cast<T*>(c_malloc(sizeof(T)*(n)))
#  define c_make(T)             T
#  define c_new(T, ...)         new (c_alloc(T)) T(__VA_ARGS__)
#endif
#ifndef c_malloc
#  define c_malloc(sz)          malloc(sz)
#  define c_calloc(n, sz)       calloc(n, sz)
#  define c_realloc(p, sz)      realloc(p, sz)
#  define c_free(p)             free(p)
#endif

#define c_delete(T, ptr)        do { T *_c_p = ptr; T##_drop(_c_p); c_free(_c_p); } while (0)
#define c_swap(T, x, y)         do { T _c_t = x; x = y; y = _c_t; } while (0)
#define c_arraylen(a)           (sizeof (a)/sizeof *(a))

// x and y are i_keyraw* type, defaults to i_key*:
#define c_default_cmp(x, y)     (c_default_less(y, x) - c_default_less(x, y))
#define c_default_less(x, y)    (*(x) < *(y))
#define c_default_eq(x, y)      (*(x) == *(y))
#define c_memcmp_eq(x, y)       (memcmp(x, y, sizeof *(x)) == 0)
#define c_default_hash(x)       c_fasthash(x, sizeof *(x))

#define c_default_clone(v)      (v)
#define c_default_toraw(vp)     (*(vp))
#define c_default_drop(vp)      ((void) (vp))
#define c_derived_keyclone(v)   i_keyfrom((i_keyto((&(v)))))
#define c_derived_valclone(v)   i_valfrom((i_valto((&(v)))))

#define c_option(flag)          ((i_opt) & (flag))
#define c_is_fwd                (1<<0)
#define c_no_atomic             (1<<1)
#define c_no_clone              (1<<2)
#define c_no_cmp                (1<<3)

/* Generic algorithms */

typedef const char* crawstr;
#define crawstr_cmp(xp, yp) strcmp(*(xp), *(yp))
#define crawstr_hash(p) c_strhash(*(p))

#define c_strlen_lit(literal) (sizeof "" literal - 1U)
#define c_sv(...) c_MACRO_OVERLOAD(c_sv, __VA_ARGS__)
#define c_sv1(lit) (c_make(csview){lit, c_strlen_lit(lit)})
#define c_sv2(str, n) (c_make(csview){str, n})
#define c_PRIsv ".*s"
#define c_ARGsv(sv) (int)(sv).size, (sv).str

#define _c_ROTL(x, k) (x << (k) | x >> (8*sizeof(x) - (k)))

STC_INLINE uint64_t c_fasthash(const void* key, size_t len) {
    const uint8_t *x = (const uint8_t*) key;
    uint64_t u8, h = 1; size_t n = len >> 3;
    uint32_t u4;
    while (n--) {
        memcpy(&u8, x, 8), x += 8;
        h += (_c_ROTL(u8, 26) ^ u8)*0xc6a4a7935bd1e99d;
    }
    switch (len &= 7) {
        case 0: return h;
        case 4: memcpy(&u4, x, 4);
                return h + u4*0xc6a4a7935bd1e99d;
    }
    h += *x++;
    while (--len) h = (h << 10) - h + *x++;
    return _c_ROTL(h, 26) ^ h;
}

STC_INLINE uint64_t c_strhash(const char *str)
    { return c_fasthash(str, strlen(str)); }

STC_INLINE char* c_strnstrn(const char *s, const char *needle,
                            size_t slen, const size_t nlen) {
    if (!nlen) return (char *)s;
    if (nlen > slen) return NULL;
    slen -= nlen;
    do {
        if (*s == *needle && !memcmp(s, needle, nlen))
            return (char *)s;
        ++s;
    } while (slen--);
    return NULL;
}

#define c_foreach(...) c_MACRO_OVERLOAD(c_foreach, __VA_ARGS__)
#define c_foreach3(it, C, cnt) \
    for (C##_iter it = C##_begin(&cnt); it.ref; C##_next(&it))
#define c_foreach4(it, C, start, finish) \
    for (C##_iter it = start, *_endref = (C##_iter*)(finish).ref \
         ; it.ref != (C##_value*)_endref; C##_next(&it))

#define c_FLT_STACK 5

#define c_flt_drop(i, n) (++(i).dropped[(i).dn++] > (n))
#define c_flt_dropwhile(i, pred) ((i).dropwhile |= !(pred))
#define c_flt_take(i, n) (++(i).taken[(i).tn++] <= (n))
#define c_flt_taketotal(i, n) ((i).total < (n))

#define c_forfilter(...) c_MACRO_OVERLOAD(c_forfilter, __VA_ARGS__)
#define c_forfilter4(it, C, cnt, filter) \
    c_forfilter5(it, C, cnt, filter, true)
#define c_forfilter5(it, C, cnt, filter, cond) \
    c_forfilter_s(it, C, C##_begin(&cnt), filter, cond)
#define c_forfilter_s(it, C, start, filter, cond) \
    c_forwhile_s(it, C, start, cond) if (!((filter) && ++it.total)) ; else

#define c_forwhile(i, C, cnt, cond) \
    c_forwhile_s(i, C, C##_begin(&cnt), cond)
#define c_forwhile_s(i, C, start, cond) \
    for (struct {C##_iter it; const C##_value *ref; \
                 uint32_t index, total, taken[c_FLT_STACK], dropped[c_FLT_STACK]; \
                 int8_t dn, tn; bool dropwhile;} \
         i = {.it=start, .ref=i.it.ref}; i.ref && (cond) \
         ; C##_next(&i.it), i.ref = i.it.ref, ++i.index, i.dn=0, i.tn=0)

#define c_forpair(key, val, C, cnt) /* structured binding */ \
    for (struct {C##_iter _it; const C##_key* key; C##_mapped* val;} _ = {C##_begin(&cnt)} \
         ; _._it.ref && (_.key = &_._it.ref->first, _.val = &_._it.ref->second) \
         ; C##_next(&_._it))

#define c_forrange(...) c_MACRO_OVERLOAD(c_forrange, __VA_ARGS__)
#define c_forrange1(stop) c_forrange4(_c_i, size_t, 0, stop)
#define c_forrange2(i, stop) c_forrange4(i, size_t, 0, stop)
#define c_forrange3(i, itype, stop) c_forrange4(i, itype, 0, stop)
#define c_forrange4(i, itype, start, stop) \
    for (itype i=start, _end=stop; i < _end; ++i)
#define c_forrange5(i, itype, start, stop, step) \
    for (itype i=start, _inc=step, _end=(stop) - (0 < _inc) \
         ; (i <= _end) == (0 < _inc); i += _inc)

#define c_forloop(...) c_MACRO_OVERLOAD(c_forloop, __VA_ARGS__)
#define c_forloop1(stop) c_forloop4(_c_i, size_t, 0, stop)
#define c_forloop2(i, stop) c_forloop4(i, size_t, 0, stop)
#define c_forloop3(i, itype, stop) c_forloop4(i, itype, 0, stop)
#define c_forloop4(i, itype, start, stop) \
    for (struct {itype val, _end, *ref;} \
         i = {.val=start, ._end=stop, .ref=&i.val} \
         ; i.val < i._end; ++i.val)
#define c_forloop5(i, itype, start, stop, step) \
    for (struct {itype val, _inc, _end, *ref;} \
         i = {.val=start, ._inc=step, ._end=(stop) - (0 < i._inc), .ref=&i.val} \
         ; (i.val > i._end) ^ (i._inc > 0); i.val += i._inc)

#define c_forlist(it, T, ...) \
    for (T _i_data[]=__VA_ARGS__, *_i_data_end=_i_data+c_arraylen(_i_data); _i_data_end; _i_data_end=NULL) \
    for (struct {T* ref; size_t index;} it = {.ref=_i_data}; it.ref < _i_data_end; ++it.ref, ++it.index)

// [deprecated] - replaced by c_forlist():
#define c_forarray(T, v, ...) \
    for (T _a[] = __VA_ARGS__, *v = _a; v != _a + c_arraylen(_a); ++v)
#define c_forarray_p(T, v, ...) \
    for (T _a[] = __VA_ARGS__, **v = _a; v != _a + c_arraylen(_a); ++v)
#define c_autovar c_with    // [deprecated]
#define c_autoscope c_scope // [deprecated]
#define c_autodefer c_defer // [deprecated]

#define c_with(...) c_MACRO_OVERLOAD(c_with, __VA_ARGS__)
#define c_with2(declvar, drop) for (declvar, **_c_i = NULL; !_c_i; ++_c_i, drop)
#define c_with3(declvar, pred, drop) for (declvar, **_c_i = NULL; !_c_i && (pred); ++_c_i, drop)
#define c_scope(init, drop) for (int _c_i = (init, 0); !_c_i; ++_c_i, drop)
#define c_defer(...) for (int _c_i = 0; !_c_i; ++_c_i, __VA_ARGS__)
#define c_breakauto continue

#define c_auto(...) c_MACRO_OVERLOAD(c_auto, __VA_ARGS__)
#define c_auto2(C, a) \
    c_with2(C a = C##_init(), C##_drop(&a))
#define c_auto3(C, a, b) \
    c_with2(c_expand(C a = C##_init(), b = C##_init()), \
               (C##_drop(&b), C##_drop(&a)))
#define c_auto4(C, a, b, c) \
    c_with2(c_expand(C a = C##_init(), b = C##_init(), c = C##_init()), \
               (C##_drop(&c), C##_drop(&b), C##_drop(&a)))
#define c_auto5(C, a, b, c, d) \
    c_with2(c_expand(C a = C##_init(), b = C##_init(), c = C##_init(), d = C##_init()), \
               (C##_drop(&d), C##_drop(&c), C##_drop(&b), C##_drop(&a)))

#define c_autobuf(b, type, n) c_autobuf_N(b, type, n, 256)
#define c_autobuf_N(b, type, n, BYTES) \
    for (type _c_b[((BYTES) - 1) / sizeof(type) + 1], \
         *b = (n)*sizeof *b > (BYTES) ? c_alloc_n(type, n) : _c_b \
         ; b; b != _c_b ? c_free(b) : (void)0, b = NULL)

#define c_pair(v) (v)->first, (v)->second
#define c_drop(C, ...) do { c_forlist (_i, C*, {__VA_ARGS__}) C##_drop(*_i.ref); } while(0)

#define c_find_if(it, C, cnt, pred) do { \
    size_t index = 0; \
    for (it = C##_begin(&cnt); it.ref && !(pred); C##_next(&it)) \
        ++index; \
} while (0)

#define c_find_in(it, C, start, end, pred) do { \
    size_t index = 0; \
    const C##_value* _endref = (end).ref; \
    for (it = start; it.ref != _endref && !(pred); C##_next(&it)) \
        ++index; \
    if (it.ref == _endref) it.ref = NULL; \
} while (0)

#define c_erase_if(it, C, cnt, pred) do { \
    C##_iter it = C##_begin(&cnt); \
    while (it.ref) \
        if (pred) it = C##_erase_at(&cnt, it); \
        else C##_next(&it); \
} while (0)

#endif // CCOMMON_H_INCLUDED

#undef STC_API
#undef STC_DEF

#if !defined(i_static) && !defined(STC_STATIC) && (defined(i_header) || defined(STC_HEADER) || \
                                                   defined(i_implement) || defined(STC_IMPLEMENT))
#  define STC_API extern
#  define STC_DEF
#else
#  define i_static
#  define STC_API static inline
#  define STC_DEF static inline
#endif
#if defined(STC_EXTERN)
#  define i_extern
#endif
#if defined(i_static) || defined(STC_IMPLEMENT)
#  define i_implement
#endif
