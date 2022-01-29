#ifndef _pyth_oracle_util_rexp2_h_
#define _pyth_oracle_util_rexp2_h_

/* rexp2_fxp computes:

     y/2^30 ~ exp2( -x/2^30 )

   That is, it is a fixed point approximation of 1 / 2^x = 2^-x where x
   is non-negative and represented with 34 integer bits and 30
   fractional bits (34u.30 fxp).  The output will be in [0,2^30] (i.e.
   1u.30 fxp).

   The approximation is designed to minimize the RMS error over the
   interval [0,2^30) while preserving continuity overall and matching at
   the interval end points exactly (as such that 2^-i for non-negative
   integer i will be exact for i in [0,30] and correctly round-near-even
   for larger i).

   Define REXP2_FXP_ORDER from 1:7 to get the desired cost / accuracy
   tradeoff.  At REXP2_FXP_ORDER 1, this costs virtually nothing (some
   fast bit ops and 64-bit integer subtract); it is a piecewise linear
   approximation that is typically accurate to a few percent.

   At REXP2_FXP_ORDER 7, this is nearly IEEE style 30 bit precision
   (i.e. for x in [0,2^30), the result with 1 ulp instead of the
   theoretical limit of 0.5 ulp).  For x in [0,2^30), the rounding is
   approximately round nearest even and more round toward zero for x >>
   2^30.  It requires some fast bits ops, 7 64*64->64 subtracts and 7
   64*64->64 multiplies (similarly for the orders 2:6).

   REXP2_FXP_ORDER 5 is the default and has a worst case output accuracy
   comparable IEEE single precision.

   In short, for integer x/2^30, it is the exact value correctly rounded
   and, for IID random x in [0,2^30), the approximation errors can be
   thought of as a random Gaussian perturbation on the output value such
   that the approximation for a given order is the one that minimizes
   the perturbations variance. */

#include "compat_stdint.h"

#ifndef REXP2_FXP_ORDER
#define REXP2_FXP_ORDER 5
#endif

