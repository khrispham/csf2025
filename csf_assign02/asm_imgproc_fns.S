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

	movq %rdi, %rax // store rdi's value to rax
	shrq $24, %rax  // shift rax's value to the right by 24 bits

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
	movq %rdi, %rax   // Copy the pixel value to %rax
    shrq $16, %rax    // Shift right by 16 bits 
    andq $255, %rax  // Apply a mask (0xFF) to isolate the green component

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

    movq %rdi, %rax    // Store rdi's value to rax
    shrq $8, %rax      // Shift rax right by 8 bits
    andq $255, %rax    // Mask the lower 8 bits (0xFF)
    ret                // Return the result in rax

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


	movq %rdi, %rax // store rdi's value to rax
	movq $255, %rsi //store 255 to rsi (11111111)
	andq %rsi, %rax //perform a bitwise & with rsi and rax and store it into rax
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

	//handle bits to their respective positions in the 32 bits
	shlq $24, %rdi //shift red left 24
	shlq $16, %rsi	//shift green left 16
	shlq $8, %rdx	//shift blue left 8
	movq %rcx, %rax // move alpha to output
	//perform bitwise combination of the 4 rgba
	orq %rdx, %rax  //bitwise or with rax blue
	orq %rsi, %rax //bitwise or with rsi green
	orq %rdi, %rax //bitwise or with red

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
 *
 * Register use:
 *   %r12d - i (pixel row index)
 *   %r13d - j (pixel column index)
 *   %r14 - pointer to input Image
 *   %r15 - pointer to output Image
 *	 %r8 - array offset
 *   %r9 - data array address
 *   %r10 - width
 *   %r11 - height
 *
 * Stack memory:
 *   -4(%rbp)  - current pixel value
 *   -8(%rbp)  - pixel r component value (from get_r)
 *   -12(%rbp) - pixel g component value (from get_g)
 *   -16(%rbp) - pixel b component value (from get_b)
 *   -20(%rbp) - pixel a component value (from get_a)
 *   -24(%rbp) - computed pixel index
 */
	.globl imgproc_rgb
imgproc_rgb:
	// allgin stack
	pushq %rbp
	
	// Allocate enough space for local variables
	movq %rsp, %rbp
	subq $32, %rsp

	// Store callee-saved registers
	pushq %r10	
	pushq %r11
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15

	// Get width and height from input Image rdi
	movl (%rdi), %r10d //width stored in r10d
	movl 4(%rdi), %r11d //height stored in r11d

	// Store the struct pointers to registers r14 and r15
	movq %rdi, %r14
	movq %rsi, %r15

	// Initialize iterator i, j to 0
	movl $0, %r12d	
	movl $0, %r13d


	// Iterates through loop and checks if i >= height to stop iterating through loop
.RowLoop:
	// Compare if i >= height, jump to RowEnd function if lower
	cmpl %r11d, %r12d
	jae .RowEnd		// Exit the nested loop

	// Initialize iterator j=0 for the ColLoop
	movl $0, %r13d
