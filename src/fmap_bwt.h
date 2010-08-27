#ifndef FMAP_BWT_H
#define FMAP_BWT_H

/* The MIT License

   Copyright (c) 2008 Genome Research Ltd (GRL).

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
   */


#include <stdint.h>

// TODO document

// requirement: ((b)->occ_interval%16 == 0)

#ifndef FMAP_UBYTE
#define FMAP_UBYTE
typedef unsigned char uint8_t;
#endif

#define FMAP_BWT_OCC_INTERVAL 0x80

/*! @typedef
  @abstract            BWT data structure
  @field  primary       S^{-1}(0), or the primary index of BWT
  @field  L2            C(), cumulative count
  @field  seq_len       reference sequence length
  @field  bwt_size      size of bwt in bytes
  @field  occ_interval  occurence array interval, must be a strictly positive power of 16: (16, 32, 48, ...)
  @field  bwt           burrows-wheeler transform
  @field  cnt_table     occurrence array
  */
typedef struct {
    uint32_t primary; // S^{-1}(0), or the primary index of BWT
    uint32_t L2[5]; // C(), cumulative count
    uint32_t seq_len; // sequence length
    uint32_t bwt_size; // size of bwt, about seq_len/4
    uint32_t occ_interval;
    uint32_t *bwt; // BWT
    uint32_t cnt_table[256];
} fmap_bwt_t;

/*! @function
  @abstract
  @param  fn_fasta  the FASTA file name
  @return           pointer to the bwt structure 
  */
fmap_bwt_t *
fmap_bwt_read(const char *fn_fasta);

/*! @function
  @abstract
  @param  fn_fasta  the FASTA file name
  @param  bwt       the bwt structure to write
  */
void 
fmap_bwt_write(const char *fn_fasta, fmap_bwt_t *bwt);

/*! @function
  @abstract
  @param  bwt  pointer to the bwt structure to destroy
  */
void 
fmap_bwt_destroy(fmap_bwt_t *bwt);

/*! @function
  @abstract
  @param  bwt           pointer to the bwt structure to destroy
  @param  occ_interval  the new occurrence interval
  */
void 
fmap_bwt_update(fmap_bwt_t *bwt, uint32_t occ_interval);

/*! @function
  @abstract    generates the occurrence array
  @param  bwt  pointer to the bwt structure to update 
  */
void 
fmap_bwt_gen_cnt_table(fmap_bwt_t *bwt);

/*! @function
  @abstract    calculates the next occurrence given the previous occurence and the next base
  @param  bwt  pointer to the bwt structure 
  @param  k    previous occurence
  @param  c    base in two-bit integer format
  @return      the next occurrence given the base
  */
inline uint32_t 
bwt_occ(const fmap_bwt_t *bwt, uint32_t k, uint8_t c);

/*! @function
  @abstract    calculates the next occurrences given the previous occurence for all four bases
  @param  bwt  pointer to the bwt structure 
  @param  k    previous occurence
  @param  cnt  pointer to the next occurences for all four bases
  */
inline void 
bwt_occ4(const fmap_bwt_t *bwt, uint32_t k, uint32_t cnt[4]);

/*! @function
  @abstract    calculates the SA interval given the previous SA interval and the next base
  @param  bwt  pointer to the bwt structure 
  @param  k    previous lower occurence
  @param  l    previous upper occurence
  @param  c    base in two-bit integer format
  @param  ok   the next lower occurence
  @param  ol   the next upper occurence
  @discussion  more efficient version of bwt_occ but requires that k <= l (not checked)
  */
inline void 
bwt_2occ(const fmap_bwt_t *bwt, uint32_t k, uint32_t l, uint8_t c, uint32_t *ok, uint32_t *ol);

/*! @function
  @abstract     calculates the next SA intervals given the previous SA intervals for all four bases
  @param  bwt   pointer to the bwt structure 
  @param  k     previous lower occurence
  @param  l     previous upper occurence
  @param  cntk  next upper occurences
  @param  cntl  next lower occurences
  @discussion   more efficient version of bwt_occ4 but requires that k <= l (not checked)
  */
inline void 
bwt_2occ4(const fmap_bwt_t *bwt, uint32_t k, uint32_t l, uint32_t cntk[4], uint32_t cntl[4]);

/*! @function
  @abstract         computes the SA interval for the given sequence (if any)
  @param  bwt       pointer to the bwt structure 
  @param  len       the length of the sequence
  @param  str       the DNA sequence in 2-bit format
  @param  sa_begin  the beginning of the returned occurence of the SA interval
  @param  sa_end    the end of the returned occurrence of the SA interval
  @return           the size of the SA interval, 0 if none found
  */
int 
bwt_match_exact(const fmap_bwt_t *bwt, int len, const uint8_t *str, uint32_t *sa_begin, uint32_t *sa_end);

/*! @function
  @abstract         computes the SA interval for the given sequence (if any)
  @param  bwt       pointer to the bwt structure 
  @param  len       the length of the sequence
  @param  str       the DNA sequence in 2-bit format
  @param  k0        the beginning occurence of the SA interval
  @param  l0        the end occurrence of the SA interval
  @return           the size of the SA interval, 0 if none found
  @discussion       the search will be started at SA interval [k0,l0], with the results returned as [k0,l0]
  */
int 
bwt_match_exact_alt(const fmap_bwt_t *bwt, int len, const uint8_t *str, uint32_t *k0, uint32_t *l0);

#define fmap_bwt_bwt(b, k) ((b)->bwt[(k)/(b)->occ_interval*12 + 4 + (k)%(b)->occ_interval/16])

/* retrieve a character from the $-removed BWT string. Note that
 * fmap_bwt_t::bwt is not exactly the BWT string and therefore this macro is
 * called bwt_B0 instead of bwt_B */
#define fmap_bwt_B0(b, k) (fmap_bwt_bwt(b, k)>>((~(k)&0xf)<<1)&3)

#define fmap_bwt_occ_intv(b, k) ((b)->bwt + (k)/(b)->occ_interval*12)

// inverse Psi function
#define fmap_bwt_invPsi(bwt, k)												\
  (((k) == (bwt)->primary)? 0 :										\
   ((k) < (bwt)->primary)?											\
   (bwt)->L2[fmap_bwt_B0(bwt, k)] + bwt_occ(bwt, k, fmap_bwt_B0(bwt, k))		\
   : (bwt)->L2[fmap_bwt_B0(bwt, (k)-1)] + bwt_occ(bwt, k, fmap_bwt_B0(bwt, (k)-1)))


#endif
