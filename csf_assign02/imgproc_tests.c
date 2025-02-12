#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tctest.h"
#include "imgproc.h"

// An expected color identified by a (non-zero) character code.
// Used in the "struct Picture" data type.
struct ExpectedColor {
  char c;
  uint32_t color;
};

// Type representing a "picture" of an expected image.
// Useful for creating a very simple Image to be accessed
// by test functions.
struct Picture {
  struct ExpectedColor colors[40];
  int width, height;
  const char *data;
};

// Some "basic" colors to use in test struct Pictures.
// Note that the ranges '1'-'5', 'A'-'E', and 'P'-'T'
// are (respectively) colors 'r','g','b','c', and 'm'
// with just the red, green, and blue color component values,
// which is useful for defining the expected image from the
// "rgb" transformation.
#define TEST_COLORS \
    { \
      { ' ', 0x000000FF }, \
      { 'r', 0xFF0000FF }, \
      { 'g', 0x00FF00FF }, \
      { 'b', 0x0000FFFF }, \
      { 'c', 0x00FFFFFF }, \
      { 'm', 0xFF00FFFF }, \
      { '1', 0xFF0000FF }, \
      { '2', 0x000000FF }, \
      { '3', 0x000000FF }, \
      { '4', 0x000000FF }, \
      { '5', 0xFF0000FF }, \
      { 'A', 0x000000FF }, \
      { 'B', 0x00FF00FF }, \
      { 'C', 0x000000FF }, \
      { 'D', 0x00FF00FF }, \
      { 'E', 0x000000FF }, \
      { 'P', 0x000000FF }, \
      { 'Q', 0x000000FF }, \
      { 'R', 0x0000FFFF }, \
      { 'S', 0x0000FFFF }, \
      { 'T', 0x0000FFFF }, \
    }

// Expected "basic" colors after grayscale transformation
#define TEST_COLORS_GRAYSCALE \
    { \
      { ' ', 0x000000FF }, \
      { 'r', 0x4E4E4EFF }, \
      { 'g', 0x7F7F7FFF }, \
      { 'b', 0x303030FF }, \
      { 'c', 0xB0B0B0FF }, \
      { 'm', 0x7F7F7FFF }, \
    }

// Data type for the test fixture object.
// This contains data (including Image objects) that
// can be accessed by test functions. This is useful
// because multiple test functions can access the same
// data (so you don't need to create/initialize that
// data multiple times in different test functions.)
typedef struct {
  // smiley-face picture
  struct Picture smiley_pic;

  // original smiley-face Image object
  struct Image *smiley;

  // empty Image object to use for output of
  // transformation on smiley-face image
  struct Image *smiley_out;

  // empty Image object to use for output of rgb
  // transformation on smiley-face image
  struct Image *smiley_rgb_out;

  // a square image (same width/height) to use as a test
  // for the kaleidoscope transformation
  struct Picture sq_test_pic;

  // original square Image object
  struct Image *sq_test;

  // empty image for output of kaleidoscope transformation
  struct Image *sq_test_out;

} TestObjs;

// Functions to create and clean up a test fixture object
TestObjs *setup( void );
void cleanup( TestObjs *objs );

// Helper functions used by the test code
struct Image *picture_to_img( const struct Picture *pic );
uint32_t lookup_color(char c, const struct ExpectedColor *colors);
bool images_equal( struct Image *a, struct Image *b );
void destroy_img( struct Image *img );

// Test functions
void test_rgb_basic( TestObjs *objs );
void test_grayscale_basic( TestObjs *objs );
void test_fade_basic( TestObjs *objs );
void test_kaleidoscope_basic( TestObjs *objs );
void test_get_r( TestObjs *objs );
void test_get_g( TestObjs *objs );
void test_get_b( TestObjs *objs );
void test_get_a( TestObjs *objs );
void test_make_pixel( TestObjs *objs );
void test_to_grayscale( TestObjs *objs );
//void test_gradient( TestObjs *objs );
void test_compute_index( TestObjs *objs );
// TODO: add prototypes for additional test functions