#ifdef __cplusplus
extern "C" {
#endif

static inline uint64_t
rexp2_fxp( uint64_t x ) {

  /* This calculation is based on splitting x into integer and
     fractional bits such that:
       x = 2^30 i + d
     where d is in [0,2^30).  Then we have:
       y ~ 2^30 exp2( -i ) exp2( -d/2^30 )
     Or:
       y ~ round( 2^30 exp2( -d/2^30 ) ) >> i
     Thus, the main challenge is computing an approximation of:
       exp2( -delta )
     for delta = d/2^30 in [0,1) with good enough precision.  As this
     smoothly goes from 1 to 1/2, this is an ideal candidate for a
     polynomial approximation.  We aren't too particular or maxing out
     precision or precise rounding in the interest of compactness and
     speed.

     The polynomial coefficients below were selected as the polynomials
     that minimize the RMS error over the interval [0,1) while matching
     exactly at 0 and 1 (RMS error was used over minimax as it has more
     intuitive error properties for users typically).  All but the
     leading coefficent were scaled to be in the range [2^33,2^34) to
     minimize the coefficient quantization error while yielding a value
     that cannot not overflow when multiplied by d.  The intermediate
     needed for the Horner's rule application were then computed
     accordingly.  The leading coefficient scale was fixed at 2^30 to go
     straight to the output range. */

  uint64_t i = x >> 30;
  uint64_t d = x & ((UINT64_C(1)<<30)-UINT64_C(1));
  uint64_t y;

  /* BEGIN AUTOGENERATED CODE - See KJB for code generator */
# if REXP2_FXP_ORDER==1
  /* bits 4.0 rms_aerr 1.3e-02 rms_rerr 1.8e-02 max_aerr 4.3e-02 max_rerr 6.1e-02 */
  /* As implemented: bits 4.0 max_aerr 4.3e-02 max_rerr 6.1e-02 ulp 46209195.0 */
  y = UINT64_C(0x040000000) - (d>>1);
# elif REXP2_FXP_ORDER==2
  /* bits 8.3 rms_aerr 1.4e-03 rms_rerr 2.0e-03 max_aerr 1.9e-03 max_rerr 3.2e-03 */
  /* As implemented: bits 8.3 max_aerr 1.9e-03 max_rerr 3.2e-03 ulp 2086873.1 */
  y = UINT64_C(0x2c029d07d);
  y = UINT64_C(0x2b00a741f) - ((y*d)>>32);
  y = UINT64_C(0x040000000) - ((y*d)>>34);
# elif REXP2_FXP_ORDER==3
  /* bits 13.0 rms_aerr 5.5e-05 rms_rerr 7.9e-05 max_aerr 8.6e-05 max_rerr 1.2e-04 */
  /* As implemented: bits 13.0 max_aerr 8.6e-05 max_rerr 1.2e-04 ulp 91856.4 */
  y = UINT64_C(0x288319c3e);
  y = UINT64_C(0x3b33c6b15) - ((y*d)>>32);
  y = UINT64_C(0x2c44c0101) - ((y*d)>>32);
  y = UINT64_C(0x040000000) - ((y*d)>>34);
# elif REXP2_FXP_ORDER==4
  /* bits 17.6 rms_aerr 1.6e-06 rms_rerr 2.4e-06 max_aerr 2.8e-06 max_rerr 5.0e-06 */
  /* As implemented: bits 17.6 max_aerr 2.8e-06 max_rerr 5.0e-06 ulp 3000.1 */
  y = UINT64_C(0x38100ce16);
  y = UINT64_C(0x36871cfc4) - ((y*d)>>33);
  y = UINT64_C(0x3d4dfa602) - ((y*d)>>32);
  y = UINT64_C(0x2c5b2ce21) - ((y*d)>>32);
  y = UINT64_C(0x040000000) - ((y*d)>>34);
# elif REXP2_FXP_ORDER==5
  /* bits 22.8 rms_aerr 4.6e-08 rms_rerr 6.7e-08 max_aerr 7.7e-08 max_rerr 1.4e-07 */
  /* As implemented: bits 22.7 max_aerr 7.8e-08 max_rerr 1.4e-07 ulp 83.6 */
  y = UINT64_C(0x3e1a2f97e);
  y = UINT64_C(0x25bc1de09) - ((y*d)>>34);
  y = UINT64_C(0x38a155436) - ((y*d)>>32);
  y = UINT64_C(0x3d7c8e03d) - ((y*d)>>32);
  y = UINT64_C(0x2c5c78186) - ((y*d)>>32);
  y = UINT64_C(0x040000000) - ((y*d)>>34);
# elif REXP2_FXP_ORDER==6
  /* bits 27.9 rms_aerr 1.1e-09 rms_rerr 1.6e-09 max_aerr 2.1e-09 max_rerr 3.9e-09 */
  /* As implemented: bits 27.9 max_aerr 3.0e-09 max_rerr 4.0e-09 ulp 3.2 */
  y = UINT64_C(0x3959e0dfb);
  y = UINT64_C(0x29cdf1eff) - ((y*d)>>34);
  y = UINT64_C(0x273d8f899) - ((y*d)>>33);
  y = UINT64_C(0x38d2ad669) - ((y*d)>>32);
  y = UINT64_C(0x3d7f590ad) - ((y*d)>>32);
  y = UINT64_C(0x2c5c85808) - ((y*d)>>32);
  y = UINT64_C(0x040000000) - ((y*d)>>34);
# elif REXP2_FXP_ORDER==7
  /* bits 33.5 rms_aerr 2.3e-11 rms_rerr 3.4e-11 max_aerr 4.4e-11 max_rerr 8.5e-11 */
  /* As implemented: bits 29.0 max_aerr 9.7e-10 max_rerr 1.9e-09 ulp 1.0 */
  y = UINT64_C(0x2d6cd448b);
  y = UINT64_C(0x269cc5254) - ((y*d)>>34);
  y = UINT64_C(0x2b82bc124) - ((y*d)>>33);
  y = UINT64_C(0x2762b03ae) - ((y*d)>>33);
  y = UINT64_C(0x38d5e75bc) - ((y*d)>>32);
  y = UINT64_C(0x3d7f7ab76) - ((y*d)>>32);
  y = UINT64_C(0x2c5c85fa8) - ((y*d)>>32);
  y = UINT64_C(0x040000000) - ((y*d)>>34);
# else
# error "Unsupported REXP2_FXP_ORDER"
# endif
  /* END AUTOGENERATED CODE */

  return (i>UINT64_C(63)) ? 0 : (y >> i);
}

#ifdef __cplusplus
}
#endif

#endif /* _pyth_oracle_util_rexp2_h_ */
