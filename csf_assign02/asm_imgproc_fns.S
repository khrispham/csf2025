/*
 * x86-64 assembly language implementations of functions
 */

	.section .text

/* Offsets of struct Image fields */
#define IMAGE_WIDTH_OFFSET   0
#define IMAGE_HEIGHT_OFFSET  4
#define IMAGE_DATA_OFFSET    8

/*
 * TODO: define your helper functions here.
 * Don't forget to use the .globl directive to make
 * them visible to the test code.
 */

/*
 * uint32_t get_r(uint32_t pixel);
 *
 * Get bits 24-31th of the pixel which is the red component
 *
 * Parameter: %rdi - uint32_t pixel
 *
 * Returns: the 24-31th bits of uint32_t pixel (red value)
 */
.globl get_r
get_r:

	subq $8, %rsp //align stack pointer

	movq %rdi, %rax // store rdi's value to rax
	shrq $24, %rax  // shift rax's value to the right by 24 bits
	addq $8, %rsp //restore stack pointer
	ret //returns rax value

/*
 * uint32_t get_g(uint32_t pixel);
 *
 * Get bits 16-23th of the pixel which is the green component
 *
 * Parameter: %rdi - uint32_t pixel
 *
 * Returns: the 16-23th bits of uint32_t pixel (green value)
 */

.globl get_g
get_g:
	subq $8, %rsp //align stack pointer
	movq %rdi, %rax // store rdi's value to rax
	shrq $16, %rax  // shift rax's value to the right by 16 bits
	movq $255, %rsi //store 255 to rsi (11111111)
	shlq $16, %rsi //bitwise shift left rsi 16 times to create that mask we made in C language
	andq %rsi, %rax //perform a bitwise & with rsi and rax and store it into rax
	shrq $16, %rax //bitwise shift right rax 16 times to get only the g value
	addq $8, %rsp //restore stack pointer
	ret //returns rax value

/*
 * uint32_t get_b(uint32_t pixel);
 *
 * Get bits 8-15th of the pixel which is the blue component
 *
 * Parameter: %rdi - uint32_t pixel
 *
 * Returns: the 8-15th bits of uint32_t pixel (blue value)
 */

.globl get_b
get_b:
	subq $8, %rsp //align stack pointer
	movq %rdi, %rax // store rdi's value to rax
	shrq $8, %rax  // shift rax's value to the right by 8 bits
	movq $255, %rsi //store 255 to rsi (11111111)
	shlq $8, %rsi //bitwise shift left rsi 8 times to create that mask we made in C language
	andq %rsi, %rax //perform a bitwise & with rsi and rax and store it into rax
	shrq $8, %rax //bitwise shift right rax 8 times to get only the b value
	addq $8, %rsp //restore stack pointer
	ret	//returns rax value

/*
 * uint32_t get_a(uint32_t pixel);
 *
 * Get bits 0-7th of the pixel which is the alpha component
 *
 * Parameter: %rdi - uint32_t pixel
 *
 * Returns: the 0-7th bits of uint32_t pixel (alpha value)
 */
.globl get_a
get_a:


	subq $8, %rsp //align stack pointer
	movq %rdi, %rax // store rdi's value to rax
	movq $255, %rsi //store 255 to rsi (11111111)
	andq %rsi, %rax //perform a bitwise & with rsi and rax and store it into rax
	addq $8, %rsp //restore stack pointer
	ret	//returns rax value


/*
 * uint32_t make_pixel(uint32_t r, uint32_t g, uint32_t b, uint32_t a);
 *
 * construct a uint32 pixel given rgba values
 *
 * Parameter:
 *  %rdi - uint32_t red
 *  %rsi - uint32_t green
 *  %rdx - uint32_t blue
 *  %rcx - uint32_t alpha
 * Registers:
 *  %rdi - red component
 *  %rsi - green component
 *  %rdx - blue component
 *  %rcx - alpha component
 *
 * Returns: pixel
 */
