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
 * uint32_t to_grayscale( uint32_t pixel );
 *
 * construct a uint32 pixel that has the grayscale color value of the original pixel
 *
 * Parameter: %rdi - uint32_t pixel
 *
 * Registers:
 * %r12 - red value of pixel
 * %r13 - green value of pixel
 * %r14 - blue value of pixel
 * %r15 - alpha value of pixel
 *
 * Returns: a new uint32 pixel in grayscale value
 */
.globl to_grayscale

to_grayscale:
	call get_r 			// get r value of pixel
	movl %eax, %r12d 	// save red value from rax to r12
	call get_g 			// get g value of pixel
	movl %eax, %r13d 	// save red value from rax to r13
	call get_b 			// get b value of pixel
	movl %eax, %r14d 	// save red value from rax to r14
	call get_a 			// get r value of pixel
	movl %eax, %r15d 	// save red value from rax to r15

	imul $79, %r12d, %r12d		// multiply red for grayscale computation
	imul $128, %r13d, %r13d	// multiply green for grayscale computation
	imul $49, %r14d, %r14d		// multiply green for grayscale computation
 
	addl %r13d, %r12d	// red = red + green
	addl %r14d, %r12d 	// red = red + blue
	shrl $8, %r12d		// shift right by 8 to divide r+g+b in r12 by 256
	
	movl %r12d, %edi 	// 1st parameter for upcoming make_pixel
	movl %r12d, %esi 	// 2nd parameter for upcoming make_pixel
	movl %r12d, %edx 	// 3rd parameter for upcoming make_pixel
	movl %r15d, %ecx 	// 4th parameter for upcoming make_pixel
	call make_pixel 	// make grayscale pixel from computed values

	ret	

/*
 * int64_t gradient( int64_t x, int64_t max );
 *
 * compute the axis-specific int64 value that determines how faded the pixel should be
 *
 * Parameters:
 * %rdi - int64_t x
 * %rsi - int64_t max
 *
 * Registers:
 * %r12 - stores $1000000 that temp^2 will be subtracted from
 *
 * Returns: gradient value between 0 and 1000000
 */
.globl gradient
gradient:
	pushq %rbp
	subq $8, %rsp
	pushq %r12

	imul $2000000000, %rdi	//2000000000*x
	imul $1000000, %rsi		//1000000*max

	movq $0, %rdx			//make sure upper 64 bits of rdx is 0
	movq %rdi, %rax			//move dividend to rax
	divq %rsi				//rax = rax/rdi = (2000000000*x)/(1000000*max)
	subq $1000, %rax		//rax = rax - 1000 = temp

	imul %rax, %rax			//rax = rax^2 = temp^2
	movq $1000000, %r12		//save temp^2 to r12 for subtraction
	subq %rax, %r12			//r12 = r12 - rax = 1000000 - temp^2
	movq %r12, %rax			//save result to rax to be returned

	popq %r12
	addq $8, %rsp
	popq %rbp

	ret

/*
 * uint32_t to_fade( int64_t gradrow, int64_t gradcol, uint32_t pixel);
 *
 * construct a faded uint32 pixel relative to its index
 *
 * Parameters:
 * %rdi - int64_t gradrow
 * %rsi - int64_t gradcol
 * %rdx - uint32_t pixel
 *
 * Registers:
 * %rcx - temporary save for 1000000000000 divisor
 * %rbx - temporary save for pixel
 * %r12 - faded red value
 * %r13 - faded green value
 * %r14 - faded blue value
 * %r15 - unchanged alpha value
 *
 * Returns: a new uint32 faded pixel
 */
