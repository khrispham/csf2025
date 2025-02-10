#include <stdio.h>
#include <stdlib.h>
#include "tctest.h"

#include "uint256.h"

typedef struct {
  UInt256 zero; // the value equal to 0
  UInt256 one;  // the value equal to 1
  UInt256 max;  // the value equal to (2^256)-1
  UInt256 msb_set; // the value equal to 2^255
} TestObjs;

// Helper functions for implementing tests
void set_all( UInt256 *val, uint32_t wordval );

#define ASSERT_SAME( expected, actual ) \
do { \
  ASSERT( expected.data[0] == actual.data[0] ); \
  ASSERT( expected.data[1] == actual.data[1] ); \
  ASSERT( expected.data[2] == actual.data[2] ); \
  ASSERT( expected.data[3] == actual.data[3] ); \
  ASSERT( expected.data[4] == actual.data[4] ); \
  ASSERT( expected.data[5] == actual.data[5] ); \
  ASSERT( expected.data[6] == actual.data[6] ); \
  ASSERT( expected.data[7] == actual.data[7] ); \
} while ( 0 )

#define INIT_FROM_ARR( val, arr ) \
do { \
  for ( int i = 0; i < 8; ++i ) \
    val.data[i] = arr[i]; \
} while ( 0 )

// Functions to create and cleanup the test fixture object
TestObjs *setup( void );
void cleanup( TestObjs *objs );

// Declarations of test functions
void test_get_bits( TestObjs *objs );
void test_is_bit_set( TestObjs *objs );
void test_create_from_u32( TestObjs *objs );
void test_create( TestObjs *objs );
void test_create_from_hex( TestObjs *objs );
void test_format_as_hex( TestObjs *objs );
void test_add( TestObjs *objs );
void test_sub( TestObjs *objs );
void test_negate( TestObjs *objs );
void test_neg_overflow( TestObjs *objs );
void test_mul( TestObjs *objs );
void test_lshift( TestObjs *objs );
void test_format_hex_edgecases();
void test_add_edgecases();
void test_subtract_edgecases();
void test_multiply_edgecases();
void test_lshift_edgecases();

int main( int argc, char **argv ) {
  if ( argc > 1 )
    tctest_testname_to_execute = argv[1];

  TEST_INIT();

  TEST( test_get_bits );
  TEST( test_is_bit_set );
  TEST( test_create_from_u32 );
  TEST( test_create );
  TEST( test_create_from_hex );
  TEST( test_format_as_hex );
  TEST( test_add );
  TEST( test_sub );
  TEST( test_negate );
  TEST( test_neg_overflow );
  TEST( test_mul );
  TEST( test_lshift );
  TEST(test_format_hex_edgecases);
  TEST(test_add_edgecases);
  TEST(test_subtract_edgecases);
  TEST(test_multiply_edgecases);
  TEST(test_lshift_edgecases);
  
  TEST_FINI();
}

// Set all of the "words" of a UInt256 to a specific initial value
void set_all( UInt256 *val, uint32_t wordval ) {
  for ( unsigned i = 0; i < 8; ++i ) {
    val->data[i] = wordval;
  }
}

TestObjs *setup( void ) {
  TestObjs *objs = (TestObjs *) malloc( sizeof(TestObjs ) );

  // initialize several UInt256 values "manually"
  set_all( &objs->zero, 0 );
  set_all( &objs->one, 0 );
  objs->one.data[0] = 1U;
  set_all( &objs->max, 0xFFFFFFFFU );

  // create a value with only the most-significant bit set
  uint32_t msb_set_data[8] = { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0x80000000U };
  INIT_FROM_ARR( objs->msb_set, msb_set_data );

  return objs;
}

void cleanup( TestObjs *objs ) {
  free( objs );
}

