#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pnglite.h"
#include "image.h"

int png_init_called;

////////////////////////////////////////////////////////////////////////
// Helper functions
////////////////////////////////////////////////////////////////////////

uint32_t get_r( uint32_t pixel ){
  //mask with 1s in bits 24-31
  uint32_t mask = ((1 << 8) - 1) << 24;
  uint32_t rval = (pixel & mask) >> 24;
  return rval;
};
uint32_t get_g( uint32_t pixel ){
  //mask with 1s in bits 16-23
  uint32_t mask = ((1 << 8) - 1) << 16;
  uint32_t gval = (pixel & mask) >> 16;
  return gval;
};
uint32_t get_b( uint32_t pixel ){
  //mask with 1s in bits 8-15
  uint32_t mask = ((1 << 8) - 1) << 8;
  uint32_t bval = (pixel & mask) >> 8;
  return bval;
};
uint32_t get_a( uint32_t pixel ){
  //mask with 1s in bits 0-7
  uint32_t mask = (1 << 8) - 1;
  uint32_t aval = pixel & mask;
  return aval;
};

uint32_t make_pixel( uint32_t r, uint32_t g, uint32_t b, uint32_t a ){
  return (r << 24) | (g << 16) | (b << 8) | (a);
};

uint32_t to_grayscale( uint32_t pixel ){
  uint32_t r = get_r(pixel);
  uint32_t g = get_g(pixel);
  uint32_t b = get_b(pixel);
  uint32_t y = floor((79*r+128*g+49*b)/256);
  uint32_t result = make_pixel(y, y, y, get_a(pixel));
  return result;
};


int64_t gradient( int64_t x, int64_t max ){
  int64_t temp = floor((2000000000*x)/(1000000*max))-1000;
  int64_t g = 1000000 - (temp * temp);
  return g;
}

int32_t compute_index( struct Image *img, int32_t col, int32_t row ){
  return row*img->width + col;
};

////////////////////////////////////////////////////////////////////////
// Image functions
////////////////////////////////////////////////////////////////////////

int is_little_endian(void) {
  int32_t x = 1;
  return *((char *) &x) == 1;
}

uint32_t byteswap(uint32_t val) {
  const uint8_t *p = (const uint8_t *) &val;
  uint32_t result = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
  return result;
}

int img_init(struct Image *img, int32_t width, int32_t height) {
  int num_pixels = width * height;

  uint32_t *pixel_data = (uint32_t *) malloc(num_pixels * sizeof(uint32_t));
  if (pixel_data == NULL) {
    return IMG_ERR_MALLOC_FAILED;
  }

  // initialize every pixel to opaque black
  for (int32_t i = 0; i < num_pixels; i++) {
    pixel_data[i] = 0x000000FFU;
  }

  // success
  img->width = width;
  img->height = height;
  img->data = pixel_data;
  return IMG_SUCCESS;
}

int img_read(const char *filename, struct Image *img) {
  if (!png_init_called) {
    png_init(0, 0);
    png_init_called = 1;
  }

  png_t png;

  if (png_open_file_read(&png, filename) != PNG_NO_ERROR) {
    return IMG_ERR_COULD_NOT_OPEN;
  }

  // only allow truecolor 8bpp images
  if (!(png.color_type == PNG_TRUECOLOR && png.bpp == 3) &&
      !(png.color_type == PNG_TRUECOLOR_ALPHA && png.bpp == 4)) {
    png_close_file(&png);
    return IMG_ERR_NOT_TRUECOLOR;
  }
  
  int num_pixels = png.width * png.height;

  // allocate buffer for pixel data in truecolor RGBA format
  uint32_t *pixel_data = (uint32_t *) malloc(num_pixels * sizeof(uint32_t));

  if (png.color_type == PNG_TRUECOLOR) {
    // PNG pixel data is in RGB form, expand it to add the alpha channel

    unsigned char *pixel_data_raw = (unsigned char *) malloc(num_pixels * 3);
    if (png_get_data(&png, pixel_data_raw) != PNG_NO_ERROR) {
      png_close_file(&png);
      free(pixel_data);
      return IMG_ERR_MALLOC_FAILED;
    }

    for (int i = 0; i < num_pixels; i++) {
      unsigned char r = pixel_data_raw[i*3 + 0];
      unsigned char g = pixel_data_raw[i*3 + 1];
      unsigned char b = pixel_data_raw[i*3 + 2];
      unsigned char a = 255;

      pixel_data[i] = (r << 24) | (g << 16) | (b << 8) | a;
    }

    free(pixel_data_raw);
  } else {
    // PNG pixel data is already in the correct format,
    // except that the RGBA data is in big-endian form, so we
    // need to byteswap if on a little endian system
    if (png_get_data(&png, (unsigned char *) pixel_data) != PNG_NO_ERROR) {
      png_close_file(&png);
      free(pixel_data);
      return IMG_ERR_MALLOC_FAILED;
    }

    if (is_little_endian()) {
      for (int i = 0; i < num_pixels; i++) {
        pixel_data[i] = byteswap(pixel_data[i]);
      }
    }
  }

  // communicate pixel data and image dimensions to caller
  img->data = pixel_data;
  img->width = png.width;
  img->height = png.height;

  png_close_file(&png);

  return IMG_SUCCESS;
}

int img_write(const char *filename, struct Image *img) {
  if (!png_init_called) {
    png_init(0, 0);
    png_init_called = 1;
  }

  png_t png;

  if (png_open_file_write(&png, filename) != PNG_NO_ERROR) {
    return IMG_ERR_COULD_NOT_OPEN;
  }

  // if this is a little endian system, we need to byteswap
  // every uint32_t so that it can be written in big-endian order
  // (which is what PNG requires)

  uint32_t *data_to_write = img->data;
  int need_byteswap = is_little_endian();

  if (need_byteswap) {
    data_to_write = (uint32_t *) malloc(img->width * img->height * sizeof(uint32_t));
    if (data_to_write == NULL) {
      png_close_file(&png);
      return IMG_ERR_MALLOC_FAILED;
    }

    int32_t num_pixels = img->width * img->height;
    for (int32_t i = 0; i < num_pixels; i++) {
      data_to_write[i] = byteswap(img->data[i]);
    }
  }

  int rc = png_set_data(&png, img->width, img->height, 8, PNG_TRUECOLOR_ALPHA, (unsigned char *) data_to_write);
  int success = (rc == PNG_NO_ERROR);

  png_close_file(&png);
  if (need_byteswap) {
    free(data_to_write);
  }

  return success ? IMG_SUCCESS : IMG_ERR_COULD_NOT_WRITE;
}

void img_cleanup( struct Image *img ) {
  // The data array is the only dynamically-allocated
  // part of the representation of a struct Image
  free( img->data );
}