.globl to_fade
to_fade:
	pushq %rbp
	subq $8, %rsp
	pushq %rcx
	pushq %rbx
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15

	imul %rdi, %rsi 			//store gradcol * gradrow to rsi
	movq %rdx, %rdi				//pass pixel to rdi for get_x calls
	movq $1000000000000, %rcx 	//save 1000000000000 to rcx for future division

	call get_r			
	imul %rsi, %rax		//rax = rax * rsi = red value * gradcol * gradrow
	movq $0, %rdx		//set rdx to 0 to prepare for division
	divq %rcx			//rax = rax/rcx = rax/1000000000000
	movq %rax, %r12		//save faded red value to r12
			
	call get_g
	imul %rsi, %rax		//rax = rax * rsi = green value * gradcol * gradrow
	movq $0, %rdx		//set rdx to 0 to prepare for division
	divq %rcx			//rax = rax/rcx = rax/1000000000000
	movq %rax, %r13		//save faded green value to r13

	call get_b
	imul %rsi, %rax		//rax = rax * rsi = blue value * gradcol * gradrow
	movq $0, %rdx		//set rdx to 0 to prepare for division
	divq %rcx			//rax = rax/rcx = rax/1000000000000
	movq %rax, %r14		//save faded blue value to r14

	call get_a
	movq %rax, %r15		//save alpha value to r15

	movq %r12, %rdi		//set faded red value to 1st parameter
	movq %r13, %rsi		//set faded green value to 2nd parameter
	movq %r14, %rdx		//set faded blue value to 3rd parameter
	movq %r15, %rcx		//set alpha value to 4th parameter
	call make_pixel		//make the pixel from those 4 parameters

	popq %r15
	popq %r14
	popq %r13
	popq %r12
	popq %rbx
	popq %rcx
	addq $8, %rsp
	popq %rbp

	ret

/*
 * int32_t compute_index( int32_t width, int32_t col, int32_t row );
 *
 * compute the 1d index value from specified col and row indices
 *
 * Parameters:
 * %edi - int32_t width
 * %esi - int32_t col
 * %edx - int32_t row
 *
 * Returns: int32 index value
 */
.globl compute_index
compute_index:
	imul %edx, %edi		//multiply width and row
	addl %esi, %edi		//add col to product
	movl %edi, %eax		//save final result to rax to be returned
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
	// align stack
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
	movq %rdi, %r14	//store original image to r14
	movq %rsi, %r15	//store output image to r15

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
	jae .ColEnd	//jumps to ColEnd function if width <= j



	movl %r10d, %r9d // Store width to r9d which gets the data array address
	imull %r12d, %r9d // Multiply width in r10 by i 
	addl %r13d, %r9d // Add j (col) to that value


	// r9d has the index to the current pixel we are at in data


	//get the pixel
	movq %r14, %r8 	// Copy the original image pointer to %r8
	addq $8, %r8 	// Get to the data pointer by offsetting the pointer by 8 (skips width and height)
	movq (%r8), %r8	// Start at the beginning of the array of data
	movl (%r8, %r9, 4), %edi// Access the array values and move it to register %edi (32-bit rdi) (so we can do our calls for get_r etc)


	// Get rgba components
	call get_r	
	movl %eax, -8(%rbp)   // Save red component from rax to rbp 
	call get_g
	movl %eax, -12(%rbp)  // Save green component from rax to rbp 
	call get_b
	movl %eax, -16(%rbp)  // Save blue component from rax to rbp 
	call get_a
	movl %eax, -20(%rbp)  // Save alpha component from rax to rbp 



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

	//move rgb values to their registers before calling make_pixel
	movq -8(%rbp), %rdi //moves red pixel to rdi
	movq $0, %rsi	//moves 0 to green rsi
	movq $0, %rdx	//moves 0 to blue rdx
	movq -20(%rbp), %rcx 	//moves alpha value to rcx
	
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

	//move rgb values to their registers before calling make_pixel
	movq $0, %rdi	//moves 0 to red register
	movq -12(%rbp), %rsi //moves the g_value to rsi
	movq $0, %rdx	//moves 0 to blue register
	movq -20(%rbp), %rcx //moves the alpha_value to rcx
	
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

	movq $0, %rdi	//move 0 to the red register
	movq $0, %rsi	//move 0 to the green register
	movq -16(%rbp), %rdx	//move the b_val to the blue register rdx
	movq -20(%rbp), %rcx	//move the alpha value to the rcx register
	
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

	
	addq $32, %rsp //deallocate the memory used from the stack
	popq %rbp	//restory rbp register to original value

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
 * 
 *
 */
	.globl imgproc_grayscale