int main( int argc, char **argv ) {
  // allow the specific test to execute to be specified as the
  // first command line argument
  if ( argc > 1 )
    tctest_testname_to_execute = argv[1];

  TEST_INIT();

  // Run tests.
  // Make sure you add additional TEST() macro invocations
  // for any additional test functions you add.
  TEST( test_rgb_basic );
  TEST( test_grayscale_basic );
  TEST( test_fade_basic );
  TEST( test_kaleidoscope_basic );
  TEST( test_get_r );
  TEST( test_get_g );
  TEST( test_get_b );
  TEST( test_get_a );
  TEST( test_make_pixel );
  TEST( test_to_grayscale );
  //TEST( test_gradient );
  TEST( test_compute_index );

  TEST_FINI();
}

////////////////////////////////////////////////////////////////////////
// Test fixture setup/cleanup functions
////////////////////////////////////////////////////////////////////////

TestObjs *setup( void ) {
  TestObjs *objs = (TestObjs *) malloc( sizeof(TestObjs) );

  struct Picture smiley_pic = {
    TEST_COLORS,
    16, // width
    10, // height
    "    mrrrggbc    "
    "   c        b   "
    "  r   r  b   c  "
    " b            b "
    " b            r "
    " g   b    c   r "
    "  c   ggrb   b  "
    "   m        c   "
    "    gggrrbmc    "
    "                "
  };
  objs->smiley_pic = smiley_pic;
  objs->smiley = picture_to_img( &smiley_pic );

  objs->smiley_out = (struct Image *) malloc( sizeof( struct Image ) );
  img_init( objs->smiley_out, objs->smiley->width, objs->smiley->height );

  // the rgb transformation requires an output Image with width
  // and height twice the original image
  objs->smiley_rgb_out = (struct Image *) malloc( sizeof( struct Image ) );
  img_init( objs->smiley_rgb_out, objs->smiley->width * 2, objs->smiley->height * 2 );

  struct Picture sq_test_pic = {
    TEST_COLORS,
    12, // width
    12, // height
    "rrrrrr      "
    " ggggg      "
    "  bbbb      "
    "   mmm      "
    "    cc      "
    "     r      "
    "            "
    "            "
    "            "
    "            "
    "            "
    "            "
  };
  objs->sq_test_pic = sq_test_pic;
  objs->sq_test = picture_to_img( &sq_test_pic );
  objs->sq_test_out = (struct Image *) malloc( sizeof( struct Image ) );
  img_init( objs->sq_test_out, objs->sq_test->width, objs->sq_test->height );

  return objs;
}

void cleanup( TestObjs *objs ) {
  destroy_img( objs->smiley );
  destroy_img( objs->smiley_out );
  destroy_img( objs->smiley_rgb_out );
  destroy_img( objs->sq_test );
  destroy_img( objs->sq_test_out );

  free( objs );
}

////////////////////////////////////////////////////////////////////////
// Test code helper functions
////////////////////////////////////////////////////////////////////////

struct Image *picture_to_img( const struct Picture *pic ) {
  struct Image *img;

  img = (struct Image *) malloc( sizeof(struct Image) );
  img_init( img, pic->width, pic->height );

  for ( int i = 0; i < pic->height; ++i ) {
    for ( int j = 0; j < pic->width; ++j ) {
      int index = i * img->width + j;
      uint32_t color = lookup_color( pic->data[index], pic->colors );
      img->data[index] = color;
    }
  }

  return img;
}

uint32_t lookup_color(char c, const struct ExpectedColor *colors) {
  for (int i = 0; ; i++) {
    assert(colors[i].c != 0);
    if (colors[i].c == c) {
      return colors[i].color;
    }
  }
}