.globl make_pixel

make_pixel:
	subq $8, %rsp //allign the stack

	//handle bits to their respective positions in the 32 bits
	shlq $24, %rdi //shift red left 24
	shlq $16, %rsi	//shift green left 16
	shlq $8, %rdx	//shift blue left 8
	movq %rcx, %rax // move alpha to output
	//perform bitwise combination of the 4 rgba
	orq %rdx, %rax  //bitwise or with rax blue
	orq %rax, %rsi //bitwise or with rsi green
	orq %rax, %rdi //bitwise or with red

	addq $8, %rsp //restore stack pointer

	ret



/*
 * Implementations of API functions
 */

/*
 * Render an output image containing 4 copies of the original image,
 * refered to as A, B, C, and D in the following diagram:
 *
 *   +---+---+
 *   | A | B |
 *   +---+---+
 *   | C | D |
 *   +---+---+
 *
 * The width and height of the output image are (respectively) twice
 * the width and height of the input image.
 *
 * A is an exact copy of the original input image. B has only the
 * red color component values of the input image, C has only the
 * green color component values, and D has only the blue color component
 * values.
 *
 * Each of the copies (A-D) should use the same alpha values as the
 * original image.
 *
 * This transformation always succeeds.
 *
 * Parameters:
 *   %rdi - pointer to the input Image
 *   %rsi - pointer to the output Image (which will have
 *          width and height twice the width/height of the
 *          input image)
 */
	.globl imgproc_rgb
imgproc_rgb:
	/* TODO: implement */
	ret

/*
 * void imgproc_grayscale( struct Image *input_img, struct Image *output_img );
 *
 * Transform image by converting each pixel to grayscale.
 * This transformation always succeeds.
 *
 * Parameters:
 *   %rdi - pointer to original struct Image
 *   %rsi - pointer to output struct Image
 */
	.globl imgproc_grayscale
imgproc_grayscale:
	/* TODO: implement */
	ret

/*
 * Render a "faded" version of the input image.
 *
 * See the assignment description for an explanation of how this transformation
 * should work.
 *
 * This transformation always succeeds.
 *
 * Parameters:
 *   %rdi - pointer to the input Image
 *   %rsi - pointer to the output Image
 */
	.globl imgproc_fade
imgproc_fade:
	/* TODO: implement */
	ret

/*
 * Render a "kaleidoscope" transformation of input_img in output_img.
 * The input_img must be square, i.e., the width and height must be
 * the same. Assume that the input image is divided into 8 "wedges"
 * like this:
 *
 *    +----+----+
 *    |\ A |   /|
 *    | \  |  / |
 *    |  \ | /  |
 *    | B \|/   |
 *    +----+----+
 *    |   /|\   |
 *    |  / | \  |
 *    | /  |  \ |
 *    |/   |   \|
 *    +----+----+
 *
 * The wedge labeled "A" is the one that will be replicated 8 times
 * in the output image. In the output image,
 *
 *    Wedge A will have exactly the same pixels as wedge A in
 *    the input image, in the same positions.
 *
 *    Wedge B will have exactly the same pixels as wedge A, but the
 *    column and row are transposed. (I.e., it is a reflection across
 *    the diagonal.)
 *
 *    All other wedges are mirrored versions of A such that the overall
 *    image is symmetrical across all four lines dividing the image
 *    (vertical split, horizontal split, and the two diagonal splits.)
 *
 * Parameters:
 *   input_img  - pointer to the input Image
 *   output_img - pointer to the output Image (in which the transformed
 *                pixels should be stored)
 *
 * Returns:
 *   1 if successful, 0 if the transformation fails because the
 *   width and height of input_img are not the same.
 */
	.globl imgproc_kaleidoscope
imgproc_kaleidoscope:
	/* TODO: implement */
	ret

	/* This avoids linker warning about executable stack */
.section .note.GNU-stack,"",@progbits

/*
vim:ft=gas:
*/