imgproc_grayscale:
	//align stack
	pushq %rbp

	//Allocate enough space for local variables
	movq %rsp, %rbp
	subq $8, %rsp

	// Get width and height from input Image rdi
	movl (%rdi), %r10d //width stored in r10d
	movl 4(%rdi), %r11d //height stored in r11d


	// Store the struct pointers to registers r14 and r15
	movq %rdi, %r14	//store original image to r14
	movq %rsi, %r15	//store output image to r15

	// Initialize iterator i, j to 0
	movl $0, %r12d	
	movl $0, %r13d

	// Iterates through loop and checks if i >= height to stop iterating through loop
.RowLoopGrayscale:
	// Compare if i >= height, jump to RowEnd function if lower
	cmpl %r11d, %r12d
	jae .RowEndGrayscale		// Exit the nested loop

	// Initialize iterator j=0 for the ColLoop
	movl $0, %r13d
.ColLoopGrayscale:
	// Compare j < width to continue
	cmpl %r10d, %r13d
	jae .ColEndGrayscale	//jumps to ColEnd function if width <= j


	//store width and height in stack
	pushq %r10
	pushq %r11

	//get the 1d index
	movl %r10d, %edi 	//width is 1st parameter
	movl %r13d, %esi	//j as col is 2nd parameter
	movl %r12d, %edx	//i as row in 3rd parameter
	call compute_index
	movl %eax, %r9d		//move 1d index result to r9d

	//return width and height to registers
	popq %r11
	popq %r10

	// r9d has the index to the current pixel we are at in data


	//get the pixel
	movq 8(%r14), %r8   		// Load the data pointer from offset 8 of the input image struct
	movl (%r8, %r9, 4), %edi 	// Access the array values and move it to register %edi (32-bit rdi) (so we can do our calls for get_r etc)
	

	//store everything in stack since to_grayscale accesses those registers
	//not directly accessed by to_grayscale but are caller-saved 
	pushq %r9			//current index
	pushq %r10			//width
	pushq %r11			//height
	//directly accessed by to_grayscale
	pushq %r12			//i
	pushq %r13			//j
	pushq %r14			//input_img
	pushq %r15			//output_img

	call to_grayscale			//Get grayscale value 
	

	//return everything to their respective register
	popq %r15
	popq %r14
	popq %r13
	popq %r12
	popq %r11
	popq %r10
	popq %r9
	

	movq 8(%r15), %r8			//Load the output image's data pointer into r8
	movl %eax, (%r8, %r9, 4) 	//Put the grayscale value into output image's data address
	

	
	// Increment j counter by 1
	incl %r13d
	jmp  .ColLoopGrayscale // Continue the j loop

.ColEndGrayscale:
	// Increment i counter by 1
	incl %r12d
	jmp  .RowLoopGrayscale // Go back to the Row loop with i incremented by 1

.RowEndGrayscale:
	// Restore callee-saved registers

	addq $8, %rsp //deallocate the memory used from the stack
	popq %rbp	//restory rbp register to original value


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
	//align stack
	pushq %rbp

	//Allocate enough space for local variables
	movq %rsp, %rbp
	subq $8, %rsp

	// Get width and height from input Image rdi
	movl (%rdi), %r10d //width stored in r10d
	movl 4(%rdi), %r11d //height stored in r11d


	// Store the struct pointers to registers r14 and r15
	movq %rdi, %r14	//store original image to r14
	movq %rsi, %r15	//store output image to r15

	// Initialize iterator i, j to 0
	movl $0, %r12d	
	movl $0, %r13d

	// Iterates through loop and checks if i >= height to stop iterating through loop
.RowLoopFade:
	// Compare if i >= height, jump to RowEnd function if lower
	cmpl %r11d, %r12d
	jae .RowEndFade		// Exit the nested loop

	// Initialize iterator j=0 for the ColLoop
	movl $0, %r13d