// Returns true IFF both Image objects are identical
bool images_equal( struct Image *a, struct Image *b ) {
  if ( a->width != b->width || a->height != b->height )
    return false;

  for ( int i = 0; i < a->height; ++i )
    for ( int j = 0; j < a->width; ++j ) {
      int index = i*a->width + j;
      if ( a->data[index] != b->data[index] )
        return false;
    }

  return true;
}

void destroy_img( struct Image *img ) {
  if ( img != NULL )
    img_cleanup( img );
  free( img );
}

uint32_t randomRGBA(void) {
  // Generate a random 32-bit integer.
  uint32_t red   = (uint32_t)(rand() % 256);
  uint32_t green = (uint32_t)(rand() % 256);
  uint32_t blue  = (uint32_t)(rand() % 256);
  uint32_t alpha = (uint32_t)(rand() % 256);

  // Pack the channels into a single 32-bit value.
  return (red << 24) | (green << 16) | (blue << 8) | (alpha);
}

////////////////////////////////////////////////////////////////////////
// Test helper functions
////////////////////////////////////////////////////////////////////////

void test_get_r( TestObjs *objs ) {
  // Set up pixel data with known values.
  uint32_t red = 0xFF0000FF;
  uint32_t green = 0x00FF00FF;
  uint32_t blue = 0x0000FFFF;
  uint32_t black = 0x000000FF;
  uint32_t white = 0xFFFFFFFF;
  uint32_t funny = 0x69696969;
  uint32_t transparent = 0xABCDEF00;
  uint32_t random1 = 0x824525EB;
  uint32_t random2 = 0x0182DC12;


  // Test each color
  uint32_t newred = get_r(red);
  ASSERT(newred == 0xFF);

  uint32_t newgreen = get_r(green);
  ASSERT(newgreen == 0x00);

  uint32_t newblue = get_r(blue);
  ASSERT(newblue == 0x00);

  uint32_t newblack = get_r(black);
  ASSERT(newblack == 0x00);

  uint32_t newwhite = get_r(white);
  ASSERT(newwhite == 0xFF);

  uint32_t newfunny = get_r(funny);
  ASSERT(newfunny == 0x69);

  uint32_t newtransparent = get_r(transparent);
  ASSERT(newtransparent == 0xAB);

  uint32_t newrandom1 = get_r(random1);
  ASSERT(newrandom1 == 0x82);

  uint32_t newrandom2 = get_r(random2);
  ASSERT(newrandom2 == 0x01);  
}

void test_get_g( TestObjs *objs ) {
  // Set up pixel data with known values.
  uint32_t red = 0xFF0000FF;
  uint32_t green = 0x00FF00FF;
  uint32_t blue = 0x0000FFFF;
  uint32_t black = 0x000000FF;
  uint32_t white = 0xFFFFFFFF;
  uint32_t funny = 0x69696969;
  uint32_t transparent = 0xABCDEF00;
  uint32_t random1 = 0x824525EB;
  uint32_t random2 = 0x0182DC12;


  // Test each color
  uint32_t newred = get_g(red);
  ASSERT(newred == 0x00);

  uint32_t newgreen = get_g(green);
  ASSERT(newgreen == 0xFF);

  uint32_t newblue = get_g(blue);
  ASSERT(newblue == 0x00);

  uint32_t newblack = get_g(black);
  ASSERT(newblack == 0x00);

  uint32_t newwhite = get_g(white);
  ASSERT(newwhite == 0xFF);

  uint32_t newfunny = get_g(funny);
  ASSERT(newfunny == 0x69);

  uint32_t newtransparent = get_g(transparent);
  ASSERT(newtransparent == 0xCD);

  uint32_t newrandom1 = get_g(random1);
  ASSERT(newrandom1 == 0x45);

  uint32_t newrandom2 = get_g(random2);
  ASSERT(newrandom2 == 0x82); 
}

