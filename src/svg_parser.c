/*=====================================================================
  svg_parser.c  –  手写 SVG 解析（只支持 <svg>, <circle>, <rect>, <line>）
  =====================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "svg_parser.h"

static int shape_id = 0;

/* ---------- 工具函数 ---------- */
static void trim(char *s) {
    char *p = s;
    while (isspace(*p)) p++;
    memmove(s, p, strlen(p) + 1);
    p = s + strlen(s) - 1;
    while (p >= s && isspace(*p)) *p-- = '\0';
}

static unsigned int parse_color(const char *s) {
    if (*s == '#') {
        unsigned int r, g, b;
        if (sscanf(s + 1, "%2x%2x%2x", &r, &g, &b) == 3)
            return (r << 16) | (g << 8) | b;
    }
    return 0x000000;   // default black
}

/* ---------- 解析属性 ---------- */
static int parse_double(const char **p, double *val) {
    char *end;
    *val = strtod(*p, &end);
    if (end == *p) return 0;
    *p = end;
    return 1;
}

/* ---------- 解析单个 shape ---------- */
static SvgShape *parse_shape(const char *line) {
    const char *p = line;
    SvgShape *shape = NULL;

    if (strstr(p, "<circle")) {
        shape = calloc(1, sizeof(SvgShape));
        shape->type = SVG_SHAPE_CIRCLE;
        p = strstr(p, "cx="); if (!p) goto err;
        p += 4; if (!parse_double(&p, &shape->data.circle.cx)) goto err;
        p = strstr(p, "cy="); if (!p) goto err;
        p += 4; if (!parse_double(&p, &shape->data.circle.cy)) goto err;
        p = strstr(p, "r=");  if (!p) goto err;
        p += 3; if (!parse_double(&p, &shape->data.circle.r)) goto err;
        p = strstr(p, "fill=");
        shape->data.circle.fill_color = p ? parse_color(p + 6) : 0x000000;
    }
    else if (strstr(p, "<rect")) {
        shape = calloc(1, sizeof(SvgShape));
        shape->type = SVG_SHAPE_RECT;
        p = strstr(p, "x="); if (!p) goto err;
        p += 3; if (!parse_double(&p, &shape->data.rect.x)) goto err;
        p = strstr(p, "y="); if (!p) goto err;
        p += 3; if (!parse_double(&p, &shape->data.rect.y)) goto err;
        p = strstr(p, "width=");  if (!p) goto err;
        p += 7; if (!parse_double(&p, &shape->data.rect.width)) goto err;
        p = strstr(p, "height="); if (!p) goto err;
        p += 8; if (!parse_double(&p, &shape->data.rect.height)) goto err;
        p = strstr(p, "fill=");
        shape->data.rect.fill_color = p ? parse_color(p + 6) : 0x000000;
    }
    else if (strstr(p, "<line")) {
        shape = calloc(1, sizeof(SvgShape));
        shape->type = SVG_SHAPE_LINE;
        p = strstr(p, "x1="); if (!p) goto err;
        p += 4; if (!parse_double(&p, &shape->data.line.x1)) goto err;
        p = strstr(p, "y1="); if (!p) goto err;
        p += 4; if (!parse_double(&p, &shape->data.line.y1)) goto err;
        p = strstr(p, "x2="); if (!p) goto err;
        p += 4; if (!parse_double(&p, &shape->data.line.x2)) goto err;
        p = strstr(p, "y2="); if (!p) goto err;
        p += 4; if (!parse_double(&p, &shape->data.line.y2)) goto err;
        p = strstr(p, "stroke=");
        shape->data.line.stroke_color = p ? parse_color(p + 8) : 0x000000;
    }
    else {
        return NULL;   // unknown tag
    }

    shape->id = ++shape_id;
    return shape;

err:
    free(shape);
    return NULL;
}

/* ---------- 主入口 ---------- */
int svg_load_from_file(const char *filename, SvgDocument **doc_out) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return -1;

    SvgDocument *doc = calloc(1, sizeof(SvgDocument));
    SvgShape **tail = &doc->shapes;
    char line[2048];

    while (fgets(line, sizeof(line), fp)) {
        trim(line);
        if (!*line) continue;

        if (strstr(line, "<svg")) {
            const char *p = strstr(line, "width=");
            if (p) { p += 7; parse_double(&p, &doc->width); }
            p = strstr(line, "height=");
            if (p) { p += 8; parse_double(&p, &doc->height); }
        }
        else {
            SvgShape *sh = parse_shape(line);
            if (sh) {
                *tail = sh;
                tail = &sh->next;
            }
        }
    }
    fclose(fp);
    *doc_out = doc;
    return 0;
}

void svg_free_document(SvgDocument *doc) {
    SvgShape *cur = doc->shapes;
    while (cur) {
        SvgShape *next = cur->next;
        free(cur);
        cur = next;
    }
    free(doc);
}