void test_get_bits( TestObjs *objs ) {
  ASSERT( 0U == uint256_get_bits( objs->zero, 0 ) );
  ASSERT( 0U == uint256_get_bits( objs->zero, 1 ) );
  ASSERT( 0U == uint256_get_bits( objs->zero, 2 ) );
  ASSERT( 0U == uint256_get_bits( objs->zero, 3 ) );
  ASSERT( 0U == uint256_get_bits( objs->zero, 4 ) );
  ASSERT( 0U == uint256_get_bits( objs->zero, 5 ) );
  ASSERT( 0U == uint256_get_bits( objs->zero, 6 ) );
  ASSERT( 0U == uint256_get_bits( objs->zero, 7 ) );

  ASSERT( 1U == uint256_get_bits( objs->one, 0 ) );
  ASSERT( 0U == uint256_get_bits( objs->one, 1 ) );
  ASSERT( 0U == uint256_get_bits( objs->one, 2 ) );
  ASSERT( 0U == uint256_get_bits( objs->one, 3 ) );
  ASSERT( 0U == uint256_get_bits( objs->one, 4 ) );
  ASSERT( 0U == uint256_get_bits( objs->one, 5 ) );
  ASSERT( 0U == uint256_get_bits( objs->one, 6 ) );
  ASSERT( 0U == uint256_get_bits( objs->one, 7 ) );

  ASSERT( 0xFFFFFFFFU == uint256_get_bits( objs->max, 0 ) );
  ASSERT( 0xFFFFFFFFU == uint256_get_bits( objs->max, 1 ) );
  ASSERT( 0xFFFFFFFFU == uint256_get_bits( objs->max, 2 ) );
  ASSERT( 0xFFFFFFFFU == uint256_get_bits( objs->max, 3 ) );
  ASSERT( 0xFFFFFFFFU == uint256_get_bits( objs->max, 4 ) );
  ASSERT( 0xFFFFFFFFU == uint256_get_bits( objs->max, 5 ) );
  ASSERT( 0xFFFFFFFFU == uint256_get_bits( objs->max, 6 ) );
  ASSERT( 0xFFFFFFFFU == uint256_get_bits( objs->max, 7 ) );
}

void test_is_bit_set( TestObjs *objs ) {
  // no bits should be set in the zero value
  for ( int i = 0; i < 256; ++i )
    ASSERT( !uint256_is_bit_set( objs->zero, i ) );

  // only bit 0 should be set in the one value
  ASSERT( uint256_is_bit_set( objs->one, 0 ) );
  for ( int i = 1; i < 256; ++i )
    ASSERT( !uint256_is_bit_set( objs->one, i ) );

  // only bit 255 should be set in the msb_set value
  for ( int i = 0; i < 255; ++i )
    ASSERT( !uint256_is_bit_set( objs->msb_set, i ) );
  ASSERT( uint256_is_bit_set( objs->msb_set, 255 ) );

  // all bits should be set in the max value
  for ( int i = 0; i < 256; ++i )
    ASSERT( uint256_is_bit_set( objs->max, i ) );
}

void test_create_from_u32( TestObjs *objs ) {
  UInt256 zero = uint256_create_from_u32( 0U );
  UInt256 one = uint256_create_from_u32( 1U );

  ASSERT_SAME( objs->zero, zero );
  ASSERT_SAME( objs->one, one );
}

void test_create( TestObjs *objs ) {
  (void) objs;

  uint32_t data1[8] = { 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U };
  UInt256 val1 = uint256_create( data1 );
  ASSERT( 1U == val1.data[0] );
  ASSERT( 2U == val1.data[1] );
  ASSERT( 3U == val1.data[2] );
  ASSERT( 4U == val1.data[3] );
  ASSERT( 5U == val1.data[4] );
  ASSERT( 6U == val1.data[5] );
  ASSERT( 7U == val1.data[6] );
  ASSERT( 8U == val1.data[7] );
}

void test_create_from_hex( TestObjs *objs ) {
  UInt256 zero = uint256_create_from_hex( "0" );
  ASSERT_SAME( objs->zero, zero );

  UInt256 one = uint256_create_from_hex( "1" );
  ASSERT_SAME( objs->one, one );

  UInt256 max = uint256_create_from_hex( "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff" );
  ASSERT_SAME( objs->max, max );
}

void test_format_as_hex( TestObjs *objs ) {
  char *s;

  s = uint256_format_as_hex( objs->zero );
  ASSERT( 0 == strcmp( "0", s ) );
  free( s );

  s = uint256_format_as_hex( objs->one );
  ASSERT( 0 == strcmp( "1", s ) );
  free( s );

  s = uint256_format_as_hex( objs->max );
  ASSERT( 0 == strcmp( "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", s ) );
  free( s );
}


