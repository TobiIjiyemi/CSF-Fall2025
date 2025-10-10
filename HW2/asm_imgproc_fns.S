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
	movl %edi, %eax /*move data directly to return register*/
	shrl $24, %eax /*shift right 24 bits, clear the other bits and move red data into the lowest bits*/
	ret

	.globl get_g
get_g:
	movl %edi, %eax /*move data directly to return register*/
	shll $8, %eax
	shrl $24, %eax 
	ret

	.globl get_b
get_b:
	movl %edi, %eax /*move data direclty to return register*/
	shll $16, %eax /*mask just the blue bits*/
	shrl $24, %eax /*shift by 8 bits to move to lowest bits*/
	ret
	
	/*function to get alpha value*/
	.globl get_a
get_a:
	movl %edi, %eax /*move data directly to return register*/
	shll $24, %eax /*mask lower bits of eax and return the result*/
	shrl $24, %eax
	ret

	.globl make_pixel
make_pixel:
	shll $8, %edi /*shift the red bits up by 8 bits*/
	addl %esi, %edi /*add green bits to red bits, */
	shll $8, %edi /*shift green and red up by 8. red is now 16 further than where it started, and green is just +8*/
	addl %edx, %edi /*add blue bits to green and red.*/
	shll $8, %edi /*shift up 8 bits, red is +24 bits, green is +16, and blue is +8 bits.*/
	addl %ecx, %edi /*add remaining alpha bits*/
	movl %edi, %eax /*move resulting value to eax and return*/
	ret

	.globl is_in_ellipse
is_in_ellipse:

	pushq %rbx /* stores row */

	movq %rdx, %r8 /* set r8 = width */
	shrq $1, %r8 /* a_diff = width/2 */

	movq %rcx, %r9 /* set r9 = height */
	shrq $1, %r9 /* b_diff = height/2 */

	movq %rsi, %rax /* rax = col */
	subq %r8, %rax /* dist_x = col - a_diff */
	
	movq %rdi, %rbx /* rbx = row */
	subq %r9, %rbx /* dist_y = row - b_diff */

	imulq %rax, %rax /* rax = dist_x * dist_x */
	imulq $10000, %rax /* 10000 * dist_x^2 */

	movq %r8, %r10
	imulq %r10, %r10 /* a_diff * a_diff */
	
	xorq %rdx, %rdx /* clearing upper bits */
	
	idivq %r10 /* check_x = (10000 * (dist_x * dist_x)) / (a_diff * a_diff); */
	movq %rax, %r10 /* check_x */

	imulq %rbx, %rbx /* dist_y * dist_y */
	imulq $10000, %rbx /* 10000 * esi */
	
	movq %r9, %r11
	imulq %r11, %r11 /* b_diff * b_diff */
	
	movq %rbx, %rax

	xorq %rdx, %rdx /* clearing upper bits */

	idivq %r11 /* check_y = (10000 * (dist_y * dist_y)) / (b_diff * b_diff); */
	
	movq %rax, %r11 /* r11 = check_y */

	addq %r10, %r11 /* check_x + check_y */
	cmpq $10000, %r11 /* check_x + check_y <= 10000 */

	jg .Loutside_ellipse

	.Linside_ellipse:
	movq $1, %rax /* 1 if true */
	jmp .Lepilogue

	.Loutside_ellipse:
	movq $0, %rax /* 0 if false */

	.Lepilogue:
	popq %rbx
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
	movl %r9d, IMAGE_HEIGHT_OFFSET(%rsi) /* Load the height into the output image */

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
	
	/* prologue: pushing callee saved registers being used */
	/*
	* Register use:
	*   %rbp - row counter
	*	%rbx - index
	*	%r12 - input image width
	*   %r13 - input image height
	*   %r14d - input_img->data
	*   %r15d - output_img->data
	*/
	pushq %rbp
	pushq %rbx	
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15
	
	movl IMAGE_WIDTH_OFFSET(%rdi), %r12d /* store the width of image in %r12 */
	movl IMAGE_HEIGHT_OFFSET(%rdi), %r13d /* store the height of rdi into %r13 */

	movl %r12d, IMAGE_WIDTH_OFFSET(%rsi) /* Load the width into the output image */
	movl %r13d, IMAGE_HEIGHT_OFFSET(%rsi) /* Load the height into the output image */

	movq IMAGE_DATA_OFFSET(%rdi), %r14 /* r14 = input_img->data */
	movq IMAGE_DATA_OFFSET(%rsi), %r15 /* r15 = output_img->data */

	xorq %rbp, %rbp /* i = 0 counter for outer loop */	
	
	.Lrow_loop_start:
	cmpq %r13, %rbp /* comparing i to height, i < height : height - rbp */
	jge .Lrow_loop_end /* if i >= height, then end loop */

	xorq %rcx, %rcx /* count for inner loop */
	
	.Lcolumn_loop_start:
	cmpq %r12, %rcx /* comparing j to width, j < width */
	jge .Lcolumn_loop_end /* if j >= width, then end inner loop */

	/* call is_in_ellipse */
	pushq %rcx

	movq %rbp, %rdi /* first arguement, rows */
	movq %rcx, %rsi /* second arguement, columns */

	movq %r12, %rdx /* third arguement, width */
	movq %r13, %rcx /* fourth arguement, height */

	call is_in_ellipse /* 0 if not in ellipse, 1 if in ellipse */

	cmpq $1, %rax /* if 1, then in ellipse, if not then skip */

	popq %rcx /* resore col */

	jne .Lskip_pixel /* pixel not in ellipse */

	movq %rbp, %rbx
	imulq %r12, %rbx /* rbx = row * width */
	addq %rcx, %rbx /* r10 = (row * width) + col */

	movl (%r14, %rbx, 4), %r11d /* load input pixel into r11d */

	movl %r11d, (%r15, %rbx, 4) /* output pixel = input pixel */

	.Lskip_pixel:
	incq %rcx /* j++ */
	jmp .Lcolumn_loop_start
	
	.Lcolumn_loop_end:
	incq %rbp /* i++ */
	jmp .Lrow_loop_start

	.Lrow_loop_end:
	/* popping callee saved registers that were used */
	popq %r15
	popq %r14
	popq %r13
	popq %r12
	popq %rbx
	popq %rbp
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
    pushq   %r12 /*save all the callee saved registers*/
    pushq   %r13
    pushq   %r14
    pushq   %r15
    pushq   %rbx
    movl    IMAGE_WIDTH_OFFSET(%rdi), %r8d   /* move width to r8*/
    movslq  %r8d, %r8 /*extend sign bit into 64 bits*/
    movl    IMAGE_HEIGHT_OFFSET(%rdi), %r9d  # move height to r9 (from input)
    movl    %r8d, IMAGE_WIDTH_OFFSET(%rsi)   # set width of image to r8d
    movl    %r9d, IMAGE_HEIGHT_OFFSET(%rsi)  # set height of image to r9d
    movq    IMAGE_DATA_OFFSET(%rdi), %r10    # save data pointer to r10
    movq    IMAGE_DATA_OFFSET(%rsi), %r11    # save other data pointer to r11


    xorl    %ebx, %ebx # RBX FOR COUNTER!! j = 0