void test_get_b( TestObjs *objs ) {
  // Set up pixel data with known values.
  uint32_t red = 0xFF0000FF;
  uint32_t green = 0x00FF00FF;
  uint32_t blue = 0x0000FFFF;
  uint32_t black = 0x000000FF;
  uint32_t white = 0xFFFFFFFF;
  uint32_t funny = 0x69696969;
  uint32_t transparent = 0xABCDEF00;
  uint32_t random1 = 0x824525EB;
  uint32_t random2 = 0x0182DC12;


  // Test each color
  uint32_t newred = get_b(red);
  ASSERT(newred == 0x00);

  uint32_t newgreen = get_b(green);
  ASSERT(newgreen == 0x00);

  uint32_t newblue = get_b(blue);
  ASSERT(newblue == 0xFF);

  uint32_t newblack = get_b(black);
  ASSERT(newblack == 0x00);

  uint32_t newwhite = get_b(white);
  ASSERT(newwhite == 0xFF);

  uint32_t newfunny = get_b(funny);
  ASSERT(newfunny == 0x69);

  uint32_t newtransparent = get_b(transparent);
  ASSERT(newtransparent == 0xEF);

  uint32_t newrandom1 = get_b(random1);
  ASSERT(newrandom1 == 0x25);

  uint32_t newrandom2 = get_b(random2);
  ASSERT(newrandom2 == 0xDC);
}

void test_get_a( TestObjs *objs ) {
  // Set up pixel data with known values.
  uint32_t red = 0xFF0000FF;
  uint32_t green = 0x00FF00FF;
  uint32_t blue = 0x0000FFFF;
  uint32_t black = 0x000000FF;
  uint32_t white = 0xFFFFFFFF;
  uint32_t funny = 0x69696969;
  uint32_t transparent = 0xABCDEF00;
  uint32_t random1 = 0x824525EB;
  uint32_t random2 = 0x0182DC12;


  // Test each color
  uint32_t newred = get_a(red);
  ASSERT(newred == 0xFF);

  uint32_t newgreen = get_a(green);
  ASSERT(newgreen == 0xFF);

  uint32_t newblue = get_a(blue);
  ASSERT(newblue == 0xFF);

  uint32_t newblack = get_a(black);
  ASSERT(newblack == 0xFF);

  uint32_t newwhite = get_a(white);
  ASSERT(newwhite == 0xFF);

  uint32_t newfunny = get_a(funny);
  ASSERT(newfunny == 0x69);

  uint32_t newtransparent = get_a(transparent);
  ASSERT(newtransparent == 0x00);

  uint32_t newrandom1 = get_a(random1);
  ASSERT(newrandom1 == 0xEB);

  uint32_t newrandom2 = get_a(random2);
  ASSERT(newrandom2 == 0x12);
}

void test_make_pixel( TestObjs *objs ) {
  // Set up pixel data with known values.
  uint32_t red = 0xFF0000FF;
  uint32_t green = 0x00FF00FF;
  uint32_t blue = 0x0000FFFF;
  uint32_t black = 0x000000FF;
  uint32_t white = 0xFFFFFFFF;
  uint32_t funny = 0x69696969;
  uint32_t transparent = 0xABCDEF00;
  uint32_t random1 = 0x824525EB;
  uint32_t random2 = 0x0182DC12;


  // Test each color
  uint32_t newred = make_pixel(0xFF, 0x00, 0x00, 0xFF);
  ASSERT(newred == red);

  uint32_t newgreen = make_pixel(0x00, 0xFF, 0x00, 0xFF);
  ASSERT(newgreen == green);

  uint32_t newblue = make_pixel(0x00, 0x00, 0xFF, 0xFF);
  ASSERT(newblue == blue);

  uint32_t newblack = make_pixel(0x00, 0x00, 0x00, 0xFF);
  ASSERT(newblack == black);

  uint32_t newwhite = make_pixel(0xFF, 0xFF, 0xFF, 0xFF);
  ASSERT(newwhite == white);

  uint32_t newfunny = make_pixel(0x69, 0x69, 0x69, 0x69);
  ASSERT(newfunny == funny);

  uint32_t newtransparent = make_pixel(0xAB, 0xCD, 0xEF, 0x00);
  ASSERT(newtransparent == transparent);

  uint32_t newrandom1 = make_pixel(0x82, 0x45, 0x25, 0xEB);
  ASSERT(newrandom1 == random1);

  uint32_t newrandom2 = make_pixel(0x01, 0x82, 0xDC, 0x12);
  ASSERT(newrandom2 == random2);
}

