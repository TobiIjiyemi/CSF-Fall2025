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

	.globl get_r
get_r:
	movl 4(%rsp), %eax /* store the value in 4+rsp from the stack into eax */ 
	shr 24, %eax
	ret

	.globl get_g
get_g:
	movl 4(%rsp), %eax /* store the value located at rsp+4 in eax*/ 
	shl 8, %eax
	shr 24, *eax
	ret

	.globl get_b
get_b: 
	movl 4(%rsp), %eax
	shl 16, %eax
	shr 24, %eax
	ret

	.globl get_a
get_a:
	movl 4(%rsp), %eax
	shl 24, %eax
	shr 24, %eax
	ret

	.globl make_pixel
make_pixel:
	movl $0, %eax /* Equivalent of setting pixel equal to 0 */
	addl 4(%rsp), %eax
	shl 8, %eax





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
	subq $8, %rsp

	movl IMAGE_WIDTH_OFFSET(%rdi), %r12 /* store the width of image in %r12 */
	movl IMAGE_HEIGHT_OFFSET(%rdi), %r13 /* store the height of rdi into %r13 */

	movl %r12, IMAGE_WIDTH_OFFSET(%rsi) /* store new width of image in address at %rsi*/
	movl %r13, IMAGE_HEIGHT_OFFSET(%rsi) /* store new height of image at address %rsi + 4*/

	/*reserve %eax register for multiplication of height and width*/
	movl $1, %eax
	mull %r12
	mull %r13

	/*eax should now contain product of widht and height 
	* we can use to compare and keep track of when image is finished
	*/
	
	/* initialize index to 0*/
	movl $0, %ebp

	/* compare value in ebp and value in eax, see if index is within image range*/
	cmp %eax %ebp
	movl IMAGE_DATA_OFFSET(%rdi, %ebp, 4), %ebx
	jl .perform_complement

	/* missing code in the middle, still in implementation phase*/
	
	/* sorta like a recursive function call */
	jl .imgproc_complement

	/* still to implement */
	addq $8, %rsp
	ret

.perform_complement
	movl %ebx, %ecx
	AND %ebx, 0xFFFFFF00
	NOT %ebx
	AND %ebx, 0xFFFFFF00
	AND %ecx, 0xFF
	OR %ecx, %ebx
	movl %ebx, IMAGE_DATA_OFFSET(%rsi, %ebp, 4)
	addl $1, %ebp
	call imgproc_complement

/*
 *  Transform the input image by swapping the row and column
 *  of each source pixel when copying it to the output image.
 *  E.g., a pixel at row i and column j of the input image
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

	movl IMAGE_WIDTH_OFFSET(%rdi), %r12 /* store the width of image in %r12 */
	movl IMAGE_HEIGHT_OFFSET(%rdi), %r13 /* store the height of rdi into %r13 */

	cmpl %r12, %r13	/* Comparing width and height to ensure square image */
	
	jne .not_Square /* If width and height are not equal, then jump to the label not_square  */

	movl %r12, IMAGE_WIDTH_OFFSET(%rsi) /* Load the width into the output image */
	movl %r13, IMAGE_HEIGHT_OFFSET(%rsi) /* Load the height into the output image */

	mov $0, %eax /* counter for outer loop */
	

	.outer_loop_start:
	cmp %eax, %r13 /* comparing i to height, i < height */
	jge .outer_loop_end /* if i >= height, then end loop */


	mov $0, %ecx /* count for inner loop */
	.inner_loop_start:
	cmp %ecx, %r12 /* comparing j to width, j < width */
	jge .inner_loop_end /* if j >= width, then end inner loop */


	incl %ecx /* j++ */
	jmp .inner_loop_start 
	.inner_loop_end:

	incl %eax /* i++ */
	jmp .outer_loop_start /* return to outer loop i < height times */
	.outer_loop_end:


	cmp





	.not_square: 
	movq $0, %eax

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
