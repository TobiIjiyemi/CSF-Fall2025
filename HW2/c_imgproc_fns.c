// C implementations of image processing functions

#include <stdlib.h>
#include <assert.h>
#include "imgproc.h"

// TODO: define your helper functions here

//! Transform the color component values in each input pixel
//! by applying the bitwise complement operation. I.e., each bit
//! in the color component information should be inverted
//! (1 becomes 0, 0 becomes 1.) The alpha value of each pixel should
//! be left unchanged.
//!
//! @param input_img pointer to the input Image
//! @param output_img pointer to the output Image (in which the
//!                   transformed pixels should be stored)

uint32_t get_r(uint32_t data) {
  return  (data >> 24);
}

uint32_t get_g(uint32_t data) {
  return (data << 8) >> 24;
}

uint32_t get_b(uint32_t data) {
  return (data << 16) >> 24;
}

uint32_t get_a(uint32_t data) {
  return (data << 24) >> 24;
}

uint32_t make_pixel (uint32_t r, uint32_t g, uint32_t b, uint32_t a) {
  uint32_t pixel = 0;
  pixel += r;
  pixel = pixel << 8;

  pixel += g;
  pixel = pixel << 8;

  pixel += b;
  pixel = pixel << 8;

  pixel += a;
  return pixel;
}



//! For all other pixels, we consider the pixel's color component
//! values r, g, and b, and also the pixel's upper-left neighbor's
//! color component values nr, ng, and nb. In comparing the color
//! component values of the pixel and its upper-left neighbor,
//! we consider the differences (nr-r), (ng-g), and (nb-b).
//! Whichever of these differences has the largest absolute value
//! we refer to as diff. (Note that in the case that more than one
//! difference has the same absolute value, the red difference has
//! priority over green and blue, and the green difference has priority
//! over blue.)
int get_max(int red_d, int blue_d, int green_d) {
  if (abs(red_d) >= abs(blue_d) && abs(red_d) >= abs(green_d)) {
    return red_d;
  }
  if (abs(green_d) >= abs(blue_d)) {
    return green_d;
  }
  return blue_d;
}

void imgproc_complement( struct Image *input_img, struct Image *output_img ) {
  int32_t width = input_img->width;
  int32_t height = input_img->height;
  output_img->width = width;
  output_img->height = height;
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      uint32_t data = input_img->data[i*width + j];
      int32_t inverted_rgb = ~(data >> 8);
      int32_t alphaval = get_a(data);
      output_img->data[i*width + j] = (inverted_rgb << 8) + (alphaval >> 24);
    }
  }
}

//! Transform the input image by swapping the row and column
//! of each source pixel when copying it to the output image.
//! E.g., a pixel at row i and column j of the input image
//! should be copied to row j and column i of the output image.
//! Note that this transformation can only be applied to square
//! images (where the width and height are identical.)
//!
//! @param input_img pointer to the input Image
//! @param output_img pointer to the output Image (in which the
//!                   transformed pixels should be stored)
//!
//! @return 1 if the transformation succeeded, or 0 if the
//!         transformation can't be applied because the image
//!         width and height are not the same
int imgproc_transpose( struct Image *input_img, struct Image *output_img ) {
  // TODO: implement
  uint32_t width = input_img->width;
  uint32_t height = input_img->height;
  if (height != width) {
    return 0;
  }

  // doesn't really matter which the output's dimension is set equal to since it's a square
  output_img->height = width;
  output_img->width = height;

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      uint32_t data = input_img->data[i * width + j];
      output_img->data[j * height + i] = data;
    }
  }
  return 1;
}