void test_add_edgecases() {
  char *s;
  //edge case adding to overflow
  UInt256 zero = uint256_create_from_hex( "0" );
  UInt256 one = uint256_create_from_hex( "1" );
  UInt256 max = uint256_create_from_hex( "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff" );
  UInt256 result;
  UInt256 value1;
  UInt256 value2;
  result = uint256_add(one, max);
  s = uint256_format_as_hex( result );
  ASSERT( 0 == strcmp( "0", s ) );
  free( s);

  //edgecase adding two 0s
  result = uint256_add(zero, zero);
  s = uint256_format_as_hex(result);
  ASSERT(0 == strcmp("0", s));
  free(s);

  //edge case causing carrys
  UInt256 carry = uint256_create_from_hex("ffffffff00000000ffffffff00000000ffffffff00000000ffffffff00000000");
  result = uint256_add(carry, one);
  s = uint256_format_as_hex(result);
  ASSERT(0 == strcmp("ffffffff00000000ffffffff00000000ffffffff00000000ffffffff00000001", s));
  free(s);

  //edge case adding 0
  result = uint256_add(zero, max);
  s = uint256_format_as_hex( result );
  ASSERT( 0 == strcmp( "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", s ) );
  free( s);

  //edge case adding two maxes
  result = uint256_add(max, max);
  s = uint256_format_as_hex(result);
  ASSERT(0 == strcmp("fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffe", s));
  free(s);

  //cases using genfact.rb
  value1 = uint256_create_from_hex( "3f8686bd2e40f6a604ea22542d1ef1a0c1133c126daefc777213ed212541be2" );
  value2 = uint256_create_from_hex( "572a40a707971f64b79ce5c99adba9baaf98a27308a07cc8f5f1d1d5f1d3380" );
  result = uint256_add(value1, value2);
  s = uint256_format_as_hex( result );
  ASSERT( 0 == strcmp( "96b0c76435d8160abc87081dc7fa9b5b70abde85764f79406805bef71714f62", s ) );
  free( s);

  value1 = uint256_create_from_hex( "8e91ef1c3b04397515d0e6a92512b58d94163b9d5ac5b027d2ab7b0fcf3c42e" );
  value2 = uint256_create_from_hex( "fcb121d798221417eb4b59a27681fd05cf389a641230efc73731f99a2ade38f" );
  result = uint256_add(value1, value2);
  s = uint256_format_as_hex( result );
  ASSERT( 0 == strcmp( "18b4310f3d3264d8d011c404b9b94b293634ed6016cf69fef09dd74a9fa1a7bd", s ) );
  free( s);

  value1 = uint256_create_from_hex( "b16339873082dddf37f98a7fd34797d0e4857400d25ff9bae515abf3c3a4d50" );
  value2 = uint256_create_from_hex( "7f287a693f0c41314115124ede4f0ffebca4640ff3cfebe115722523adf4dc6" );
  result = uint256_add(value1, value2);
  s = uint256_format_as_hex( result );
  ASSERT( 0 == strcmp( "1308bb3f06f8f1f10790e9cceb196a7cfa129d810c62fe59bfa87d1177199b16", s ) );
  free( s);

  
}


void test_subtract_edgecases() {
  char *s;
  UInt256 zero = uint256_create_from_hex( "0" );
  UInt256 one = uint256_create_from_hex( "1" );
  UInt256 max = uint256_create_from_hex( "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff" );
  UInt256 result;
  UInt256 value1;
  UInt256 value2;
  //edge case subtracting to overflow
  result = uint256_sub(zero, one);
  s = uint256_format_as_hex( result );
  ASSERT( 0 == strcmp( "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", s ) );
  free( s);

  //edgecase subtracting two 0s
  result = uint256_sub(zero, zero);
  s = uint256_format_as_hex(result);
  ASSERT(0 == strcmp("0", s));
  free(s);

  //edge case subtracting 0
  result = uint256_sub(max, zero);
  s = uint256_format_as_hex( result );
  ASSERT( 0 == strcmp( "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", s ) );
  free( s);

  //edge case subtracting two maxes
  result = uint256_sub(max, max);
  s = uint256_format_as_hex(result);
  ASSERT(0 == strcmp("0", s));
  free(s);
  //edge case subtracting 1 from max value
  result = uint256_sub(max, one);
  s = uint256_format_as_hex(result);
  ASSERT(0 == strcmp("fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffe", s));
  free(s);
  


  //cases using genfact.rb

  value1 = uint256_create_from_hex( "55f87153c6a5894edca3b6fcd8b0d608341c7c90975b62b78f5c2d70dcea8a9" );
  value2 = uint256_create_from_hex( "d8d1d910fc0177a85db706b0cca8d00c14d08b859857b4c778f6808a900e0d" );
  result = uint256_sub(value1, value2);
  s = uint256_format_as_hex( result );
  ASSERT( 0 == strcmp( "486b53c2b6e571d456c84691cbe6490772cf73d83dd5e76b17ccc56833e9a9c", s ) );
  free( s);

  value1 = uint256_create_from_hex( "373c3a22bfe914db11da3eb91b8a0ff57e308f8eee8c35a903954c64b5ef0ab" );
  value2 = uint256_create_from_hex( "1cb88d98e840c9761722eb057c70adb9220d63eada0175ec6b7774b89c89b2a" );
  result = uint256_sub(value1, value2);
  s = uint256_format_as_hex( result );
  ASSERT( 0 == strcmp( "1a83ac89d7a84b64fab753b39f19623c5c232ba4148abfbc981dd7ac1965581", s ) );
  free( s);
  
  value1 = uint256_create_from_hex( "cfbd12e4c502f8a044b4285891e1a353a313e1b53f45eecaec19bca042aa564" );
  value2 = uint256_create_from_hex( "7402ef1466bacb9316fb807928d2f7e3fc4b40efe5eec9a409fc9ff2c461748" );
  result = uint256_sub(value1, value2);
  s = uint256_format_as_hex( result );
  ASSERT( 0 == strcmp( "5bba23d05e482d0d2db8a7df690eab6fa6c8a0c559572526e21d1cad7e48e1c", s ) );
  free( s);
}