.ColLoopFade:
	// Compare j < width to continue
	cmpl %r10d, %r13d
	jae .ColEndFade	//jumps to ColEnd function if width <= j


	//store width and height in stack
	pushq %r10
	pushq %r11

	//get the 1d index
	movl %r10d, %edi 	//width is 1st parameter
	movl %r13d, %esi	//j as col is 2nd parameter
	movl %r12d, %edx	//i as row in 3rd parameter
	call compute_index
	movl %eax, %r9d		//move 1d index result to r9d

	//return width and height to registers
	popq %r11
	popq %r10

	// r9d has the index to the current pixel we are at in data

	//get the pixel
	pushq %rbx
	movq 8(%r14), %r8   		// Load the data pointer from offset 8 of the input image struct
	movl (%r8, %r9, 4), %ebx 	// move pixel value to ebx

	//PROBLEM W EDX FROM T0_FADE

	//store everything in stack since gradient and fade accesses those registers
	pushq %r9			//current index
	pushq %r14			//input_img
	pushq %r15			//output_img

	//get gradrow and gradcol
	movq %r12, %rdi		//i 1st parameter
	movq %r11, %rsi		//height 2nd parameter
	pushq %r12			//save i 
	pushq %r11			//save height
	call gradient
	popq %r11			//restore height
	popq %r12			//restore i
	pushq %rax			//push gradrow to stack

	movq %r13, %rdi		//j 1st parameter
	movq %r10, %rsi		//width 2nd parameter
	pushq %r13			//save j
	pushq %r10			//save width
	call gradient
	popq %r10			//restore width
	popq %r13			//restore j
	pushq %rax			//push gradcol to stack
	

	//POTENTIAL PROBLEM: CHECK TO FADE'S RCX AND RBX
	//						GRADIENT AND TO FADE BOTH USES R12

	popq %rsi			//pop gradcol from stack to 2nd parameter rsi
	popq %rdi			//pop gradrow from stack to 1st parameter rdi
	movq %rbx, %rdx		//move pixel from rbx to rdx
	call to_fade			//Get faded value 
	

	//return everything to their respective register
	popq %r15
	popq %r14
	popq %r9
	
	popq %rbx

	movq 8(%r15), %r8			//Load the output image's data pointer into r8
	movl %eax, (%r8, %r9, 4) 	//Put the faded value into output image's data address
	

	
	// Increment j counter by 1
	incl %r13d
	jmp  .ColLoopFade // Continue the j loop

.ColEndFade:
	// Increment i counter by 1
	incl %r12d
	jmp  .RowLoopFade // Go back to the Row loop with i incremented by 1

.RowEndFade:
	// Restore callee-saved registers

	addq $8, %rsp //deallocate the memory used from the stack
	popq %rbp	//restory rbp register to original value


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
 *
 *
 *
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
 



	.globl imgproc_kaleidoscope
imgproc_kaleidoscope:
	// align stack
	pushq %rbp
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
	movl (%rsi), %r10d // width stored in r10d
	movl 4(%rsi), %r11d // height stored in r11d
	//compare the width and height and see if they are equal
	cmpl %r10d, %r11d
	jne .TrashDimensions //if not equal, jump to trashdimensions that returns 0

	movl %r10d, %r14d	//move the width to r14
	shrl $1, %r14d // divide width by 2
	movl %r11d, %r15d //move the height to r15
	shrl $1, %r15d // divide height by 2


	movl $0, %r12d // set i = 0 to start the forloop


.RowLoopK:
	cmpl %r15d, %r12d //if i < height/2
	jae .RowEndK // Exit the loop if i >= height/2

	movl %r12d, %r13d // j = i

