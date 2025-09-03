#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "fixpoint.h"
#include <stdint.h>

////////////////////////////////////////////////////////////////////////
// Helper functions
// Note that you can make these "visible" (not static)
// if you want to be able to write unit tests for them
////////////////////////////////////////////////////////////////////////

// TODO: add helper functions

////////////////////////////////////////////////////////////////////////
// Public API functions
////////////////////////////////////////////////////////////////////////

void
fixpoint_init( fixpoint_t *val, uint32_t whole, uint32_t frac, bool negative ) {
  //might need to check for null input values?
    if (!val) {
    return;
  }

  val->whole = whole;
  val->frac = frac;

  if (whole == 0 && frac == 0) {
    val->negative = false;
  } else {
    val->negative = negative;
  }
}

uint32_t
fixpoint_get_whole( const fixpoint_t *val ) {
  //check for null input values?
  return val->whole;
}

uint32_t
fixpoint_get_frac( const fixpoint_t *val ) {
  return val->frac;
}

bool
fixpoint_is_negative( const fixpoint_t *val ) {
  return val->negative;
}

void
fixpoint_negate( fixpoint_t *val ) {
  if (val->whole == 0 && val->frac == 0) {
    val->negative = false;
  } else {
     val->negative = !(val->negative);
  }
}

result_t
fixpoint_add( fixpoint_t *result, const fixpoint_t *left, const fixpoint_t *right ) {
  // TODO: implement  
  uint32_t max = UINT32_MAX;
  uint32_t sum_frac = left->frac + right->frac;
  uint32_t sum_whole = left->whole + right->whole;

  // When both numbers have the same sign + addressing negative sign at the end
   if (left->negative && right->negative) {
    if (sum_frac < left->frac || sum_frac < right->frac) { // the sum of the fractional part should be less than either of the addends
      result->frac = sum_frac;
      // carry to the whole part
      if (sum_whole == max) { // overflow occurred, the whole part is already at max value
        result->whole = sum_whole;
        result->negative = left->negative; // both numbers are negative, so the result should be negative
        RESULT_OVERFLOW;
      } else {
        sum_whole += 1;
      }
      sum_whole += 1;
    } else {
      result->frac = sum_frac;
      RESULT_OK;
    }
    if (sum_whole < left->whole || sum_whole < right->whole) { // overflow occurred, the sum of the whole part should be less than either of the addends
      result->whole = sum_whole;
      RESULT_OVERFLOW;
    } else {
      result->whole = sum_whole;
      result->negative = true;
      RESULT_OK;
   }

  // When one number is positive and the other is negative


}

result_t
fixpoint_sub( fixpoint_t *result, const fixpoint_t *left, const fixpoint_t *right ) {
  // TODO: implement
}

result_t
fixpoint_mul( fixpoint_t *result, const fixpoint_t *left, const fixpoint_t *right ) {
  // TODO: implement
}

int
fixpoint_compare( const fixpoint_t *left, const fixpoint_t *right ) {
  // TODO: implement
}

void
fixpoint_format_hex( fixpoint_str_t *s, const fixpoint_t *val ) {
  // TODO: implement
}

bool
fixpoint_parse_hex( fixpoint_t *val, const fixpoint_str_t *s ) {
  // TODO: implement
}
