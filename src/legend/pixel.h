#ifndef PIXEL_H
#define PIXEL_H
#include <stdint.h>

typedef unsigned long pixel_t;
typedef unsigned char component_t;

enum pixel_format {
	PIXEL_BGRA32, PIXEL_RGBA32,
	PIXEL_BGRX32, PIXEL_RGBX32,
};
typedef enum pixel_format pixel_format_t;

static inline pixel_t
pixel_bgra32(component_t r, component_t g, component_t b, component_t a)
{
	return (pixel_t)b | ((pixel_t)g << 8) |
		((pixel_t)r << 16) | ((pixel_t)a << 24);
}

static inline pixel_t
pixel_rgba32(component_t r, component_t g, component_t b, component_t a)
{
	return (pixel_t)r | ((pixel_t)g << 8) |
		((pixel_t)b << 16) | ((pixel_t)a << 24);
}

static inline pixel_t
pixel_bgrx32(component_t r, component_t g, component_t b)
{
	return (pixel_t)b | ((pixel_t)g << 8) | ((pixel_t)r << 16);
}

static inline pixel_t
pixel_rgbx32(component_t r, component_t g, component_t b)
{
	return (pixel_t)r | ((pixel_t)g << 8) | ((pixel_t)b << 16);
}

/* decode a pixel value into components. */
static inline void
pixel_get_bgra32(pixel_t c, component_t *r, component_t *g, component_t *b, component_t *a)
{
	*b = c;
	*g = c >> 8;
	*r = c >> 16;
	*a = c >> 24;
}

/* decode a pixel value into components. */
static inline void
pixel_get_rgba32(pixel_t c, component_t *r, component_t *g, component_t *b, component_t *a)
{
	*r = c;
	*g = c >> 8;
	*b = c >> 16;
	*a = c >> 24;
}

/* decode a pixel value into components. */
static inline void
pixel_get_bgrx32(pixel_t c, component_t *r,
	component_t *g, component_t *b)
{
	*b = c;
	*g = c >> 8;
	*r = c >> 16;
}

/* decode a pixel value into components. */
static inline void
pixel_get_rgbx32(pixel_t c, component_t *r,
	component_t *g, component_t *b)
{
	*r = c;
	*g = c >> 8;
	*b = c >> 16;
}

/* p - image data base pointer
 * r - rowbytes
 * return point started at x, y
 */
static inline void *
getrow32(void *p, int x, int y, size_t r)
{
	return &((uint32_t*)((unsigned char*)p + y * r))[x];
}

/* p - image data pointer
 * r - rowbytes
 * c - color */
static inline void
putpixel32(void *p, int x, int y, size_t r, pixel_t c)
{
	((uint32_t*)((unsigned char*)p + y * r))[x] = c;
}

/* p - row pointer
 * x - column
 * return color */
static inline pixel_t
getpixelfromrow32(void *row, int x)
{
	return ((uint32_t*)row)[x];
}
#endif