void test_multiply_edgecases() {
  char *s;
  UInt256 zero = uint256_create_from_hex( "0" );
  UInt256 one = uint256_create_from_hex( "1" );
  UInt256 max = uint256_create_from_hex( "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff" );
  UInt256 result;
  UInt256 value1;
  UInt256 value2;
  //edge case multiplying with 0 in left position
  result = uint256_mul(zero, one);
  s = uint256_format_as_hex( result );
  ASSERT( 0 == strcmp( "0", s ) );
  free( s);

  //edge case multiplying with 0 in right position
  result = uint256_mul(one, zero);
  s = uint256_format_as_hex( result );
  ASSERT( 0 == strcmp( "0", s ) );
  free( s);

  //edgecase multiplying two 0s
  result = uint256_mul(zero, zero);
  s = uint256_format_as_hex(result);
  ASSERT(0 == strcmp("0", s));
  free(s);

  //edgecase multiplying two maxes
  result = uint256_mul(max, max);
  s = uint256_format_as_hex(result);
  ASSERT(0 == strcmp("1", s));
  free(s);


  //cases using genfact.rb

  value1 = uint256_create_from_hex( "d73539841d5b290d08f248b01205b38" );
  value2 = uint256_create_from_hex( "cbfe48a152135f42442b08d54771eeb" );
  result = uint256_mul(value1, value2);
  s = uint256_format_as_hex( result );
  ASSERT( 0 == strcmp( "ab7cf8796a0213cc367c0841d680fee5a491295fbedfc914a3b78c9a6c4c68", s ) );
  free( s);

  value1 = uint256_create_from_hex( "ac5ce9adb467d90c5537753eb4bbee2" );
  value2 = uint256_create_from_hex( "5e99ced2d244a466f25c0257fb0df90" );
  result = uint256_mul(value1, value2);
  s = uint256_format_as_hex( result );
  ASSERT( 0 == strcmp( "3fb1ac9626e2da7c33888a2af3ae7276f1fdfc72d55e03c517a79aaf423d20", s ) );
  free( s);
  
  value1 = uint256_create_from_hex( "4b9bb578e4b5f09405b21192ae61b1f" );
  value2 = uint256_create_from_hex( "bf2b1b9b867228b52b7eed2d316bdf8" );
  result = uint256_mul(value1, value2);
  s = uint256_format_as_hex( result );
  ASSERT( 0 == strcmp( "3875e7b4091201b9717d01927d6cdf2ea23aaf914a4d1fa55f0e9c529a2908", s ) );
  free( s);
}

void test_format_hex_edgecases() {
  char *s;
      //test to make sure it works with lots of 0's in between values
  UInt256 value1 = uint256_create_from_hex( "f000000000000000000000000f" );
  s = uint256_format_as_hex( value1 );
  ASSERT( 0 == strcmp( "f000000000000000000000000f", s ) );
  free( s);


  //0 edge case
  UInt256 value2 = uint256_create_from_hex( "0" );
  s = uint256_format_as_hex( value2 );
  ASSERT( 0 == strcmp( "0", s ) );
  free( s);

    //only one significant 32 bit
  UInt256 value3 = uint256_create_from_hex( "f000000000000000000000000000000000000000000000000000000000000000" );
  
  s = uint256_format_as_hex( value3 );
  ASSERT( 0 == strcmp( "f000000000000000000000000000000000000000000000000000000000000000", s ) );
  free( s);
}



