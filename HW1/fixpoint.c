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

  // When both numbers have the same sign
  if (left->negative == right->negative) { 
    uint32_t sum_frac = left->frac + right->frac;
    uint32_t sum_whole = left->whole + right->whole;
    uint32_t carry = (sum_frac < left->frac) ? 1 : 0; // check for overflow in fractional part
    
    result->whole = sum_whole + carry;
    result->frac = sum_frac;
    result->negative = left->negative; // both have same sign, so take either
    
    if (result->whole == 0 && result->frac == 0) {
      result->negative = false; // zero is not negative
    }
   
    if (result->whole < left->whole || result->whole < right->whole) {
      return RESULT_OVERFLOW;  
    }
    return RESULT_OK;
  }

  // When the numbers have different signs
    if (left->negative) {
      // (-left) + right  = right - left
      fixpoint_t pos_left = *left;
      pos_left.negative = false;
      return fixpoint_sub(result, &pos_left, right);
    } else { // right - left
      // left + (-right) == left - right
      fixpoint_t pos_right = *right;
      pos_right.negative = false;
      return fixpoint_sub(result, &pos_right, left);
    }
  return RESULT_OK; // placeholder to avoid compiler warning
}


result_t
fixpoint_sub( fixpoint_t *result, const fixpoint_t *left, const fixpoint_t *right ) {
  // TODO: implement
    fixpoint_t neg_right = *right;
    fixpoint_negate(&neg_right); 
    return fixpoint_add(result, left, &neg_right);
}

result_t
fixpoint_mul( fixpoint_t *result, const fixpoint_t *left, const fixpoint_t *right ) {
  uint32_t lwhole, rwhole, lfrac, rfrac = left->whole, right->whole, left->frac, right->frac;
  

  uint64_t WX = (left->whole << 32) + left->frac;
  uint64_t Y = right->whole << 32;
  uint64_t Z = right->frac;
  
  uint64_t PRS = WX * Z;;
  uint64_t TUV = WX * Y;;

  uint64_t product = PRS + (TUV << 32);; //supposed to be 128 bits? but we can't use that
  

  //frac * frac
  if (left->whole == 0 && right->whole == 0) {
    uint64_t left, right = left->frac, right->frac;
    uint32_t product;
    if (left * right > UINT32_MAX) {
      RESULT_UNDERFLOW;
      product = (left * right) % UINT32_MAX;
    }
    product = left * right;
    /*uint32_t lexpo, rexpo = 1;
    while (left > 10) {
      left /= 10;
      lexpo += 1;
    }
    while (right > 10) {
      right /= 10;
      rexpo += 1;*/
    result->whole = 0;
    result->frac = product;
  } else if (left->whole == 0 || right->whole == 0) {
    //if one of the multplied numbers are 0, then we're multplying a fractor by a demical
    //multiple the whole and fractional part (add the sum) multiply the fractional and fractional part and add those togeher
    uint32_t whole, fractional, otherfrac;
    if (left->whole != 0) {
      whole = left->whole;
      fractional = left->frac;
      otherfrac = right->frac;
      
    } else {
      whole = right->whole;
      fractional = right->frac;
      otherfrac = left-> frac;
    }
    uint32_t otherfexpo = 1;
    uint64_t product;
    while (fractional > 10) {
      otherfrac /= 10;
      otherfexpo += 1;
    }
  }
  //whole and frac * frac

  //whole and frac * whole and frac
  //figure out signs at the end of the multiplication operation.
  if (left->negative == right->negative/**/) {
    result->bool = false;
  } else {
    result->bool = true;
  }
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
