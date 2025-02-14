// C implementations of image processing functions
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "imgproc.h"
#include "image.h"

// TODO: define your helper functions here

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
    //int64_t temp = floor((2000000000*x)/(1000000*max))-1000;
    int64_t temp = (2000000000*x)/(1000000*max)-1000;
    int64_t g = 1000000 - (temp * temp);
    return g;
}

uint32_t to_fade( int64_t gradrow, int64_t gradcol, uint32_t pixel){
    //int64_t gradred = floor(gradcol * gradrow * get_r(pixel)/1000000000000);
    //int64_t gradgreen = floor(gradcol * gradrow * get_g(pixel)/1000000000000);
    //int64_t gradblue = floor(gradcol * gradrow * get_b(pixel)/1000000000000);
    int64_t gradred = gradcol * gradrow * get_r(pixel)/1000000000000;
    int64_t gradgreen = gradcol * gradrow * get_g(pixel)/1000000000000;
    int64_t gradblue = gradcol * gradrow * get_b(pixel)/1000000000000;
    uint32_t newpixel = make_pixel(gradred, gradgreen, gradblue, get_a(pixel));
    return newpixel;
}

int32_t compute_index( int32_t width, int32_t col, int32_t row ){
    return row*width + col;
};



// Convert input pixels to grayscale.
// This transformation always succeeds.
//
// Parameters:
//   input_img  - pointer to the input Image
//   output_img - pointer to the output Image (in which the transformed
//                pixels should be stored)
void imgproc_grayscale( struct Image *input_img, struct Image *output_img ) {
    // for (int i = 0; i < (input_img->height*input_img->width-1); i++){
    //   output_img->data[i] = to_grayscale(input_img->data[i]);
    // }

    for (int32_t row = 0; row < input_img->height; row++) {
        for (int32_t col = 0; col < input_img->width; col++) {
            // Get the current pixel from the input image
            uint32_t pixel = input_img->data[compute_index(input_img->width, col, row)];
            output_img->data[compute_index(output_img->width, col, row)] = to_grayscale(pixel);

        }
    }
}

// Render an output image containing 4 replicas of the original image,
// refered to as A, B, C, and D in the following diagram:
//
//   +---+---+
//   | A | B |
//   +---+---+
//   | C | D |
//   +---+---+
//
// The width and height of the output image are (respectively) twice
// the width and height of the input image.
//
// A is an exact copy of the original input image. B has only the
// red color component values of the input image, C has only the
// green color component values, and D has only the blue color component
// values.
//
// Each of the copies (A-D) should use the same alpha values as the
// original image.
//
// This transformation always succeeds.
//
// Parameters:
//   input_img - pointer to the input Image
//   output_img - pointer to the output Image (which will have
//                width and height twice the width/height of the
//                input image)
void imgproc_rgb( struct Image *input_img, struct Image *output_img ) {
    //Quadrant A
    for (int32_t row = 0; row < input_img->height; row++) {
        for (int32_t col = 0; col < input_img->width; col++) {
            // Get the current pixel from the input image
            uint32_t pixel = input_img->data[compute_index(input_img->width, col, row)];
            //make the new pixel of the output the same for quadrant A
            output_img->data[compute_index(output_img->width, col, row)] = make_pixel(get_r(pixel), get_g(pixel), get_b(pixel), get_a(pixel)); 
        }
    }
    //Quadrant B
    for (int32_t row = 0; row < input_img->height; row++) {
        for (int32_t col = 0; col < input_img->width; col++) {
            // Get the current pixel from the input image
            uint32_t pixel = input_img->data[compute_index(input_img->width, col, row)];
            //make the new pixel of the output the same for quadrant A
            output_img->data[compute_index(output_img->width, col + input_img->width, row)] = make_pixel(get_r(pixel), 0, 0, get_a(pixel)); 
        }
      }
    //Quadrant C
    for (int32_t row = 0; row < input_img->height; row++) {
        for (int32_t col = 0; col < input_img->width; col++) {
            // Get the current pixel from the input image
            uint32_t pixel = input_img->data[compute_index(input_img->width, col, row)];
            //make the new pixel of the output the same for quadrant A
            output_img->data[compute_index(output_img->width, col, row + input_img->height)] = make_pixel(0, get_g(pixel), 0, get_a(pixel)); 
        }
    }
    //Quadrant D
    for (int32_t row = 0; row < input_img->height; row++) {
        for (int32_t col = 0; col < input_img->width; col++) {
            // Get the current pixel from the input image
            uint32_t pixel = input_img->data[compute_index(input_img->width, col, row)];
            //make the new pixel of the output the same for quadrant A
            output_img->data[compute_index(output_img->width, col + input_img->width, row + input_img->height)] = make_pixel(0, 0, get_b(pixel), get_a(pixel)); 
        }
    }

}

