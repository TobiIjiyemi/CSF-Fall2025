#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "fixpoint.h"
#include <stdint.h>
#include <stdlib.h>
////////////////////////////////////////////////////////////////////////
// Helper functions
// Note that you can make these "visible" (not static)
// if you want to be able to write unit tests for them
////////////////////////////////////////////////////////////////////////

// TODO: add helper functions

result_t
fixpoint__add_same_signs(fixpoint_t *result, const fixpoint_t *left, const fixpoint_t *right)
{
  // When both numbers have the same sign
  uint32_t sum_frac = fixpoint_get_frac(left) + fixpoint_get_frac(right);
  uint32_t sum_whole = fixpoint_get_whole(left) + fixpoint_get_whole(right);

  uint32_t carry = (sum_frac < fixpoint_get_frac(left)) ? 1 : 0; // check for overflow in fractional part

  result_t overflow_check = RESULT_OK;

  if (sum_whole < fixpoint_get_whole(left))
  {
    overflow_check = RESULT_OVERFLOW;
  }

  result->whole = sum_whole + carry;

  if (carry == 1 && result->whole < 1)
  {
    overflow_check = RESULT_OVERFLOW;
  }

  result->frac = sum_frac;
  result->negative = fixpoint_is_negative(left); // both have same sign, so take either

  return overflow_check;
}

void fixpoint_add_leftIsLarger(fixpoint_t *result, const fixpoint_t *left, const fixpoint_t *right)
{
  uint32_t left_whole = fixpoint_get_whole(left);
  uint32_t left_frac = fixpoint_get_frac(left);

  uint32_t right_whole = fixpoint_get_whole(right);
  uint32_t right_frac = fixpoint_get_frac(right);

  result->negative = fixpoint_is_negative(left);
  result->whole = left_whole - right_whole;
  if (left_frac >= right_frac)
  {
    result->frac = left_frac - right_frac;
  }
  else
  {
    // Borrow
    result->whole -= 1;
    result->frac = left_frac + (UINT32_MAX - right_frac) + 1;
  }
}

void fixpoint_add_rightIsLarger(fixpoint_t *result, const fixpoint_t *left, const fixpoint_t *right)
{
  uint32_t left_whole = fixpoint_get_whole(left);
  uint32_t left_frac = fixpoint_get_frac(left);

  uint32_t right_whole = fixpoint_get_whole(right);
  uint32_t right_frac = fixpoint_get_frac(right);

  // left + (-right) == left - right
  result->negative = fixpoint_is_negative(right);
  result->whole = right_whole - left_whole;
  if (right_frac >= left_frac)
  {
    result->frac = right_frac - left_frac;
  }
  else
  {
    result->whole -= 1;
    result->frac = right_frac + (UINT32_MAX - left_frac) + 1;
  }
}

////////////////////////////////////////////////////////////////////////
// Public API functions
////////////////////////////////////////////////////////////////////////

void fixpoint_init(fixpoint_t *val, uint32_t whole, uint32_t frac, bool negative)
{
  // might need to check for null input values?
  if (!val)
  {
    return;
  }

  val->whole = whole;
  val->frac = frac;

  if (whole == 0 && frac == 0)
  {
    val->negative = false;
  }
  else
  {
    val->negative = negative;
  }
}

uint32_t
fixpoint_get_whole(const fixpoint_t *val)
{
  // check for null input values?
  return val->whole;
}

uint32_t
fixpoint_get_frac(const fixpoint_t *val)
{
  return val->frac;
}

bool fixpoint_is_negative(const fixpoint_t *val)
{
  return val->negative;
}

void fixpoint_negate(fixpoint_t *val)
{
  if (val->whole == 0 && val->frac == 0)
  {
    val->negative = false;
  }
  else
  {
    val->negative = !(val->negative);
  }
}

result_t
fixpoint_add(fixpoint_t *result, const fixpoint_t *left, const fixpoint_t *right)
{
  // TODO: implement

  if (fixpoint_is_negative(left) == fixpoint_is_negative(right))
  {
    return fixpoint__add_same_signs(result, left, right);
  }

  // When the numbers have different signs
  uint32_t left_whole = fixpoint_get_whole(left);
  uint32_t left_frac = fixpoint_get_frac(left);

  uint32_t right_whole = fixpoint_get_whole(right);
  uint32_t right_frac = fixpoint_get_frac(right);

  if (left_whole == right_whole && left_frac == right_frac)
  {
    result->whole = 0;
    result->frac = 0;
    result->negative = false;
    return RESULT_OK;
  }

  bool left_is_larger = (left_whole > right_whole) || (left_whole == right_whole && left_frac >= right_frac);

  if (left_is_larger)
  {
    fixpoint_add_leftIsLarger(result, left, right);
  }
  else
  {
    fixpoint_add_rightIsLarger(result, left, right);
  }

  if (result->whole == 0 && result->frac == 0)
  {
    result->negative = false; // zero is not negative
  }
  return RESULT_OK; // placeholder to avoid compiler warning
}

result_t
fixpoint_sub(fixpoint_t *result, const fixpoint_t *left, const fixpoint_t *right)
{
  // TODO: implement
  fixpoint_t neg_right = *right;
  fixpoint_negate(&neg_right);
  return fixpoint_add(result, left, &neg_right);
}

