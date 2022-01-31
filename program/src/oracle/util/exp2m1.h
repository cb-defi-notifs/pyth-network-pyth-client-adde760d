#ifndef _pyth_oracle_util_exp2m1_h_
#define _pyth_oracle_util_exp2m1_h_

/* exp2m1_fxp computes:

     y/2^30 ~ exp2m1( x/2^30 ) = 2^( x/2^30 ) - 1

   That is, it computes a fixed point approximation of 2^x-1 where x is
   non-negative and represented with 34 integer bits and 30 fractional
   bits (34u.30 fxp).  If x/2^30 >= 34 (i.e. x>exp2m1_fxp_max()),
   returns UINT64_MAX.  For x<=exp2m1_fxp_max(), a exp2_fxp can also
   trivially constructed by just adding 2^30 to the result.

   The approximation is designed to minimize the RMS error over the
   interval [0,2^30) while preserving continuity overall and matching at
   at key places exactly (e.g. if x/2^30 = 2^i for i in [-30,33], this
   will be exact).

   Define EXP2M1_FXP_ORDER from 1:7 to get the desired cost / accuracy
   tradeoff.  At EXP2M1_FXP_ORDER 1, this costs virtually nothing (some
   fast bit ops); it is a piecewise linear approximation that is
   typically accurate to a few percent.

   At EXP2M1_FXP_ORDER 7, this is nearly IEEE style 30 bit precision
   (i.e. for x in [0,2^30), the result with 0.6 ulp instead of the
   theoretical limit of 0.5 ulp).  For x in [0,2^30), the rounding is
   approximately round nearest even.  For larger x, relative error is
   approximately constant.  It requires some fast bits ops, 7 64*64->64
   adds and 7 64*64->64 multiplies (similarly for the orders 2:6).

   EXP2M1_FXP_ORDER 5 is the default and has a worst case output
   accuracy comparable IEEE single precision.

   In short, for integer x/2^30, it is the exact value correctly rounded
   and, for IID random x in [0,2^30), the approximation errors can be
   thought of as a random Gaussian perturbation on the output value such
   that the approximation for a given order is the one that minimizes
   the perturbation variance. */

#include "compat_stdint.h"

#ifndef EXP2M1_FXP_ORDER
#define EXP2M1_FXP_ORDER 5
#endif