void test_to_grayscale( TestObjs *objs ) {
  // Set up pixel data with known values.
  uint32_t red = 0xFF0000FF;
  uint32_t green = 0x00FF00FF;
  uint32_t blue = 0x0000FFFF;
  uint32_t black = 0x000000FF;
  uint32_t white = 0xFFFFFFFF;
  uint32_t funny = 0x69696969;
  uint32_t transparent = 0xABCDEF00;
  uint32_t random1 = 0x824525EB;
  uint32_t random2 = 0x0182DC12;


  // Test each color
  uint32_t newred = to_grayscale(red);
  ASSERT(newred == 0x4E4E4EFF);

  uint32_t newgreen = to_grayscale(green);
  ASSERT(newgreen == 0x7F7F7FFF);

  uint32_t newblue = to_grayscale(blue);
  ASSERT(newblue == 0x303030FF);

  uint32_t newblack = to_grayscale(black);
  ASSERT(newblack == 0x000000FF);

  uint32_t newwhite = to_grayscale(white);
  ASSERT(newwhite == 0xFFFFFFFF);

  uint32_t newfunny = to_grayscale(funny);
  ASSERT(newfunny == 0x69696969);

  uint32_t newtransparent = to_grayscale(transparent);
  ASSERT(newtransparent == 0xC9C9C900);

  uint32_t newrandom1 = to_grayscale(random1);
  ASSERT(newrandom1 == 0x515151EB);

  uint32_t newrandom2 = to_grayscale(random2);
  ASSERT(newrandom2 == 0x6B6B6B12);
}

void test_compute_index( TestObjs *objs ) {
  // Create an image with 64 pixels
  struct Image img;
  img.width = 8;
  img.height = 8;
  img.data = malloc(sizeof(uint32_t) * img.width * img.height);
  if (img.data == NULL) {
      fprintf(stderr, "Memory allocation failed.\n");
      exit(EXIT_FAILURE);
  }
  
  // Set up pixel data with random values.
  for(int i = 0; i < (img.width * img.height); i++){
    img.data[i] = randomRGBA();
  }

  int index = 0;
  // Test each pixel
  for(int row = 0; row < img.height; row++){
    for(int col = 0; col < img.width; col++){
      ASSERT(img.data[compute_index(&img, col, row)] == img.data[index]);
      index++;
    }
  }

  // Clean up allocated memory.
  free(img.data);
}

void test_rgb_basic( TestObjs *objs ) {
  struct Picture smiley_rgb_pic = {
    TEST_COLORS,
    32, // width
    20, // height
    "    mrrrggbc        51112234    "
    "   c        b      4        3   "
    "  r   r  b   c    1   1  3   4  "
    " b            b  3            3 "
    " b            r  3            1 "
    " g   b    c   r  2   3    4   1 "
    "  c   ggrb   b    4   2213   3  "
    "   m        c      5        4   "
    "    gggrrbmc        22211354    "
    "                                "
    "    EAAABBCD        TPPPQQRS    "
    "   D        C      S        R   "
    "  A   A  C   D    P   P  R   S  "
    " C            C  R            R "
    " C            A  R            P "
    " B   C    D   A  Q   R    S   P "
    "  D   BBAC   C    S   QQPR   R  "
    "   E        D      T        S   "
    "    BBBAACED        QQQPPRTS    "
    "                                "
  };

  struct Image *smiley_rgb_expected = picture_to_img( &smiley_rgb_pic );

  imgproc_rgb( objs->smiley, objs->smiley_rgb_out );

  ASSERT( images_equal( smiley_rgb_expected, objs->smiley_rgb_out ) );

  destroy_img( smiley_rgb_expected );
}

