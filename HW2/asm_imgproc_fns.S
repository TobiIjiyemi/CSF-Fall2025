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
 * Definitions of image transformation functions
 */

/*
 *  Transform the color component values in each input pixel
 *  by applying the bitwise complement operation. I.e., each bit
 *  in the color component information should be inverted
 *  (1 becomes 0, 0 becomes 1.) The alpha value of each pixel should
 *  be left unchanged.
 * 
 *  Parameters:
 *  %rdi - pointer to the input Image
 *  %rsi - pointer to the output Image (in which the
 *         transformed pixels should be stored)
 */
	.globl imgproc_complement
imgproc_complement:
	/* subq $8, %rsp */

	movl IMAGE_WIDTH_OFFSET(%rdi), %r8d /* store the width of image in %r12 */
	movl IMAGE_HEIGHT_OFFSET(%rdi), %r9d /* store the height of rdi into %r13 */

	movl %r8d, IMAGE_WIDTH_OFFSET(%rsi) /* store new width of image in address at %rsi*/
	movl %r9d, IMAGE_HEIGHT_OFFSET(%rsi) /* store new height of image at address %rsi + 4*/

	/*reserve %eax register for multiplication of height and width*/
	movl $1, %eax
	imull %r8d
	imull %r9d

	/*eax should now contain product of widht and height 
	* we can use to compare and keep track of when image is finished
	*/

	/* initialize index to 0*/
	movq $0, %rdx
	movl $0, %ebp

	/* compare value in ebp and value in eax, see if index is within image range*/
	movl IMAGE_DATA_OFFSET(%rdi, %rdx, 4), %ebx
	jl .perform_complement

	.perform_complement:
	jge .complement_end
	movl %ebx, %ecx
	AND $0xFFFFFF00, %ebx
	NOT %ebx
	AND  $0xFFFFFF00, %ebx
	AND $0xFF, %ecx
	OR %ecx, %ebx 
	movl %ebx, IMAGE_DATA_OFFSET(%rsi, %rdx, 4)
	addl $1, %ebp
	addq $1, %rdx
	cmp %eax, %ebp
	jmp .perform_complement

	.complement_end:
	ret

	


/*
 *  Transform the input image by swapping the row and column
 *  of each source pixel when copying it to the output image.
 *  E.g., a pixel at row i and column j of the inxput image
 *  should be copied to row j and column i of the output image.
 *  Note that this transformation can only be applied to square
 *  images (where the width and height are identical.)
 * 
 *  Parameters:
 *  %rdi - pointer to the input Image
 *  %rsi - pointer to the output Image (in which the
 *                    transformed pixels should be stored)
 * 
 *  @return 1 if the transformation succeeded, or 0 if the
 *          transformation can't be applied because the image
 *          width and height are not the same
 */
	.globl imgproc_transpose
imgproc_transpose:
	/* TODO: implement */

	movl IMAGE_WIDTH_OFFSET(%rdi), %r8d /* store the width of image in %r8 */
	movl IMAGE_HEIGHT_OFFSET(%rdi), %r9d /* store the height of rdi into %r9 */

	cmpl %r8d, %r9d	/* Comparing width and height to ensure square image */
	
	jne .not_square /* If width and height are not equal, then jump to the label not_square  */

	movl %r8d, IMAGE_WIDTH_OFFSET(%rsi) /* Load the width into the output image */
	movl %r8d, IMAGE_HEIGHT_OFFSET(%rsi) /* Load the height into the output image */

	movq IMAGE_DATA_OFFSET(%rdi), %r10 /* input_img->data */
	movq IMAGE_DATA_OFFSET(%rsi), %r11 /* output_img->data */

	xorl %eax, %eax /* i = 0 counter for outer loop */	

	.outer_loop_start:
	cmpl %r9d, %eax /* comparing i to height, i < height */
	jge .outer_loop_end /* if i >= height, then end loop */


	xorl %ecx, %ecx /* count for inner loop */
	
	.inner_loop_start:
	cmp %r8d, %ecx /* comparing j to width, j < width */
	jge .inner_loop_end /* if j >= width, then end inner loop */

	movl %eax, %edx /* store i in %edx */
	imull %r8d, %edx /* i times width */
	addl %ecx, %edx /* i * width + j */

	movl (%r10, %rdx, 4), %esi /* set esi equal to input->data[i * width + j] */

	movl %ecx, %edx /* store j in %edx */
	imull %r9d, %edx /* j times width */
	addl %eax, %edx /* j * width + i */

	movl %esi, (%r11, %rdx, 4) /* move esi which stores input data into output->data[j * width + i] */

	incl %ecx /* j++ */
	jmp .inner_loop_start 
	
	.inner_loop_end:
	incl %eax /* i++ */
	jmp .outer_loop_start /* return to outer loop i < height times */
	
	.outer_loop_end:
	movl $1, %eax /* transpose success */
	ret

	.not_square: 
	movl $0, %eax /* transpose fail */
	ret

/*
 *  Transform the input image by copying only those pixels that are
 *  within an ellipse centered within the bounds of the image.
 *  Pixels not in the ellipse should be left unmodified, which will
 *  make them opaque black.
 * 
 *  Let w represent the width of the image and h represent the
 *  height of the image. Let a=floor(w/2) and b=floor(h/2).
 *  Consider the pixel at row b and column a is being at the
 *  center of the image. When considering whether a specific pixel
 *  is in the ellipse, x is the horizontal distance to the center
 *  of the image and y is the vertical distance to the center of
 *  the image. The pixel at the coordinates described by x and y
 *  is in the ellipse if the following inequality is true:
 * 
 *    floor( (10000*x*x) / (a*a) ) + floor( (10000*y*y) / (b*b) ) <= 10000
 * 
 *  Parameters:
 *  %rdi - pointer to the input Image
 *  %rsi - pointer to the output Image (in which the
 *                    transformed pixels should be stored)
 */
	.globl imgproc_ellipse
imgproc_ellipse:
	/* TODO: implement */
	ret

/*
 *  Transform the input image using an "emboss" effect. The pixels
 *  of the source image are transformed as follows.
 * 
 *  The top row and left column of pixels are transformed so that their
 *  red, green, and blue color component values are all set to 128,
 *  and their alpha values are not modified.
 * 
 *  For all other pixels, we consider the pixel's color component
 *  values r, g, and b, and also the pixel's upper-left neighbor's
 *  color component values nr, ng, and nb. In comparing the color
 *  component values of the pixel and its upper-left neighbor,
 *  we consider the differences (nr-r), (ng-g), and (nb-b).
 *  Whichever of these differences has the largest absolute value
 *  we refer to as diff. (Note that in the case that more than one
 *  difference has the same absolute value, the red difference has
 *  priority over green and blue, and the green difference has priority
 *  over blue.)
 * 
 *  From the value diff, compute the value gray as 128 + diff.
 *  However, gray should be clamped so that it is in the range
 *  0..255. I.e., if it's negative, it should become 0, and if
 *  it is greater than 255, it should become 255.
 * 
 *  For all pixels not in the top or left row, the pixel's red, green,
 *  and blue color component values should be set to gray, and the
 *  alpha value should be left unmodified.
 * 
 *  Parameters:
 *  %rdi - pointer to the input Image
 *  %rsi - pointer to the output Image (in which the
 *                    transformed pixels should be stored)
 */
	.globl imgproc_emboss
imgproc_emboss:
	/* TODO: implement */
	ret

/*
vim:ft=gas:
*/