#ifdef __cplusplus
extern "C" {
#endif

static inline uint64_t exp2m1_fxp_max( void ) { return UINT64_C(0x87fffffff); } /* == 34*2^30 - 1 (i=33,d=2^30-1) */

static inline uint64_t
exp2m1_fxp( uint64_t x ) {

  /* This calculation is based on splitting x into integer and
     fractional bits such that:
       x = 2^30 i + d
     where d is in [0,2^30).  Then we have:
       y = 2^(30+i) exp2( d/2^30 ) - 2^30
         = (2^64 exp2( d/2^30 )) / 2^(64-30-i) - 2^30
         = (2^64 exp2m1( d/2^30 ) + 2^64) / 2^(34-i) - 2^30
         = (2^64 exp2m1( d/2^30 ))/2^(34-i) + 2^(30+i)- 2^30
     Thus, the main challenge is computing an approximation of:
       2^64 exp2m1( d/2^30 ) / 2^(34-i)
     for delta = d/2^30 in [0,1) with good enough precision.  As this
     smoothly goes from 0 to 1, this is an ideal candidate for a
     polynomial approximation (Pade' approximation might be even better
     if we are willing to accept the cost of a divide).  We aren't too
     particular or maxing out precision or precise rounding in the
     interest of compactness and speed.

     The polynomial coefficients below were selected as the polynomials
     that minimize the RMS error over the interval [0,1) while matching
     exactly at 0 and 1 (RMS error was used over minimax as it has more
     intuitive error properties for users typically).  The coefficients
     were scaled such that the intermediates in Horner's rule are all
     less than 2^34 to minimize the coefficient quantization error while
     not overflowing 64 bits when multiplied by d.  The output scale was
     fixed at 2^64 so that we have some extra precision when i is
     non-zero. */

  if( x>exp2m1_fxp_max() ) return UINT64_MAX;

  uint64_t i = x >> 30;                             /* In [0,34) */
  uint64_t d = x & ((UINT64_C(1)<<30)-UINT64_C(1)); /* In [0,2^30) */

  /* Estimate y ~ 2^64 exp2m1( d/2^30 ) */

  uint64_t y;

  /* BEGIN AUTOGENERATED CODE - See KJB for code generator */
# if EXP2M1_FXP_ORDER==1
  /* bits 4.0 rms_aerr 2.6e-02 rms_rerr 1.8e-02 max_aerr 8.6e-02 max_rerr 6.1e-02 */
  /* As implemented: bits 4.0 max_aerr 8.6e-02 max_rerr 6.1e-02 ulp 92418389.1 */
  y = d<<34;
# elif EXP2M1_FXP_ORDER==2
  /* bits 8.3 rms_aerr 2.7e-03 rms_rerr 2.0e-03 max_aerr 3.9e-03 max_rerr 3.2e-03 */
  /* As implemented: bits 8.3 max_aerr 3.9e-03 max_rerr 3.2e-03 ulp 4173744.7 */
  y = UINT64_C(0x2c029d07d);
  y = UINT64_C(0x29feb17c1) + ((y*d)>>31);
  y =  (y*d);
# elif EXP2M1_FXP_ORDER==3
  /* bits 13.0 rms_aerr 1.1e-04 rms_rerr 7.9e-05 max_aerr 1.7e-04 max_rerr 1.2e-04 */
  /* As implemented: bits 13.0 max_aerr 1.7e-04 max_rerr 1.2e-04 ulp 183713.4 */
  y = UINT64_C(0x288319c3e);
  y = UINT64_C(0x1cd1735e6) + ((y*d)>>32);
  y = UINT64_C(0x2c86e3185) + ((y*d)>>31);
  y =  (y*d);
# elif EXP2M1_FXP_ORDER==4
  /* bits 17.6 rms_aerr 3.3e-06 rms_rerr 2.4e-06 max_aerr 5.6e-06 max_rerr 5.0e-06 */
  /* As implemented: bits 17.6 max_aerr 5.6e-06 max_rerr 5.0e-06 ulp 5998.7 */
  y = UINT64_C(0x38100ce15);
  y = UINT64_C(0x1a7f168b9) + ((y*d)>>33);
  y = UINT64_C(0x1eeba70d4) + ((y*d)>>32);
  y = UINT64_C(0x2c5a09747) + ((y*d)>>31);
  y =  (y*d);
# elif EXP2M1_FXP_ORDER==5
  /* bits 22.8 rms_aerr 9.1e-08 rms_rerr 6.7e-08 max_aerr 1.5e-07 max_rerr 1.4e-07 */
  /* As implemented: bits 22.8 max_aerr 1.5e-07 max_rerr 1.4e-07 ulp 165.8 */
  y = UINT64_C(0x3e1a2fa1b);
  y = UINT64_C(0x24a7ddfee) + ((y*d)>>33);
  y = UINT64_C(0x1c994ed30) + ((y*d)>>33);
  y = UINT64_C(0x1ebd13698) + ((y*d)>>32);
  y = UINT64_C(0x2c5c9fe11) + ((y*d)>>31);
  y =  (y*d);
# elif EXP2M1_FXP_ORDER==6
  /* bits 27.9 rms_aerr 2.1e-09 rms_rerr 1.6e-09 max_aerr 4.2e-09 max_rerr 3.9e-09 */
  /* As implemented: bits 27.8 max_aerr 4.6e-09 max_rerr 4.3e-09 ulp 5.0 */
  y = UINT64_C(0x3959e8bc0);
  y = UINT64_C(0x28987867f) + ((y*d)>>33);
  y = UINT64_C(0x27aac1b83) + ((y*d)>>33);
  y = UINT64_C(0x1c67f6aa0) + ((y*d)>>33);
  y = UINT64_C(0x1ebfde70a) + ((y*d)>>32);
  y = UINT64_C(0x2c5c8510d) + ((y*d)>>31);
  y =  (y*d);
# elif EXP2M1_FXP_ORDER==7
  /* bits 33.4 rms_aerr 4.7e-11 rms_rerr 3.5e-11 max_aerr 9.9e-11 max_rerr 9.0e-11 */
  /* As implemented: bits 30.8 max_aerr 5.9e-10 max_rerr 5.4e-10 ulp 0.6 */
  y = UINT64_C(0x2d6e5bd1d);
  y = UINT64_C(0x257992e8b) + ((y*d)>>33);
  y = UINT64_C(0x2c02265a3) + ((y*d)>>33);
  y = UINT64_C(0x27607eb13) + ((y*d)>>33);
  y = UINT64_C(0x1c6b30b08) + ((y*d)>>33);
  y = UINT64_C(0x1ebfbcc25) + ((y*d)>>32);
  y = UINT64_C(0x2c5c8604f) + ((y*d)>>31);
  y =  (y*d);
# else
# error "Unsupported EXP2M1_FXP_ORDER"
# endif
  /* END AUTOGENERATED CODE */

  /* Note: (y + 2^(s-1)) / 2^s does a divide with grade school rounding
     (rounds to nearest with ties away from 0).  The add will not
     overflow as y is <= 2^64-2^34 at this point and 2^(s-1) is at most
     2^33. */

  int s = 34-(int)i; /* In [1,34] so no UB in below shifts */
  return ((y+(UINT64_C(1) << (s-1))) >> s) + (UINT64_C(1)<<(64-s)) - (UINT64_C(1)<<30);
}

#ifdef __cplusplus
}
#endif

#endif /* _pyth_oracle_util_exp2m1_h_ */
