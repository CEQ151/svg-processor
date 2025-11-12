/*=====================================================================
  svg_render.c  –  控制台打印 + 光栅化（RGB24）
  修复：所有 set_pixel 调用都传入高度 h，移除非法 doc 引用
  =====================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "svg_render.h"

/* ---------- 控制台打印 ---------- */
void svg_print_summary(const SvgDocument *doc) {
    int cnt = 0;
    for (SvgShape *s = doc->shapes; s; s = s->next) ++cnt;
    printf("SVG Document: width=%.2f, height=%.2f\n", doc->width, doc->height);
    printf("Total shapes: %d\n", cnt);
}

void svg_print_shapes(const SvgDocument *doc) {
    for (SvgShape *s = doc->shapes; s; s = s->next) {
        printf("[%d] ", s->id);
        switch (s->type) {
            case SVG_SHAPE_CIRCLE:
                printf("CIRCLE: cx=%.2f, cy=%.2f, r=%.2f, fill=#%06X\n",
                       s->data.circle.cx, s->data.circle.cy, s->data.circle.r,
                       s->data.circle.fill_color);
                break;
            case SVG_SHAPE_RECT:
                printf("RECT: x=%.2f, y=%.2f, width=%.2f, height=%.2f, fill=#%06X\n",
                       s->data.rect.x, s->data.rect.y,
                       s->data.rect.width, s->data.rect.height,
                       s->data.rect.fill_color);
                break;
            case SVG_SHAPE_LINE:
                printf("LINE: from (%.2f,%.2f) to (%.2f,%.2f), stroke=#%06X\n",
                       s->data.line.x1, s->data.line.y1,
                       s->data.line.x2, s->data.line.y2,
                       s->data.line.stroke_color);
                break;
        }
    }
}

/* ---------- 像素设置 ---------- */
static void set_pixel(uint8_t *pix, int w, int h, int x, int y, unsigned int col) {
    if (x < 0 || x >= w || y < 0 || y >= h) return;
    int idx = (y * w + x) * 3;
    pix[idx]     = (col >> 16) & 0xFF;  // R
    pix[idx + 1] = (col >> 8)  & 0xFF;  // G
    pix[idx + 2] = col & 0xFF;          // B
}

/* ---------- 画线：Bresenham 算法 ---------- */
static void draw_line(uint8_t *pix, int w, int h,
                      double x0, double y0, double x1, double y1,
                      unsigned int col) {
    int ix0 = (int)round(x0), iy0 = (int)round(y0);
    int ix1 = (int)round(x1), iy1 = (int)round(y1);
    int dx = abs(ix1 - ix0), sx = ix0 < ix1 ? 1 : -1;
    int dy = -abs(iy1 - iy0), sy = iy0 < iy1 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        set_pixel(pix, w, h, ix0, iy0, col);
        if (ix0 == ix1 && iy0 == iy1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; ix0 += sx; }
        if (e2 <= dx) { err += dx; iy0 += sy; }
    }
}

/* ---------- 填充矩形 ---------- */
static void fill_rect(uint8_t *pix, int w, int h,
                      double x, double y, double wd, double ht,
                      unsigned int col) {
    int ix = (int)round(x), iy = (int)round(y);
    int iwd = (int)round(wd), iht = (int)round(ht);
    for (int j = 0; j < iht; ++j) {
        for (int i = 0; i < iwd; ++i) {
            set_pixel(pix, w, h, ix + i, iy + j, col);
        }
    }
}

/* ---------- 填充圆：中点圆算法（简单） ---------- */
static void fill_circle(uint8_t *pix, int w, int h,
                        double cx, double cy, double r,
                        unsigned int col) {
    int icx = (int)round(cx), icy = (int)round(cy);
    int ir = (int)round(r);
    for (int y = -ir; y <= ir; ++y) {
        int x_max = (int)sqrt(ir * ir - y * y + 0.5);
        for (int xx = -x_max; xx <= x_max; ++xx) {
            set_pixel(pix, w, h, icx + xx, icy + y, col);
        }
    }
}

/* ---------- 主光栅化函数 ---------- */
uint8_t *svg_rasterize(const SvgDocument *doc, int *out_w, int *out_h) {
    int w = (int)ceil(doc->width);
    int h = (int)ceil(doc->height);
    if (w <= 0) w = 1;
    if (h <= 0) h = 1;

    size_t size = (size_t)w * h * 3;
    uint8_t *pix = calloc(1, size);
    if (!pix) return NULL;

    // 白底
    memset(pix, 255, size);

    for (SvgShape *s = doc->shapes; s; s = s->next) {
        switch (s->type) {
            case SVG_SHAPE_CIRCLE:
                fill_circle(pix, w, h,
                            s->data.circle.cx, s->data.circle.cy, s->data.circle.r,
                            s->data.circle.fill_color);
                break;
            case SVG_SHAPE_RECT:
                fill_rect(pix, w, h,
                          s->data.rect.x, s->data.rect.y,
                          s->data.rect.width, s->data.rect.height,
                          s->data.rect.fill_color);
                break;
            case SVG_SHAPE_LINE:
                draw_line(pix, w, h,
                          s->data.line.x1, s->data.line.y1,
                          s->data.line.x2, s->data.line.y2,
                          s->data.line.stroke_color);
                break;
        }
    }

    *out_w = w;
    *out_h = h;
    return pix;
}