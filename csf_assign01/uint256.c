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
  // char *start;
  //if length is over 64, only use the rightmost 64
  if (length > 64) {
    hex += length - 64;
    length = 64;
  } 
  //start at the last value
  const char *position = hex + length - 1; 
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
  if (val.data[0] == 0) {
      strcpy(hex, "0"); // Copy "0" into the allocated memory
      return hex;
  }
    char segment[9];
    uint32_t newval = val.data[0];
    sprintf(segment, "%x", newval);
    strcat(hex, segment);
    return hex;
  }


    //handle endpoint without the leading 0's
  char segment[9];
  uint32_t newval = val.data[endPoint];
  sprintf(segment, "%x", newval); // format with leading 0s
  strcat(hex, segment);


  //iterate to everything but endpoint
  for (int i = endPoint-1; i >= 0; i--) {
      uint32_t newval = val.data[i];
      char segment[9];
      sprintf(segment, "%08x", newval); // format with leading 0s
      strcat(hex, segment);
  }
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
    // uint32_t rightval = right.data[i];

    //add the bitwise values and the carry from the previous uint32
    sum.data[i] = left.data[i] + right.data[i] + carry;
     //check if the sum exceeds the max value in that a uint32_t can store
    if (sum.data[i] < leftval || (sum.data[i] == leftval && carry == 1)) {  
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
  //computing result = a - b = a + -b
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

//printing for debug purposes
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
  UInt256 product = uint256_create_from_u32(0);
  //store bit-wise product
  UInt256 term = uint256_create_from_u32(0);
  for (int index = 0; index < 8; index++){
    for (int bitindex = 0; bitindex < 32; bitindex++){
      if((right.data[index] & (1u << bitindex)) == (1u << bitindex)){
        //if nth bit is 1, multiply left value by 2^n
        term = uint256_lshift(left, index*32 + bitindex);
        product = uint256_add(product, term);
      }
    }
  }
  return product;
}

UInt256 uint256_lshift( UInt256 val, unsigned shift ) {
  assert( shift < 256 );
  unsigned indexshift = shift / 32;
  unsigned bitshift = shift % 32;
  uint32_t left;
  uint32_t right;
  UInt256 temp = uint256_create_from_u32(0);
  UInt256 result = uint256_create_from_u32(0);
  for (int i = 7; i >= 0; i--) {
    //separate left and right side of current element
    right = val.data[i] << bitshift;
    if (bitshift > 0) {
      left = val.data[i] >> (32 - bitshift);
    }else{
      //shift 32 leading to undefined behavior
      left = 0;
    }
    if (i + indexshift < 8) {
      //assign right part to appropriate index
      temp.data[i + indexshift] = right;
    }
    if (i + indexshift + 1 < 8) {
      //assign left part to appropriate index
      temp.data[i + indexshift + 1] = left;
    }
    //store current index's shifted value to result, then reset temp
    result = uint256_add(result, temp);
    temp = uint256_create_from_u32(0);
  }
  return result;
}