// Render a "faded" version of the input image.
//
// See the assignment description for an explanation of how this transformation
// should work.
//
// This transformation always succeeds.
//
// Parameters:
//   input_img - pointer to the input Image
//   output_img - pointer to the output Image
void imgproc_fade( struct Image *input_img, struct Image *output_img ) {
    for (int32_t row = 0; row < input_img->height; row++) {
        for (int32_t col = 0; col < input_img->width; col++) {
            // Get the current pixel from the input image
            uint32_t pixel = input_img->data[compute_index(input_img->width, col, row)];
            // Compute the two gradient values
            int64_t gradr = gradient(row, input_img->height);
            int64_t gradc = gradient(col, input_img->width);
            //make the new pixel of the output
            output_img->data[compute_index(output_img->width, col, row)] = to_fade(gradr, gradc, pixel); 
        }
    }
}

// Render a "kaleidoscope" transformation of input_img in output_img.
// The input_img must be square, i.e., the width and height must be
// the same. Assume that the input image is divided into 8 "wedges"
// like this:
//
//    +----+----+
//    |\ A |   /|
//    | \  |  / |
//    |  \ | /  |
//    | B \|/   |
//    +----+----+
//    |   /|\   |
//    |  / | \  |
//    | /  |  \ |
//    |/   |   \|
//    +----+----+
//
// The wedge labeled "A" is the one that will be replicated 8 times
// in the output image. In the output image,
//
//    Wedge A will have exactly the same pixels as wedge A in
//    the input image, in the same positions.
//
//    Wedge B will have exactly the same pixels as wedge A, but the
//    column and row are transposed. (I.e., it is a reflection across
//    the diagonal.)
//
//    All other wedges are mirrored versions of A such that the overall
//    image is symmetrical across all four lines dividing the image
//    (vertical split, horizontal split, and the two diagonal splits.)
//
// Parameters:
//   input_img  - pointer to the input Image
//   output_img - pointer to the output Image (in which the transformed
//                pixels should be stored)
//
// Returns:
//   1 if successful, 0 if the transformation fails because the
//   width and height of input_img are not the same.
int imgproc_kaleidoscope( struct Image *input_img, struct Image *output_img ) {
    // Ensure input image is square
    if (input_img->width != input_img->height) {
        return 0;
    }

    int size = input_img->width; 
    int odd = 0;
    if (size % 2 == 1) {
        size++; 
        odd = 1;
    }
    if (odd) {
        // Allocate memory for newdata which is the even value
        uint32_t *newdata = malloc(sizeof(uint32_t) * size * size);
        if (newdata == NULL) {
            fprintf(stderr, "Memory allocation failed for newdata.\n");
            exit(EXIT_FAILURE);
        }

        // Initialize the output image
        img_init(output_img, input_img->width, input_img->height);
        if (output_img->data == NULL) {
            fprintf(stderr, "Memory allocation failed for output_img->data.\n");
            free(newdata); 
            exit(EXIT_FAILURE);
        }

        for (int row = 0; row < size / 2; row++) {
            for (int col = row; col < size / 2; col++) {
                uint32_t pixel = input_img->data[compute_index(input_img->width, col, row)];


                //A
                newdata[row * size + col] = pixel;
                //B
                newdata[col * size + row] = pixel;

                //Mirror A across y axis
                newdata[row * size + size - col - 1] = pixel;
                //Mirror B across y axis
                newdata[col * size + size - row - 1 ] = pixel;


                //Mirror A across x axis Call this C
                newdata[(size - row - 1)  * size  + col] = pixel;
                //Mirror B across x axis Call this D
                newdata[(size - col - 1 )  * size  + row] = pixel;

                //Mirror C across y axis
                newdata[(size - row - 1)  * size  + size - col - 1] = pixel;
                //Mirror D across y axis
                newdata[(size - col - 1)  * size  + size - row - 1] = pixel;

            }
        }

        // Copy data from newdata to output_img (original n x n dimensions)
        for (int row = 0; row < input_img->height; row++) {
            for (int col = 0; col < input_img->width; col++) {
                output_img->data[compute_index(output_img->width, col, row)] = newdata[row * size + col];
            }
        }

        // Free the allocated memory for newdata
        free(newdata);
        return 1;

    }

    else {
        // Initialize the output image
        if (output_img->data == NULL) {
            fprintf(stderr, "Memory allocation failed.\n");
            exit(EXIT_FAILURE);
        }
        
        int fakesize = input_img->width;
        if (size % 2 == 1) {
            fakesize++;
            // size++;
        }

        for (int row = 0; row < fakesize/2; row++) {
            for (int col = row; col < fakesize/2; col++) {
                uint32_t pixel = input_img->data[row * size + col];
                //A
                output_img->data[row * size + col] = pixel;
                //B
                output_img->data[col * size + row] = pixel;

                //Mirror A across y axis
                output_img->data[row * size + size - col - 1] = pixel;
                //Mirror B across y axis
                output_img->data[col * size + size - row - 1 ] = pixel;


                //Mirror A across x axis Call this C
                output_img->data[(size - row - 1)  * size  + col] = pixel;
                //Mirror B across x axis Call this D
                output_img->data[(size - col - 1 )  * size  + row] = pixel;

                //Mirror C across y axis
                output_img->data[(size - row - 1)  * size  + size - col - 1] = pixel;
                //Mirror D across y axis
                output_img->data[(size - col - 1)  * size  + size - row - 1] = pixel;

            }
        }

    }

    return 1;

}
