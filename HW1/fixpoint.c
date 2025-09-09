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

    result_t overflow_check = RESULT_OK;

    if (sum_whole < fixpoint_get_whole(left)) {
      overflow_check = RESULT_OVERFLOW;
    }
    
    result->whole = sum_whole + carry;

    if (carry == 1 && result->whole < 1) {
      overflow_check = RESULT_OVERFLOW;
    }

    result->frac = sum_frac;
    result->negative = fixpoint_is_negative(left); // both have same sign, so take either
    
    return overflow_check;
  }

  // When the numbers have different signs
  uint32_t left_whole = fixpoint_get_whole(left);
  uint32_t left_frac = fixpoint_get_frac(left);

  uint32_t right_whole = fixpoint_get_whole(right);
  uint32_t right_frac = fixpoint_get_frac(right);

  if (left_whole == right_whole && left_frac == right_frac) {
    result->whole = 0;
    result->frac = 0;
    result->negative = false;
    return RESULT_OK;
  }

  bool left_is_larger = (left_whole > right_whole) || (left_whole == right_whole && left_frac >= right_frac);

  if (left_is_larger) {
    result->negative = fixpoint_is_negative(left);
    result->whole = left_whole - right_whole;
    if (left_frac >= right_frac) {
      result->frac = left_frac - right_frac;
    } else {
      // Borrow
      result->whole -= 1;
      result->frac = left_frac + (UINT32_MAX - right_frac) + 1;
    }
  } else {
    // left + (-right) == left - right
    result->negative = fixpoint_is_negative(right);
    result->whole = right_whole-left_whole;
    if (right_frac >= left_frac) {
      result->frac = right_frac - left_frac;
    } else {
      result->whole -= 1;
      result->frac = right_frac + (UINT32_MAX - left_frac) + 1;
    }
  }
  if (result->whole == 0 && result->frac == 0) {
    result->negative = false; // zero is not negative
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
  /*
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
    int32_t lexpo, rexpo = 1;
    while (left > 10) {
      left /= 10;
      lexpo += 1;
    }
    while (right > 10) {
      right /= 10;
      rexpo += 1;
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
  

    //break reuslt into whole and fractional.. like the decimal part, 
    (whole/(10 ^ otherfexpo)) * otherfrac
    //take the otherfractional and the fractional , and call fixpoint_mul
    fixpoint_mul(#, fixpoint_t (0, frac, 0), fixpoint_t (0, otherfrac, 0));

    add the first fractional part with the second fractional part: result->frac;
    result->whole = 

  }
  //whole and frac * frac

  //whole and frac * whole and frac
  //figure out signs at the end of the multiplication operation.
  if (left->negative == right->negative) {
    result->bool = false;
  } else {
    result->bool = true;
  }
 return RESULT_OK; // placeholder to avoid compiler warning
 */
}


int
fixpoint_compare( const fixpoint_t *left, const fixpoint_t *right ) {
  // TODO: implement

  uint32_t left_whole = fixpoint_get_whole(left);
  uint32_t left_frac = fixpoint_get_frac(left);

  uint32_t right_whole = fixpoint_get_whole(right);
  uint32_t right_frac = fixpoint_get_frac(right);

  if (fixpoint_is_negative(left) && !fixpoint_is_negative(right)) { // if left is negative and right is non-negative
    return -1;
  } else if (!fixpoint_is_negative(left) && fixpoint_is_negative(right)) { // if left is non-negative and right is negative
    return 1;
  } else {
    if (!fixpoint_is_negative(left) && !fixpoint_is_negative(right)) { // both non-negative
      if (left_whole > right_whole) {
        return 1; 
      } else if (right_whole > left_whole) {
        return -1;
      } else {
        if (left_frac > right_frac) {
          return 1;
        } else if (right_frac > left_frac) {
          return -1;
        } else {
          return 0;
        }
      }
    } else {
      if (left_whole > right_whole) {
        return -1; 
      } else if (right_whole > left_whole) {
        return 1;
      } else {
        if (left_frac > right_frac) {
          return -1;
        } else if (right_frac > left_frac) {
          return 1;
        } else {
          return 0;
        }
      }
    }
  }
}

void
fixpoint_format_hex( fixpoint_str_t *s, const fixpoint_t *val ) {
  // TODO: implement
  char whole_part[9];
  char frac_part[9];
  char temp[19];

  snprintf(whole_part, sizeof(whole_part), "%08x", val->whole);
  snprintf(frac_part, sizeof(frac_part), "%08x", val->frac);

  // removing the leading zeros
  char *whole_ptr = whole_part;
  while (*whole_ptr == '0' && *(whole_ptr + 1) != '\0') {
    whole_ptr++;
  }

  // remove trailing zeros
  int length_of_frac = strlen(frac_part);
  int i = length_of_frac - 1;
  while (i > 0 && frac_part[i] == '0') {
    i--;
  }
  frac_part[i + 1] = '\0';

  if (val->negative == true) {
    snprintf(temp, sizeof(temp), "-%s.%s", whole_ptr, frac_part);
  } else {
    snprintf(temp, sizeof(temp), "%s.%s", whole_ptr, frac_part);
  }

  strncpy(s->str, temp, sizeof(s->str));
  s->str[sizeof(s->str) - 1] = '\0';

}

bool
fixpoint_parse_hex( fixpoint_t *val, const fixpoint_str_t *s ) {
  // TODO: implement
}
