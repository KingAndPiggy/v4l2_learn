
/**
 * @file image.h
 * Image helper functions like resizing, color filter, converters...
 */

#ifndef _CV_LIB_VISION_IMAGE_H
#define _CV_LIB_VISION_IMAGE_H

#include "std.h"
#include <sys/time.h>

/* The different type of images we currently support */
enum image_type {
  IMAGE_YUV422,     ///< UYVY format (uint16 per pixel)
  IMAGE_GRAYSCALE,  ///< Grayscale image with only the Y part (uint8 per pixel)
  IMAGE_JPEG,       ///< An JPEG encoded image (not per pixel encoded)
  IMAGE_GRADIENT    ///< An image gradient (int16 per pixel)
};

/* Main image structure */
struct image_t {
  enum image_type type;   ///< The image type
  uint16_t w;             ///< Image width
  uint16_t h;             ///< Image height
  struct timeval ts;      ///< The timestamp of creation

  uint8_t buf_idx;        ///< Buffer index for V4L2 freeing
  uint32_t buf_size;      ///< The buffer size
  void *buf;              ///< Image buffer (depending on the image_type)
};

/* Image point structure */
struct point_t {
  uint16_t x;             ///< The x coordinate of the point
  uint16_t y;             ///< The y coordinate of the point
};

/* Vector structure for point differences */
struct flow_t {
  struct point_t pos;         ///< The original position the flow comes from
  int16_t flow_x;             ///< The x direction flow in subpixels
  int16_t flow_y;             ///< The y direction flow in subpixels
};

/* Usefull image functions */
void image_create(struct image_t *img, uint16_t width, uint16_t height, enum image_type type);
void image_free(struct image_t *img);
void image_copy(struct image_t *input, struct image_t *output);
void image_switch(struct image_t *a, struct image_t *b);
void image_to_grayscale(struct image_t *input, struct image_t *output);
uint16_t image_yuv422_colorfilt(struct image_t *input, struct image_t *output, uint8_t y_m, uint8_t y_M, uint8_t u_m, uint8_t u_M, uint8_t v_m, uint8_t v_M);
void image_yuv422_downsample(struct image_t *input, struct image_t *output, uint16_t downsample);
void image_subpixel_window(struct image_t *input, struct image_t *output, struct point_t *center, uint16_t subpixel_factor);
void image_gradients(struct image_t *input, struct image_t *dx, struct image_t *dy);
void image_calculate_g(struct image_t *dx, struct image_t *dy, int32_t *g);
uint32_t image_difference(struct image_t *img_a, struct image_t *img_b, struct image_t *diff);
int32_t image_multiply(struct image_t *img_a, struct image_t *img_b, struct image_t *mult);
void image_show_points(struct image_t *img, struct point_t *points, uint16_t points_cnt);
void image_show_flow(struct image_t *img, struct flow_t *vectors, uint16_t points_cnt, uint8_t subpixel_factor);
void image_draw_line(struct image_t *img, struct point_t *from, struct point_t *to);

#endif