void test_add( TestObjs *objs ) {
  UInt256 result;

  result = uint256_add( objs->zero, objs->zero );
  ASSERT_SAME( objs->zero, result );

  result = uint256_add( objs->zero, objs->one );
  ASSERT_SAME( objs->one, result );

  result = uint256_add( objs->one, objs->zero );
  ASSERT_SAME( objs->one, result );

  uint32_t two_data[8] = { 2U };
  UInt256 two;
  INIT_FROM_ARR( two, two_data );
  result = uint256_add( objs->one, objs->one );
  ASSERT_SAME( two, result );

  result = uint256_add( objs->max, objs->one );
  ASSERT_SAME( objs->zero, result );

  result = uint256_add( objs->one, objs->max );
  ASSERT_SAME( objs->zero, result );

  result = uint256_add( two, objs->max );
  ASSERT_SAME( objs->one, result );

  result = uint256_add( objs->max, two );
  ASSERT_SAME( objs->one, result );
}

void test_sub( TestObjs *objs ) {
  UInt256 result;
  uint32_t two_data[8] = { 2U };
  UInt256 two;
  INIT_FROM_ARR( two, two_data );

  result = uint256_sub( objs->zero, objs->zero );
  ASSERT_SAME( objs->zero, result );

  result = uint256_sub( objs->max, objs->zero );
  ASSERT_SAME( objs->max, result );

  result = uint256_sub( objs->one, objs->one );
  ASSERT_SAME( objs->zero, result );

  result = uint256_sub( objs->max, objs->max );
  ASSERT_SAME( objs->zero, result );

  result = uint256_sub( objs->zero, objs->one );
  ASSERT_SAME( objs->max, result );

  result = uint256_sub( objs->zero, objs->max );
  ASSERT_SAME( objs->one, result );

  result = uint256_sub( objs->one, objs->max );
  ASSERT_SAME( two, result );

  result = uint256_sub( objs->one, two );
  ASSERT_SAME( objs->max, result );
}

void test_negate( TestObjs *objs ) {
  UInt256 result;

  result = uint256_negate( objs->zero );
  ASSERT_SAME( objs->zero, result );

  result = uint256_negate( objs->one );
  ASSERT_SAME( objs->max, result );

  result = uint256_negate( objs->max );
  ASSERT_SAME( objs->one, result );
}

void test_neg_overflow( TestObjs *objs ) {
  UInt256 result;

  uint32_t two_data[8] = { 2U };
  UInt256 two;
  INIT_FROM_ARR( two, two_data );

  result = uint256_sub( objs->one, objs->max );
  ASSERT_SAME( two, result );
}

void test_mul( TestObjs *objs ) {
  UInt256 left, right, result;

  // some very basic tests

  result = uint256_mul( objs->one, objs->one );
  ASSERT_SAME( objs->one, result );

  result = uint256_mul( objs->one, objs->zero );
  ASSERT_SAME( objs->zero, result );

  result = uint256_mul( objs->zero, objs->max );
  ASSERT_SAME( objs->zero, result );

  result = uint256_mul( objs->zero, objs->one );
  ASSERT_SAME( objs->zero, result );

  UInt256 two = { { 2,0,0,0,0,0,0,0 } }, four = { { 4,0,0,0,0,0,0,0 } };
  result = uint256_mul( two, two );
  ASSERT_SAME( four, result );

  // a more complicated test

  // fc42c691d6284761fb49dd54f3a13eb * 4cfd2c7d8790c50c280ff0ff77617a8e = 4bdd4cc8b6067f7617c05917f828d17a26046ba5f436cb7df595f6c68c00a5a
  left.data[0] = 0x4f3a13ebU;
  left.data[1] = 0x1fb49dd5U;
  left.data[2] = 0x1d628476U;
  left.data[3] = 0xfc42c69U;
  left.data[4] = 0x0U;
  left.data[5] = 0x0U;
  left.data[6] = 0x0U;
  left.data[7] = 0x0U;
  right.data[0] = 0x77617a8eU;
  right.data[1] = 0x280ff0ffU;
  right.data[2] = 0x8790c50cU;
  right.data[3] = 0x4cfd2c7dU;
  right.data[4] = 0x0U;
  right.data[5] = 0x0U;
  right.data[6] = 0x0U;
  right.data[7] = 0x0U;
  result = uint256_mul(left, right);
  ASSERT(0x68c00a5aU == result.data[0]);
  ASSERT(0xdf595f6cU == result.data[1]);
  ASSERT(0x5f436cb7U == result.data[2]);
  ASSERT(0xa26046baU == result.data[3]);
  ASSERT(0x7f828d17U == result.data[4]);
  ASSERT(0x617c0591U == result.data[5]);
  ASSERT(0x8b6067f7U == result.data[6]);
  ASSERT(0x4bdd4ccU == result.data[7]);
}

