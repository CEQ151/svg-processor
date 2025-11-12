#ifndef SVG_RENDER_H
#define SVG_RENDER_H

#include "svg_types.h"
#include <cstdint>

void svg_print_summary(const SvgDocument *doc);
void svg_print_shapes(const SvgDocument *doc);

/* rasterize to memory buffer (RGB24) */
uint8_t *svg_rasterize(const SvgDocument *doc, int *w, int *h);

#endif /* SVG_RENDER_H */