.ColLoopK:
	cmpl %r14d, %r13d // if j < width/2
	jae .ColEndK // Exit the loop if j >= width/2

	

	// Calculate the index in the input image
	movl %r10d, %r9d // Store width to r9d which gets the data array address
	imull %r12d, %r9d // // Multiply width in r10 by i 
	addl %r13d, %r9d // Add j (col) to that value


	// r9d has the index to the current pixel we are at in data



	// Get the pixel
	movq %rdi, %r8 	// Copy the original image pointer to %r8
	addq $8, %r8 	// Get to the data pointer by offsetting the pointer by 8 (skips width and height)
	movq (%r8), %r8	// Start at the beginning of the array of data


	//Top left
	//section A
	movl (%r8, %r9, 4), %eax // Access the array values and move it to register %eax (32-bit rax) 
	movq %rsi, %r8 	// Copy the output image pointer to %r8
	addq $8, %r8 	// Get to the data pointer array by adding 8
	movq (%r8), %r8	// Set the pointer to the beginning of the array
	movl %eax, (%r8, %r9, 4) //put the pixel on the output image

	//section B
	// Calculate the index for wedge B (flip i and j)
	movl %r10d, %r9d // Store width to r9d
	imull %r13d, %r9d // Multiply width by j 
	addl %r12d, %r9d // Add i  to get the index

	// Copy the pixel to wedge B
	movl %eax, (%r8, %r9, 4) // Store the pixel in the output image (wedge B)

	//Top right quadrant
    //Mirror A across y axis
	movl %r10d, %r9d // Store width to r9d
	imull %r12d, %r9d // Multiply width by i 
	addl %r10d, %r9d //add width to r9d
	subl $1, %r9d //subtract one to reach a existing pointer
	subl %r13d, %r9d // subtract j to get the index
	movl %eax, (%r8, %r9, 4) // Store the pixel in the output image 


	//Mirror B across y axis
	movl %r10d, %r9d // Store width to r9d
	imull %r13d, %r9d // Multiply width by i 
	addl %r10d, %r9d //add width to r9d
	subl $1, %r9d //subtract one to reach a existing pointer
	subl %r12d, %r9d // subtract j to get the index
	movl %eax, (%r8, %r9, 4) // Store the pixel in the output image 


	//Mirror A across x axis Call this C
	movl %r10d, %r9d // Store width to r9d
	subl $1, %r9d //subtract one to reach a existing pointer
	subl %r12d, %r9d // subtract i to get the index
	imull %r10d, %r9d // Multiply width by (size - row - 1) 
	addl %r13d, %r9d //add j to r9d
	movl %eax, (%r8, %r9, 4) // Store the pixel in the output image 


    //Mirror B across x axis Call this D
	movl %r10d, %r9d // Store width to r9d
	subl $1, %r9d //subtract one to reach a existing pointer
	subl %r13d, %r9d // subtract i to get the index
	imull %r10d, %r9d // Multiply width by (size - row - 1) 
	addl %r12d, %r9d //add j to r9d
	movl %eax, (%r8, %r9, 4) // Store the pixel in the output image 

    //Mirror C across y axis
	movl %r10d, %r9d // Store width to r9d
	subl $1, %r9d //subtract one to reach a existing pointer
	subl %r13d, %r9d // subtract i to get the index
	imull %r10d, %r9d // Multiply width by (size - row - 1) 
	subl $1, %r9d //subtract one to reach a existing pointer
	subl %r12d, %r9d //subtract j from r9d
	addl %r10d, %r9d //add width to get the final transformation
	movl %eax, (%r8, %r9, 4) // Store the pixel in the output image 

    //Mirror D across y axis
	movl %r10d, %r9d // Store width to r9d
	subl $1, %r9d //subtract one to reach a existing pointer
	subl %r12d, %r9d // subtract i to get the index
	imull %r10d, %r9d // Multiply width by (size - row - 1) 
	subl $1, %r9d //subtract one to reach a existing pointer
	subl %r13d, %r9d //subtract j from r9d
	addl %r10d, %r9d //add width to get the final transformation
	movl %eax, (%r8, %r9, 4) // Store the pixel in the output image 



	// Increment j
	incl %r13d
	jmp .ColLoopK

.ColEndK:
	// Increment i
	incl %r12d
	jmp .RowLoopK

.RowEndK:
	// Restore callee-saved registers
	popq %r15
	popq %r14
	popq %r13
	popq %r12
	popq %r11
	popq %r10

	addq $32, %rsp // Deallocate the memory used from the stack
	popq %rbp // Restore rbp register to original value

	ret $1

.TrashDimensions:
	ret $0

	/* This avoids linker warning about executable stack */
.section .note.GNU-stack,"",@progbits

/*
vim:ft=gas:
*/
