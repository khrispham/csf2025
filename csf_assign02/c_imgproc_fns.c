// C implementations of image processing functions
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "imgproc.h"
#include "image.h"

// TODO: define your helper functions here

// Convert input pixels to grayscale.
// This transformation always succeeds.
//
// Parameters:
//   input_img  - pointer to the input Image
//   output_img - pointer to the output Image (in which the transformed
//                pixels should be stored)
void imgproc_grayscale( struct Image *input_img, struct Image *output_img ) {
    //initialize output image
    img_init(output_img, input_img->width, input_img->height);
    if (output_img->data == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    // for (int i = 0; i < (input_img->height*input_img->width-1); i++){
    //   output_img->data[i] = to_grayscale(input_img->data[i]);
    // }

    for (int32_t row = 0; row < input_img->height; row++) {
        for (int32_t col = 0; col < input_img->width; col++) {
            // Get the current pixel from the input image
            uint32_t pixel = input_img->data[compute_index(input_img, col, row)];
            output_img->data[compute_index(output_img, col, row)] = to_grayscale(pixel);

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

    // Initialize the output image
    img_init(output_img, 2 * input_img->width, 2 * input_img->height);
    if (output_img->data == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    //Quadrant A
    for (int32_t row = 0; row < input_img->height; row++) {
        for (int32_t col = 0; col < input_img->width; col++) {
            // Get the current pixel from the input image
            uint32_t pixel = input_img->data[compute_index(input_img, col, row)];
            //make the new pixel of the output the same for quadrant A
            output_img->data[compute_index(output_img, col, row)] = make_pixel(get_r(pixel), get_g(pixel), get_b(pixel), get_a(pixel)); 
        }
    }
    //Quadrant B
    for (int32_t row = 0; row < input_img->height; row++) {
        for (int32_t col = 0; col < input_img->width; col++) {
            // Get the current pixel from the input image
            uint32_t pixel = input_img->data[compute_index(input_img, col, row)];
            //make the new pixel of the output the same for quadrant A
            output_img->data[compute_index(output_img, col + input_img->width, row)] = make_pixel(get_r(pixel), 0, 0, get_a(pixel)); 
        }
      }
    //Quadrant C
    for (int32_t row = 0; row < input_img->height; row++) {
        for (int32_t col = 0; col < input_img->width; col++) {
            // Get the current pixel from the input image
            uint32_t pixel = input_img->data[compute_index(input_img, col, row)];
            //make the new pixel of the output the same for quadrant A
            output_img->data[compute_index(output_img, col, row + input_img->height)] = make_pixel(0, get_g(pixel), 0, get_a(pixel)); 
        }
    }
    //Quadrant D
    for (int32_t row = 0; row < input_img->height; row++) {
        for (int32_t col = 0; col < input_img->width; col++) {
            // Get the current pixel from the input image
            uint32_t pixel = input_img->data[compute_index(input_img, col, row)];
            //make the new pixel of the output the same for quadrant A
            output_img->data[compute_index(output_img, col + input_img->width, row + input_img->height)] = make_pixel(0, 0, get_b(pixel), get_a(pixel)); 
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
    // Initialize the output image
    img_init(output_img, input_img->width, input_img->height);
    if (output_img->data == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    for (int32_t row = 0; row < input_img->height; row++) {
        for (int32_t col = 0; col < input_img->width; col++) {
            // Get the current pixel from the input image
            uint32_t pixel = input_img->data[compute_index(input_img, col, row)];
            // Compute the two gradient values
            int64_t gradr = gradient(row, input_img->height);
            int64_t gradc = gradient(col, input_img->width);
            // Compute each color component
            int64_t gradred = floor(gradc*gradr*get_r(pixel)/1000000000000);
            int64_t gradgreen = floor(gradc*gradr*get_g(pixel)/1000000000000);
            int64_t gradblue = floor(gradc*gradr*get_b(pixel)/1000000000000);
            //make the new pixel of the output
            output_img->data[compute_index(output_img, col, row)] = make_pixel(gradred, gradgreen, gradblue, get_a(pixel)); 
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
    if (size % 2 == 1) {
        size++;
    }

    // Initialize the output image
    img_init(output_img, size, size);
    if (output_img->data == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    
    for (int row = 0; row < size/2; row++) {
        for (int col = row; col < size/2; col++) {
            uint32_t pixel = input_img->data[compute_index(input_img, col, row)];
            //A
            output_img->data[compute_index(output_img, row, col)] = pixel;
            //B
            output_img->data[compute_index(output_img, col, row)] = pixel;

            //top right quadrant
            output_img->data[compute_index(output_img, size-1-row, col)] = pixel;
            output_img->data[compute_index(output_img, size-1- col, row)] = pixel; 

            //bottom left quadrant
            output_img->data[compute_index(output_img,row, size - 1 - col)]  = pixel;
            output_img->data[compute_index(output_img,col, size - 1 - row)] = pixel;

            //bottom right quadrant
            output_img->data[compute_index(output_img,size - 1 -row, size - 1 - col)]  = pixel;
            output_img->data[compute_index(output_img,size - 1 -col, size - 1 - row)]  = pixel;

        }
    }
    return 1;

}