void test_grayscale_basic( TestObjs *objs ) {
  struct Picture smiley_grayscale_pic = {
    TEST_COLORS_GRAYSCALE,
    16, // width
    10, // height
    "    mrrrggbc    "
    "   c        b   "
    "  r   r  b   c  "
    " b            b "
    " b            r "
    " g   b    c   r "
    "  c   ggrb   b  "
    "   m        c   "
    "    gggrrbmc    "
    "                "
  };

  struct Image *smiley_grayscale_expected = picture_to_img( &smiley_grayscale_pic );

  imgproc_grayscale( objs->smiley, objs->smiley_out );

  ASSERT( images_equal( smiley_grayscale_expected, objs->smiley_out ) );

  destroy_img( smiley_grayscale_expected );
}

void test_fade_basic( TestObjs *objs ) {
  struct Picture smiley_fade_expected_pic = {
    {
      { ' ', 0x000000ff },
      { 'a', 0x000032ff },
      { 'b', 0x000039ff },
      { 'c', 0x000044ff },
      { 'd', 0x00005dff },
      { 'e', 0x000095ff },
      { 'f', 0x0000a0ff },
      { 'g', 0x0000dbff },
      { 'h', 0x0000f0ff },
      { 'i', 0x003737ff },
      { 'j', 0x003b00ff },
      { 'k', 0x006363ff },
      { 'l', 0x006b6bff },
      { 'm', 0x007a00ff },
      { 'n', 0x008c00ff },
      { 'o', 0x008c8cff },
      { 'p', 0x009900ff },
      { 'q', 0x00a0a0ff },
      { 'r', 0x00e500ff },
      { 's', 0x00efefff },
      { 't', 0x00f000ff },
      { 'u', 0x470000ff },
      { 'v', 0x6b0000ff },
      { 'w', 0x6f0000ff },
      { 'x', 0x820082ff },
      { 'y', 0x990000ff },
      { 'z', 0x990099ff },
      { 'A', 0xa00000ff },
      { 'B', 0xa30000ff },
      { 'C', 0xf40000ff },
    },
    16, 10, // width and height
    "                "
    "   i        c   "
    "  u   y  f   k  "
    " a            d "
    " b            v "
    " j   g    s   w "
    "  l   rtCh   e  "
    "   x        q   "
    "    mnpABfzo    "
    "                "
  };

  struct Image *smiley_fade_expected = picture_to_img( &smiley_fade_expected_pic );

  imgproc_fade( objs->smiley, objs->smiley_out );

  ASSERT( images_equal( smiley_fade_expected, objs->smiley_out ) );

  destroy_img( smiley_fade_expected );
}

void test_kaleidoscope_basic( TestObjs *objs ) {
  struct Picture sq_test_kaleidoscope_expected_pic = {
    TEST_COLORS,
    12, // width
    12, // height
    "rrrrrrrrrrrr"
    "rggggggggggr"
    "rgbbbbbbbbgr"
    "rgbmmmmmmbgr"
    "rgbmccccmbgr"
    "rgbmcrrcmbgr"
    "rgbmcrrcmbgr"
    "rgbmccccmbgr"
    "rgbmmmmmmbgr"
    "rgbbbbbbbbgr"
    "rggggggggggr"
    "rrrrrrrrrrrr"
  };

  struct Image *sq_test_kaleidoscope_expected = picture_to_img( &sq_test_kaleidoscope_expected_pic );

  imgproc_kaleidoscope( objs->sq_test, objs->sq_test_out );

  ASSERT( images_equal( sq_test_kaleidoscope_expected, objs->sq_test_out ) );

  destroy_img( sq_test_kaleidoscope_expected );
}