//! Transform the input image by copying only those pixels that are
//! within an ellipse centered within the bounds of the image.
//! Pixels not in the ellipse should be left unmodified, which will
//! make them opaque black.
//!
//! Let w represent the width of the image and h represent the
//! height of the image. Let a=floor(w/2) and b=floor(h/2).
//! Consider the pixel at row b and column a is being at the
//! center of the image. When considering whether a specific pixel
//! is in the ellipse, x is the horizontal distance to the center
//! of the image and y is the vertical distance to the center of
//! the image. The pixel at the coordinates described by x and y
//! is in the ellipse if the following inequality is true:
//!
//!   floor( (10000*x*x) / (a*a) ) + floor( (10000*y*y) / (b*b) ) <= 10000
//!
//! @param input_img pointer to the input Image
//! @param output_img pointer to the output Image (in which the
//!                   transformed pixels should be stored)
void imgproc_ellipse( struct Image *input_img, struct Image *output_img ) {
  // TODO: implement

  uint32_t width = input_img->width;
  uint32_t height = input_img->height;

  output_img->width = width;
  output_img->height = height;

  for (int i = 0; i < width * height; i++) {
    output_img->data[i] = make_pixel(0, 0, 0, 255);
  }

  int a_diff = width/2;
  int b_diff = height/2;

  for (uint32_t row = 0; row < height; row++) {
    for (uint32_t col = 0; col < width; col++) {
      
      int dist_x = (col - a_diff);
      int dist_y = (row - b_diff); 

      uint32_t check_x = (int)((10000 * (dist_x * dist_x)) / (a_diff*a_diff));
      uint32_t check_y = (int)((10000 * (dist_y * dist_y)) / (b_diff*b_diff));

      if (check_x + check_y <= 10000) {
        uint32_t pixel = input_img->data[row * width + col];
        
        uint32_t r = get_r(pixel);
        uint32_t g = get_g(pixel);
        uint32_t b = get_b(pixel);
        uint32_t a = get_a(pixel);
        
        output_img->data[row * width + col] = make_pixel(r, g, b, a);
      } 
    }
  }
}

//! Transform the input image using an "emboss" effect. The pixels
//! of the source image are transformed as follows.
//!
//! The top row and left column of pixels are transformed so that their
//! red, green, and blue color component values are all set to 128,
//! and their alpha values are not modified.
//!
//! For all other pixels, we consider the pixel's color component
//! values r, g, and b, and also the pixel's upper-left neighbor's
//! color component values nr, ng, and nb. In comparing the color
//! component values of the pixel and its upper-left neighbor,
//! we consider the differences (nr-r), (ng-g), and (nb-b).
//! Whichever of these differences has the largest absolute value
//! we refer to as diff. (Note that in the case that more than one
//! difference has the same absolute value, the red difference has
//! priority over green and blue, and the green difference has priority
//! over blue.)
//!
//! From the value diff, compute the value gray as 128 + diff.
//! However, gray should be clamped so that it is in the range
//! 0..255. I.e., if it's negative, it should become 0, and if
//! it is greater than 255, it should become 255.
//!
//! For all pixels not in the top or left row, the pixel's red, green,
//! and blue color component values should be set to gray, and the
//! alpha value should be left unmodified.
//!
//! @param input_img pointer to the input Image
//! @param output_img pointer to the output Image (in which the
//!                   transformed pixels should be stored)
void imgproc_emboss( struct Image *input_img, struct Image *output_img ) {
  //two for loops, loop through top row and further left row.
  //set all bits to 128 bits. 
  //input_img->data[j]
  //input_img->data[i * width] i goes through 0 to however many rows,
  uint32_t width = input_img->width;
  uint32_t height = input_img->height;

  output_img->width = width;
  output_img->height = height;

  uint32_t setval = 128;

  for (int j = 0; j < width; j++) {
    uint32_t alpha_val = (input_img->data[j] << 24) >> 24;
    output_img->data[j] = (setval << 8) + (setval << 16) + (setval << 24) + alpha_val;
  }

  for (int i = 0; i < height; i++) {
    uint32_t alpha_val = get_a(input_img->data[i * width]);
    output_img->data[i * width] = (setval << 8) + (setval << 16) + (setval << 24) + alpha_val;
  }
  
  for (int i = 1; i < height; i++) {
    for (int j = 1; j < width; j++) {
      uint32_t alpha_val = get_a(input_img->data[i * width + j]);
      uint32_t current = input_img->data[i * width + j];
      uint32_t upper = input_img->data[(i - 1) * width + (j - 1)];
      int red_diff = (get_r(upper) - get_r(current));
      int green_diff = (get_g(upper) - get_g(current));
      int blue_diff = (get_b(upper) - get_b(current));
      int diff_max = get_max(red_diff, green_diff, blue_diff);
      int setval = 128 + diff_max;
      if (setval < 0) {
        setval = 0;
      } else if (setval > 255) {
        setval = 255;
      }
      output_img->data[i * width + j] = (setval << 8) + (setval << 16) + (setval << 24) + alpha_val;
    }
  }
}