void test_lshift( TestObjs *objs ) {
  UInt256 result;

  UInt256 two = { { 2,0,0,0,0,0,0,0 } }, four = { { 4,0,0,0,0,0,0,0 } }, sixteen = { {16,0,0,0,0,0,0,0} };
  

  // some very basic tests

  result = uint256_lshift( objs->one, 0 );
  ASSERT_SAME( objs->one, result );

  result = uint256_lshift( objs->one, 1 );
  ASSERT_SAME( two, result );

  result = uint256_lshift( objs->one, 2 );
  ASSERT_SAME( four, result );

  result = uint256_lshift( objs->one, 4 );
  ASSERT_SAME( sixteen, result );

  // a more complicated test
  {
    // Test shifting 727767d07ccff5fe25cd125b4523e8c7db1b8d1a2c8a2830284d72bb872c33a5 left by 50 bit(s)
    uint32_t arr[8] = {0x872c33a5U, 0x284d72bbU, 0x2c8a2830U, 0xdb1b8d1aU, 0x4523e8c7U, 0x25cd125bU, 0x7ccff5feU, 0x727767d0U};
    UInt256 val;
    INIT_FROM_ARR( val, arr );
    uint32_t expected_arr[8] = {0x00000000U, 0xce940000U, 0xcaee1cb0U, 0xa0c0a135U, 0x3468b228U, 0xa31f6c6eU, 0x496d148fU, 0xd7f89734U};
    UInt256 expected;
    INIT_FROM_ARR( expected, expected_arr );
    result = uint256_lshift( val, 50U );
    ASSERT_SAME( expected, result );
  }
}

void test_lshift_edgecases() {
  UInt256 zero = uint256_create_from_hex( "0" );
  UInt256 one = uint256_create_from_hex( "1" );
  UInt256 result;

  // Shift 1 left by 255 bits
  UInt256 expected255 = {{0, 0, 0, 0, 0, 0, 0, 0x80000000}};
  result = uint256_lshift(one, 255);
  ASSERT_SAME(expected255, result);

  // Shift 2nd highest index by 32
  UInt256 high32 = {{0, 0, 0, 0, 0, 0, 0x80000000, 0}};
  UInt256 expectedhigh32 = {{0, 0, 0, 0, 0, 0, 0, 0x80000000}};
  result = uint256_lshift(high32, 32);
  ASSERT_SAME(expectedhigh32, result);

  // Shift highest index by 32
  UInt256 highest32 = {{0, 0, 0, 0, 0, 0, 0, 0x80000000}};
  result = uint256_lshift(highest32, 32);
  ASSERT_SAME(zero, result);

  // Shift lowest index by 32
  UInt256 low32 = {{0xFFFFFFFF, 0, 0, 0, 0, 0, 0, 0}};
  UInt256 expectedlow32 = {{0, 0xFFFFFFFF, 0, 0, 0, 0, 0, 0}};
  result = uint256_lshift(low32, 32);
  ASSERT_SAME(expectedlow32, result);

  // Shift zero
  result = uint256_lshift(zero, 50);
  ASSERT_SAME(zero, result);

  // Shift random number by 64 bits
  UInt256 randnum = {{0x12345678, 0x9ABCDEF0, 0xDEADBEEF, 0xCAFEBABE,
                       0x0BADF00D, 0xBAADF00D, 0xFEEDFACE, 0xF00DFACE}};
  UInt256 expected64 = {{0, 0, 0x12345678, 0x9ABCDEF0, 
                                0xDEADBEEF, 0xCAFEBABE, 0x0BADF00D, 0xBAADF00D}};
  result = uint256_lshift(randnum, 64);
  ASSERT_SAME(expected64, result);
}

