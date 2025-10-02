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
	.globl get_a

/*function to get alpha value*/
get_a:
	movl %edx, %eax /*move data directly to return register*/
	andl $0xFF, %eax /*mask lower bits of eax and return the result*/
	ret

get_r:
	movl %edx, %eax /*move data directly to return register*/
	shrl $24, %eax /*shift right 24 bits, clear the other bits and move red data into the lowest bits*/
	ret

get_g:
	movl %edx, %eax /*move data directly to return register*/
	andl $0x00FF0000, %eax /*mask just the green bits,*/
	shrl $16, %eax /*shift by 16 bits to move to lowest bits*/
	ret

get_b:
	movl %edx, %eax /*move data direclty to return register*/
	andl $0x0000FF00, %eax /*mask just the blue bits*/
	shrl $8, %eax /*shift by 8 bits to move to lowest bits*/
	ret

	.globl is_in_elipse
is_in_elipse:

	movl %edx, %r8d /* set r8d = width */
	shrl $1, %r8d /* a_diff = width/2 */

	movl %ecx, %r9d /* set r9d = height */
	shrl $1, %r9d /* b_diff = height/2 */

	subl %r8d, %edi /* row minus width/2 so store in edi, dist_x = row - a_diff */
	subl %r9d, %esi /* col minust height/2 so store in esi dist_y = col - b_diff */

	
	imull %edi, %edi /* dist_x * dist_x stored in edi */
	imull $10000, %edi /* 10000 * edi */
	imull %r8d, %r8d /* a_diff * a_diff */
	movl %edi, %eax
	xorl %edx, %edx /* clear upper bits */
	idivl %r8d /* check_x = (10000 * (dist_x * dist_x)) / (a_diff * a_diff); */
	movl %eax, %r12d

	imull %esi, %esi /* dist_y * dist_y stored in esi */
	imull $10000, %esi /* 10000 * esi */
	imull %r9d, %r9d /* b_diff * b_diff */
	movl %esi, %eax
	xorl %edx, %edx /* clear upper bits */
	idivl %r9d /* check_y = (10000 * (dist_y * dist_y)) / (b_diff * b_diff); */
	movl %eax, %r13d

	addl %r12d, %r13d /* check_x + check_y */
	cmpl $10000, %r13d /* check_x + check_y <= 10000 */

	jle .inside_ellipse
	jg .outside_ellipse

	.inside_ellipse
	movl $1, %eax
	ret

	.outside_ellipse
	movl $0, %eax
	ret



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
	/* using the callee saved registers rbx, r10, and r11*/
	pushq %rbx
	pushq %r10
	pushq %r11
	movl IMAGE_WIDTH_OFFSET(%rdi), %r8d /* store the width of image in %r12 */
	movl IMAGE_HEIGHT_OFFSET(%rdi), %r9d /* store the height of rdi into %r13 */

	movl %r8d, IMAGE_WIDTH_OFFSET(%rsi) /* store new width of image in address at %rsi*/
	movl %r9d, IMAGE_HEIGHT_OFFSET(%rsi) /* store new height of image at address %rsi + 4*/

	/* reserve %eax register for multiplication of height and width*/
	movl %r8d, %eax
	imull %r9d

	/*storing the data pointer of input image in r10 */
	movq IMAGE_DATA_OFFSET(%rdi), %r10

	/*storing the data pointer of output image in r11*/
	movq IMAGE_DATA_OFFSET(%rsi), %r11

	/* initializing counter index to 0*/
	xorq %rdx, %rdx

.perform_complement_start:
	/* compare value in rdx and rax (contains eax value storing width*heigth), see if index is within image range*/
	cmpq %rax, %rdx

	/*jump to the end of the loop if rdx is greater than or equal to rax, if index is greater than or equal to height * width*/
	jge .perform_complement_end

	/*access pixel[i] using the base, index, and scale operation and put it in ebx*/
	movl (%r10, %rdx, 4), %ebx

	/* inverts all 32 bits */
	notl %ebx

	/* flips just the alpha bits */ 
	xorl $0x000000FF, %ebx

	/* put ebx into it's associated location in pixel[i]*/ 
	movl %ebx, (%r11, %rdx, 4)

	/* update loop counter and jump back*/
	addq $1, %rdx
	/*jump back up to start of the loop*/
	jmp .perform_complement_start

.perform_complement_end:
	/*realign the stack by popping the callee saved registers we pushed onto there*/
	popq %rbx
	popq %r11
	popq %r10
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
	
	movl IMAGE_WIDTH_OFFSET(%rdi), %r8d /* store the width of image in %r8 */
	movl IMAGE_HEIGHT_OFFSET(%rdi), %r9d /* store the height of rdi into %r9 */

	movl %r8d, IMAGE_WIDTH_OFFSET(%rsi) /* Load the width into the output image */
	movl %r9d, IMAGE_HEIGHT_OFFSET(%rsi) /* Load the height into the output image */

	movl IMAGE_DATA_OFFSET(%rsi), %r10d

	xorl %eax, %eax /* i = 0 counter for outer loop */	
	
	.row_loop_start:
	cmpl %r9d, %eax /* comparing i to height, i < height */
	jge .row_loop_end /* if i >= height, then end loop */

	xorl %ecx, %ecx /* count for inner loop */
	
	.column_loop_start:
	cmp %r8d, %ecx /* comparing j to width, j < width */
	jge .column_loop_end /* if j >= width, then end inner loop */

	pushq %rax /* save row */
	pushq %rcx /* save column */

	/* call is_in_ellipse */
	movl %eax, %edi /* first arguement, rows */
	movl %ecx, %esi /* second arguement, columns */

	movl %r8d, %rdx /* third arguement, width */
	movl %r9d, %rcx /* fourth arguement, height */

	call is_in_elipse /* 0 if not in ellipse, 1 if in ellipse */

	movl %eax, %r11d /* store 0 or 1 in r11d */


	popq %rax /* restore column */
	popq %rcx /* restore row */

	movl %rax, %r12d /* store column in r12 */

	imull %r8d, %eax /* row * width */
	addl %ecx, %eax /* rcx stores row * width + col */



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
	movl IMAGE_WIDTH_OFFSET(%rdi), %r8d /*move width to r8*/
	movl IMAGE_HEIGHT_OFFSET(%rsi), %r9d /*move height to r9*/

	movl %r8d, IMAGE_WIDTH_OFFSET(%rsi) /*set width of image to r8d*/
	movl %r9d, IMAGE_HEIGHT_OFFSET(%rsi) /*set height of image to r9d*/

	xorl %eax, %ecx /*start loop to go through top row of image*/
	.toprow_loop_start:
	cmp %r8d, %ecx /*compare with width, for this subfunction we're only loop through the top of the iamge*/
	jge .toprow_loop_end /*if index is greater than width, stop*/
	movq IMAGE_DATA_OFFSET(%rdi), %r10 /*save data pointer to r10*/
	movq IMAGE_DATA_OFFSET(%rsi), %r11 /*save other data pointer to r11*/
	movl (%r10, %rcx, 4), %ebx /*save memory at calculated location to ebx*/
	pushq %rdi /*save rdi to the stack*/
	movl %ebx, %edx /*save ebx to first argument register*/
	call get_a /*eax should contain just the alpha values of ebx now*/
	
	addl $1, ecx /*increment by 1 for counter*/
	jmp .toprow_loop_start

	.toprow_loop_end:
	ret

/*
vim:ft=gas:
*/
