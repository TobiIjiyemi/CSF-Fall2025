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
  if (fixpoint_is_negative(left) == fixpoint_is_negative(right)) { 
    uint32_t sum_frac = fixpoint_get_frac(left) + fixpoint_get_frac(right);
    uint32_t sum_whole = fixpoint_get_whole(left) + fixpoint_get_whole(right);
    uint32_t carry = (sum_frac < fixpoint_get_frac(left)) ? 1 : 0; // check for overflow in fractional part
    
    result->whole = sum_whole + carry;
    result->frac = sum_frac;
    result->negative = fixpoint_is_negative(left); // both have same sign, so take either
    
    if (result->whole == 0 && result->frac == 0) {
      result->negative = false; // zero is not negative
    }
   
    if (result->whole < sum_whole) {
      return RESULT_OVERFLOW;  
    }
    return RESULT_OK;
  }

  // When the numbers have different signs
  if (fixpoint_is_negative(left) && !fixpoint_is_negative(right)) {
    // (-left) + right  = right - left
    fixpoint_t neg_left = *left;
    fixpoint_negate(&neg_left);

    // right whole > whole part of left or (whole parts equal and right frac >= left frac) then result is the sign of right
    if (fixpoint_get_whole(right) > neg_left.whole || 
        (fixpoint_get_whole(right) == neg_left.whole && fixpoint_get_frac(right) >= neg_left.frac)) {
      result->negative = fixpoint_is_negative(right);
    } else {
      result->negative = fixpoint_is_negative(left);
    }


    if (fixpoint_get_frac(right) >= neg_left.frac) {
      result->frac = fixpoint_get_frac(right) - neg_left.frac;
      result->whole = fixpoint_get_whole(right) - neg_left.whole;
    } else {
      // Borrow from whole part 
      result->frac = (fixpoint_get_frac(right) + (1ULL << 32)) - neg_left.frac; 
      result->whole = (fixpoint_get_whole(right) - 1) - neg_left.whole;
    }         
    if (result->whole == 0 && result->frac == 0) {
      result->negative = false; // zero is not negative
    }
  } else {
    // left + (-right) == left - right
    fixpoint_t neg_right = *right;
    fixpoint_negate(&neg_right);

    if (fixpoint_get_whole(left) > neg_right.whole || 
          (fixpoint_get_whole(left) == neg_right.whole && fixpoint_get_frac(left) >= neg_right.frac)) {
      result->negative = fixpoint_is_negative(left);
    } else {
      result->negative = fixpoint_is_negative(right);
    }
    
    if (fixpoint_get_frac(left) >= neg_right.frac) {
        result->frac = fixpoint_get_frac(left) - neg_right.frac;
        result->whole = fixpoint_get_whole(left) - neg_right.whole;
    } else {
      // Borrow from whole part
      result->frac = (fixpoint_get_frac(left) + (1ULL << 32)) - neg_right.frac;
      result->whole = (fixpoint_get_whole(left) - 1) - neg_right.whole;
    }

    if (result->whole == 0 && result->frac == 0) {
      result->negative = false; // zero is not negative
    }
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
  //frac * frac
  if (left->whole == 0 && right->whole == 0) {
    uint32_t left, right = left->frac, right->frac;
    uint32_t lexpo, rexpo = 1;
    while (left > 10) {
      left /= 10;
      lexpo += 1;
    }
    while (right > 10) {
      right /= 10;
      rexpo += 1;
    }
    if (lexpo + rexpo > 8 || (lexpo + rexpo == 8 && left * right * 10 > 43)) {
      RESULT_UNDERFLOW;
    }
    left,right = left->frac, right->frac;
    result->frac = (left/(10^(lexpo+rexpo))) * right;
    result->whole = 0;
  } else if (left->whole == 0 || right->whole == 0) {
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
    while (fractional > 10) {
      otherfrac /= 10;
      otherfexpo += 1;
    }
    /*

    //break reuslt into whole and fractional.. like the decimal part, 
    (whole/(10 ^ otherfexpo)) * otherfrac
    //take the otherfractional and the fractional , and call fixpoint_mul
    fixpoint_mul(#, fixpoint_t (0, frac, 0), fixpoint_t (0, otherfrac, 0));

    add the first fractional part with the second fractional part: result->frac;
    result->whole = 

    */
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