.ColLoop:
	// Compare j < width to continue
	cmpl %r10d, %r13d
	jae .ColEnd

	// TODO: DO THE RGB TRANSFORMATION HERE

	movl %r10d, %r9d // Store width to r9d which gets the data array address

	imull %r12d, %r9d // Multiply width in r10 by i 

	addl %r13d, %r9d // Add j (col) to that value

	// r9d has the index to the current pixel we are at in data


	// Copy the original image pointer to %r8
	movq %r14, %r8

	// Get to the data pointer by offsetting the pointer by 8 (skips width and height)
	addq $8, %r8

	// Start at the beginning of the array of data
	movq (%r8), %r8

	// Access the array values and move it to register %edi (32-bit rdi) (so we can do our calls for get_r etc)
	movl (%r8, %r9, 4), %edi

	// Extract RGBA components
	call get_r
	movl %eax, -8(%rbp)   // Save red component from rax to rbp 
	call get_g
	movl %eax, -12(%rbp)  // Save green component from rax to rbp 
	call get_b
	movl %eax, -16(%rbp)  // Save blue component from rax to rbp 
	call get_a
	movl %eax, -20(%rbp)  // Save alpha component from rax to rbp 


	//TODO DO THE ACTUAL RGB HEREEE ISSUE HEREEEe

	//Just Quadrant A don't have to use any rgb yet
	movl (%r8, %r9, 4), %eax           // Load the original pixel
	movq 8(%r15), %r8  // Load the output image's data pointer into %r8
	subl %r13d, %r9d //subtract height
	shll $1, %r9d //multiply by 2
	addl %r13d, %r9d //add height to get dimensions on the outputimage
	movl %eax, (%r8, %r9, 4)  //put the pixel on the output image


	//Quadrant B  (RED)
	//allocate more variables cause i ran outa registers
	pushq %rdi
	pushq %rsi
	pushq %rdx
	pushq %rcx
	pushq %r9

	movq -8(%rbp), %rdi
	movq $0, %rsi
	movq $0, %rdx
	movq -20(%rbp), %rcx
	
	call make_pixel
	
	// Correctly calculate 2 * width * row
	movl %r12d, %r9d     // Load row index (i) into %ebx
	shll $1, %r9d        // Multiply row index by 2 (since output image is twice the size)
	imull %r10d, %r9d    // Multiply by the width of the input image (2 * width * row)
	//Correctly calculate 2 * width * row + col + width
	addl %r13d, %r9d     // Add column index (j) to get the column offset
	addl %r10d, %r9d     // Add width to get the correct quadrant (2 * width * row + col + width)

	// Store the pixel in the output image
	movq 8(%r15), %r8    // Load the output image's data pointer into %r8
	movl %eax, (%r8, %r9, 4)  // Store the pixel at the calculated position

	//restore callee variables
	popq %r9
	popq %rcx
	popq %rdx
	popq %rsi
	popq %rdi



	//Quadrant C  (GREEN)
	//allocate more variables cause i ran outa registers
	pushq %rdi
	pushq %rsi
	pushq %rdx
	pushq %rcx
	pushq %r9

	movq $0, %rdi
	movq -12(%rbp), %rsi
	movq $0, %rdx
	movq -20(%rbp), %rcx
	
	call make_pixel
	
	// Correctly calculate 2 * width * row
	movl %r12d, %r9d     // Load row index (i) into %ebx
	addl %r11d, %r9d	//add height to row
	shll $1, %r9d        // Multiply row + height index by 2 by shifting left bitwise
	imull %r10d, %r9d    // Multiply by the width of the input image (2 * width * row)
	//Correctly calculate 2 * width * row + col + width
	addl %r13d, %r9d     // Add column index (j) to get the column offset

	// Store the pixel in the output image
	movq 8(%r15), %r8    // Load the output image's data pointer into %r8
	movl %eax, (%r8, %r9, 4)  // Store the pixel at the calculated position

	//restore callee variables
	popq %r9
	popq %rcx
	popq %rdx
	popq %rsi
	popq %rdi




//Quadrant D (Blue)
	//allocate more variables cause i ran outa registers
	pushq %rdi
	pushq %rsi
	pushq %rdx
	pushq %rcx
	pushq %r9

	movq $0, %rdi
	movq $0, %rsi
	movq -16(%rbp), %rdx
	movq -20(%rbp), %rcx
	
	call make_pixel
	
	// Correctly calculate 2 * width * (row + height)
	movl %r12d, %r9d     // Load row index (i) into %ebx
	addl %r11d, %r9d	//add height to row
	shll $1, %r9d        // Multiply row + height index by 2 by shifting left bitwise
	imull %r10d, %r9d    // Multiply by the width of the input image (2 * width * row)
	//Correctly calculate 2 * width * row + col + width
	addl %r13d, %r9d     // Add column index (j) to get the column offset
	addl %r10d, %r9d     // Add width value to get the value

	// Store the pixel in the output image
	movq 8(%r15), %r8    // Load the output image's data pointer into %r8
	movl %eax, (%r8, %r9, 4)  // Store the pixel at the calculated position

	//restore callee variables
	popq %r9
	popq %rcx
	popq %rdx
	popq %rsi
	popq %rdi












	// Increment j counter by 1
	incl %r13d
	jmp  .ColLoop // Continue the j loop

.ColEnd:
	// Increment i counter by 1
	incl %r12d
	jmp  .RowLoop // Go back to the Row loop with i incremented by 1

.RowEnd:
	// Restore callee-saved registers
	popq %r15
	popq %r14
	popq %r13
	popq %r12
	popq %r11
	popq %r10

	addq $32, %rsp
	popq %rbp

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