.toprow_loop_start:
    cmp     %r8d, %ebx  # compare with width
    jge     .toprow_loop_end # if index is >= width, stop
    movl    (%r10, %rbx, 4), %edi # save memory at calculated location to edi
    call    get_a # eax has alpha value
    movl    $128, %edi  # 128 to edi, for red bits
    movl    $128, %esi # 128 to esi for green bits
    movl    %eax, %ecx  # alpha bits calculated above to ecx
    movl    $128, %edx # blue bits in edx set to 128 as well
    call    make_pixel   # new pixel value is in eax
    movl    %eax, (%r11, %rbx, 4) # save in calculated index
    addl    $1, %ebx  # increment by 1 for counter
    jmp     .toprow_loop_start

.toprow_loop_end:
    xorl    %ebx, %ebx  # start loop for left col. i = 0
.leftcol_loop_start:
    cmp     %r9d, %ebx  # make sure counter is less than height
    jge     .leftcol_loop_end  # if index is >= height, stop
    movl    %ebx, %eax   # calculate index i * width
    imull   %r8d, %eax
    movq    %rax, %r12  # use 64 bit r12 for indexing into current pixel
    movl    (%r10, %r12, 4), %edi # get pixel at [i][0]
    call    get_a # alpha bits are in eax now
    movl    $128, %edi # move 128 to first argumetn, red        
    movl    $128, %esi # 128 to second argument, green
    movl    %eax, %ecx # alpha bits to 4th argument, alpha
    movl    $128, %edx # 128 bits to third argument, blue
    call    make_pixel # call make pixel and newly made pixel is in eax
    movl    %eax, (%r11, %r12, 4) # put eax into proper location, indexed by r12
    addl    $1, %ebx # increase counter by 1
    jmp     .leftcol_loop_start

.leftcol_loop_end:
    movl    $1, %r12d  # R12d IS REGISTER FOR OUTER LOOP i
.outer_start:
    cmp     %r9d, %r12d # if i >= height, exit
    jge     .outer_end
    movl    $1, %r13d # R13d IS REGISTER FOR INNER LOOP j
