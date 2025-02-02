#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "uint256.h"

// Create a UInt256 value from a single uint32_t value.
// Only the least-significant 32 bits are initialized directly,
// all other bits are set to 0.
UInt256 uint256_create_from_u32( uint32_t val ) {
  UInt256 result;
  result.data[0] = val;
  for (int i = 1; i < 8; i++) {
    result.data[i] = 0;
  }
  return result;
}

// Create a UInt256 value from an array of NWORDS uint32_t values.
// The element at index 0 is the least significant, and the element
// at index 7 is the most significant.
UInt256 uint256_create( const uint32_t data[8] ) {
  UInt256 result;
  for (int i = 0; i < 8; i++) {
    result.data[i] = data[i];
  }
  return result;
}

// Create a UInt256 value from a string of hexadecimal digits.
UInt256 uint256_create_from_hex( const char *hex ) {
  UInt256 result = {0};
  size_t length = strlen(hex);
  char *start;
  //if length is over 64, only use the rightmost 64
  if (length > 64) {
    hex += length - 64;
    length = 64;
  } 
  //start at the last value
  char *position = hex + length - 1; 
  int i = 0; 
  while (length > 0) {
    //get 8 bits so that it will never overflow
    char eightBits[9] =  {0};
    int size = 8;

    if (length < 8) {
      size = length;
    }
    for (int i = size - 1; i >= 0; i--) {
      //go backwards for the values
      eightBits[i] = *position;
      position--;
    }
    //function to get the unsigned long value
    result.data[i] = strtoul(eightBits, NULL, 16);
    i++;
    length -= size;
  }


  return result;
}

// Return a dynamically-allocated string of hex digits representing the
// given UInt256 value.
char *uint256_format_as_hex( UInt256 val ) {
  char *hex = malloc(sizeof(char) * 8 * 8 + 1);
  if (!hex) {
    return NULL;
  }
  hex[0] = '\0';
  int endPoint = 0;
  //find the highest endPoint that we must iterate to
  for (int i = 7; i >= 0; i--) {
    if (val.data[i] != 0 ) {
      endPoint = i;
      break;
    }
  }
  //if the value is 0 or only 4 bits (i think this is an edge case)
  if (endPoint == 0) {
    char segment[9];
    uint32_t newval = val.data[0];
    sprintf(segment, "%x", newval);
    strcat(hex, segment);
    return hex;
  }

  //iterate to everything but endpoint
  for (int i = 0; i < endPoint; i++) {
      uint32_t newval = val.data[i];
      char segment[9]; 
      sprintf(segment, "%08x", newval); // format with leading 0s
      strcat(hex, segment);
  }
  //handle endpoint without the leading 0's
  char segment[9];
  uint32_t newval = val.data[endPoint];
  sprintf(segment, "%x", newval); // format with leading 0s
  strcat(hex, segment);
  return hex;
}

// Get 32 bits of data from a UInt256 value.
// Index 0 is the least significant 32 bits, index 7 is the most
// significant 32 bits.
uint32_t uint256_get_bits( UInt256 val, unsigned index ) {
  uint32_t bits;
  if (index >= 8) {
    return 0;
  }
  bits = val.data[index];
  return bits;
}

// Return 1 if bit at given index is set, 0 otherwise.
int uint256_is_bit_set( UInt256 val, unsigned index ) {
  if (index >= 256) {
    return 0; 
  }
  int x = 0;
  //find which index of the array contains the bit
  unsigned positionofBit = index / 32;
  //location of bit inside the position of the array
  unsigned locationofBit = index % 32;

  //taken from our slides from 1/24/25
  x|=(1<<locationofBit);

  //check if the specific location of the bit and the 32 bit location both are 1. 
  //if both are 1, then the bit is set. If the positionofBit in the 32 bit is 0, 
  //then the bit is not set.

  if ((val.data[positionofBit] & x) != 0) {
      return 1; 
  }
    return 0; 
}

// Compute the sum of two UInt256 values.
UInt256 uint256_add( UInt256 left, UInt256 right ) {
  uint32_t carry = 0;
  UInt256 sum;
  for (int i = 0; i < 8; i++) {
    uint32_t leftval = left.data[i];
    uint32_t rightval = right.data[i];

    //add the bitwise values and the carry from the previous uint32
    sum.data[i] = left.data[i] + right.data[i] + carry;

    if (sum.data[i] < leftval || sum.data[i] < rightval) {  
    // the addition overflowed
      carry = 1;
    }
    else {
      carry = 0;
    }
    
  }


  return sum;
}



// Compute the difference of two UInt256 values.
UInt256 uint256_sub( UInt256 left, UInt256 right ) {
  UInt256 result = uint256_add(left, uint256_negate(right));
  return result;
}

// Return the two's-complement negation of the given UInt256 value.
UInt256 uint256_negate( UInt256 val ) {
  UInt256 result;
  UInt256 one = uint256_create_from_u32(1);

  //make result negation of val
  for (int i = 0; i < 8; i++) {
    result.data[i] = ~val.data[i];
  }
  //add 1 to result
  result = uint256_add(result, one);

  return result;
}

void uint256_print(UInt256 val) {
  for (int j = 7; j >= 0; j--) {
    for (int i = 31; i >= 0; i--) {
      printf("%c", (val.data[j] & (1 << i)) ? '1' : '0');
    }
  }
  printf("\n");
}

// Compute the product of two UInt256 values.
UInt256 uint256_mul( UInt256 left, UInt256 right ) {
  UInt256 product;
  // TODO: implement
  return product;
}

UInt256 uint256_lshift( UInt256 val, unsigned shift ) {
  assert( shift < 256 );
  UInt256 result;
  // TODO: implement
  return result;
}