result_t 
fixpoint_mul( fixpoint_t *result, const fixpoint_t *left, const fixpoint_t *right ) { 
  uint64_t lwhole= left->whole;
  uint64_t rwhole = right->whole;
  uint64_t lfrac = left->frac;
  uint64_t rfrac = right->frac;
     
  uint32_t boxes[4]; //built to represent full 128 bit block subdivided into 32 bit blocks
  uint64_t AB = lfrac * rfrac;
  uint64_t CD = lwhole * rfrac;
  uint64_t EF = lfrac * rwhole;
  uint64_t GH = rwhole * lwhole;

  uint64_t box1Carry = CD + EF + (AB >> 32);
  uint32_t carry = box1Carry >> 32;
  uint64_t box2holder = (CD >> 32) + (EF >> 32) + (uint32_t)GH + carry;
  uint32_t carry2 = box2holder >> 32; 
  uint64_t carry3 = (GH >> 32) + carry2;
  
  result_t overflow = RESULT_OK;
  boxes[0] = AB;
  boxes[1] = box1Carry;
  boxes[2] = (uint32_t)GH + carry;
  boxes[3] = (uint32_t)carry3;
  result->whole = boxes[2];
  result->frac = boxes[1];
             
  if (boxes[0] > 0 && (boxes[3] > 0 || (carry3 >> 32) > 0) ) { overflow = RESULT_OVERFLOW | RESULT_UNDERFLOW; }
  else if (boxes[0] > 0) { overflow = RESULT_UNDERFLOW; }
  else if (boxes[3] > 0 || (carry3 >> 32) > 0) { overflow = RESULT_OVERFLOW; }
    
  if (left->negative == right->negative || (result->whole == 0 && result->frac == 0 && overflow == RESULT_OK)) { 
    result->negative = false; 
  } else {
    result->negative = true;
  }
  return overflow;
}

int fixpoint_compare(const fixpoint_t *left, const fixpoint_t *right)
{

  if (fixpoint_is_negative(left) && !fixpoint_is_negative(right))
  { // if left is negative and right is non-negative
    return -1;
  }
  else if (!fixpoint_is_negative(left) && fixpoint_is_negative(right))
  { // if left is non-negative and right is negative
    return 1;
  }

  uint32_t left_whole = fixpoint_get_whole(left);
  uint32_t left_frac = fixpoint_get_frac(left);

  uint32_t right_whole = fixpoint_get_whole(right);
  uint32_t right_frac = fixpoint_get_frac(right);

  int cmp;

  // both same sign
  if (left_whole > right_whole)
    cmp = 1;
  else if (right_whole > left_whole)
    cmp = -1;
  else if (left_frac > right_frac)
    cmp = 1;
  else if (right_frac > left_frac)
    cmp = -1;
  else
    cmp = 0;

  if (fixpoint_is_negative(left))
    return -cmp;
  return cmp;
}

void fixpoint_format_hex(fixpoint_str_t *s, const fixpoint_t *val)
{
  // TODO: implement
  char whole_part[9];
  char frac_part[9];
  char temp[19];

  snprintf(whole_part, sizeof(whole_part), "%08x", val->whole);
  snprintf(frac_part, sizeof(frac_part), "%08x", val->frac);

  // removing the leading zeros
  char *whole_ptr = whole_part;
  while (*whole_ptr == '0' && *(whole_ptr + 1) != '\0')
  {
    whole_ptr++;
  }

  // remove trailing zeros
  int length_of_frac = strlen(frac_part);
  int i = length_of_frac - 1;
  while (i > 0 && frac_part[i] == '0') i--;
  frac_part[i + 1] = '\0';

  if (val->negative == true)
  {
    snprintf(temp, sizeof(temp), "-%s.%s", whole_ptr, frac_part);
  }
  else
  {
    snprintf(temp, sizeof(temp), "%s.%s", whole_ptr, frac_part);
  }

  strncpy(s->str, temp, sizeof(s->str));
  s->str[sizeof(s->str) - 1] = '\0';
}

bool
fixpoint_parse_hex( fixpoint_t *val, const fixpoint_str_t *s ) {
  const char *ptr = s->str;
  val->negative = false;
  if (*ptr == '-') {
    val->negative = true;
    ptr++;
  }

  const char* dot = strchr(ptr, '.');

  if (!dot) {
    return false;
  }

  uint32_t wholenum = 0;
  int whole_len = 0;
  int frac_len = 0;

  if (sscanf(ptr, "%8x%n", &wholenum, &whole_len) != 1 || whole_len > 8 || whole_len == 0) {
    return false;
  } else if ((ptr[whole_len] != *dot)) {
    return false;
  }
  val->whole = wholenum;
  uint32_t frac = 0;

  //making sure there are hex digits after the dot
  if (*(dot+1) != '\0') {
    if (sscanf((dot + 1), "%8x%n", &frac, &frac_len) != 1 || frac_len > 8 || frac_len == 0) {
      return false;
    }
    if (*(dot + 1 + frac_len) != '\0') {
      return false;
    }
  } else {
    return false;
  }

  if (val->negative && wholenum == 0 && frac == 0) {
    return false;
  }
  int shift = (8 - frac_len)*4;
  val->frac = frac << shift;
  return true;
}