.inner_start:
    cmp     %r8d, %r13d # make sure j < width
    jge     .inner_end

    /*calculating the current pixel index */
    movl    %r12d, %eax  # calculate index of first pixel
    imull   %r8d, %eax   # i * width is in eax
    addl    %r13d, %eax  # i * width + j
    movq    %rax, %r14  # Use 64-bit r14 for current_idx

    /*save loop counters to stack to be updated after middle of the function use*/
    pushq   %r12
    pushq   %r13

    movl    (%r10, %r14, 4), %edi  # set edi to current pixel
    call    get_a   # get alpha_val from current pixel
    movl    %eax, %r15d  # set r15d to alpha bits, to be used near the end of the inner loop when pixel is created.

    /*caluclate upper pixel index using current index - width - 1 */
    movq    %r14, %rax # rax = current_idx
    subq    %r8, %rax  # rax = current_idx - width
    subq    $1, %rax # rax = upper_left_idx, same as (i - 1) (width - 1)

    /*calculate red difference */
    movl    (%r10, %rax, 4), %edi # set edi equal to the upper pixel
    call    get_r  # get just the red bits from the upper pixel, put into eax
    pushq   %rax  # save red bits from upper pixel into the stack
    movl    (%r10, %r14, 4), %edi # get the current pixel and save the value
    call    get_r  # get the red bits value
    popq    %rbx  # bring back red bits and put it into rbx
    subl    %eax, %ebx  # subtract rbx from eax... we'll need to take the difference of this and turn it
    # into an absoulte value

    /*calculating the green difference*/
    movq %r14, %rax
    subq %r8, %rax
    subq $1, %rax
    movl    (%r10, %rax, 4), %edi # we're repeating the process for the green bits
    call    get_g # get the upper pixel's green bits and save to rax
    pushq   %rax  # push rax unto the stack
    movl    (%r10, %r14, 4), %edi  # reload edi with teh current pixel
    call    get_g # get the green bits
    popq    %rcx # and place it into rcx
    subl    %eax, %ecx # green difference is now in r12d
    movl %ecx, %r12d
   
    /*blue difference now */
    movq %r14, %rax
    subq %r8, %rax
    subq $1, %rax
    movl    (%r10, %rax, 4), %edi # last time with blue bits..
    call    get_b # get blue bits of upper
    pushq   %rax # pushq to rax
    movl    (%r10, %r14, 4), %edi # reload current pixel to edi
    call    get_b # put blue bits of current to eax
    popq    %rcx # put the value of the blue upper bits in rcx
    subl    %eax, %ecx  # blue difference is now in r13d
    movl %ecx, %r13d /*move to r13d*/
    movl %ebx, %eax /*move red difference to eax*/

    movl %eax, %ecx /*move eax to ecx*/
    testl %ecx, %ecx /*see the sign of ecx*/
    jns .red_diff_abs_done /*if positve then just skip, no need to abs*/
    negl %ecx /*negatte to make ecx positive... ecx should just be pos red diff rn*/

    .red_diff_abs_done:
    movl %r12d, %edx /*move green differnce bits to edx*/
    testl %edx, %edx /*test sign of green diff*/
    jns .green_diff_abs_done /*if green diff is positive skip to comparison*/
    negl %edx /*make difference positive in temporary var*/
   
    .green_diff_abs_done:
    cmpl %ecx, %edx /*compare green and red difference */
    jle .skip_green /*skip next line if red is winner*/
    movl %r12d, %eax /*if red isn't the winner then move original green difference to eax*/

    .skip_green:
    movl %eax, %ecx /*move eax to ecx*/
    testl %ecx, %ecx /*run same operations where we turn it to positive*/
    jns .current_winner_abs_done
    negl %ecx /*negative if negative to make positive*/

    .current_winner_abs_done:
    movl %r13d, %edx /*take blue difference and put in edx*/
    testl %edx, %edx /*test to see sign*/
    jns .blue_diff_abs_done /*if signi s positive skip next line*/
    negl %edx /*negative to make positive*/

    .blue_diff_abs_done:
    cmpl %ecx, %edx /*compare current max diff with blue diff*/
    jle .skip_blue_update /*if current max still wins then skip next line*/
    movl %r13d, %eax /*update current winner difference to blue difference*/

    .skip_blue_update:
    # section to cap maximum value between 0 and 255
    addl    $128, %eax # add 128 to eax that contains maximum difference
    cmpl    $0, %eax # check if under 0
    jge     .zero_or_greater # if not under zero just continue
    xorl    %eax, %eax # if under 0 then set eax to 0

.zero_or_greater:
    cmpl    $255, %eax # check to see if greater than ceiling of 255
    jle     .under_equal_255 # if under or equal to 255 then skip
    movl    $255, %eax # if greater than 255 then set to 255.

.under_equal_255:
    movl    %eax, %edi # save eax values to edi, esi, edx, and all the other argument registers
    movl    %eax, %esi
    movl    %eax, %edx
    movl    %r15d, %ecx # saved alpha value from r15d and place to 4th argument register
    call    make_pixel  # create the final pixel with max pixel
    movl    %eax, (%r11, %r14, 4) # Store pixel in output buffer

    /*bring back loop counters, the computeed indexes for current and upper*/
    popq    %r13 /*restore both loop counters for i and j*/
    popq    %r12

    addl    $1, %r13d /* increase j by 1 */
    jmp     .inner_start

.inner_end:
    /*add 1 to i*/
    addl    $1, %r12d
    jmp     .outer_start

.outer_end:
    /*release all the values of callee saved registers*/
    popq    %rbx
    popq    %r15
    popq    %r14
    popq    %r13
    popq    %r12
    ret