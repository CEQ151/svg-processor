/*=====================================================================
  main.c  –  命令行解析、调用 parser / render / export
  =====================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "svg_parser.h"
#include "svg_render.h"
#include "bmp_writer.h"

/* stb_image_write (single header) */
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

static void usage(const char *prog) {
    printf("Usage:\n");
    printf("  %s --parse|-p <input.svg>\n", prog);
    printf("  %s --export_bmp|-eb <input.svg> <output.bmp>\n", prog);
    printf("  %s --export_jpg|-ej <input.svg> <output.jpg>\n", prog);
}

int main(int argc, char *argv[]) {
    if (argc < 3) { usage(argv[0]); return 1; }

    const char *cmd = argv[1];
    const char *in  = argv[2];

    SvgDocument *doc = NULL;
    if (svg_load_from_file(in, &doc) != 0) {
        fprintf(stderr, "Failed to load SVG: %s\n", in);
        return 1;
    }

    if (strcmp(cmd, "--parse") == 0 || strcmp(cmd, "-p") == 0) {
        svg_print_summary(doc);
        svg_print_shapes(doc);
    }
    else if (strcmp(cmd, "--export_bmp") == 0 || strcmp(cmd, "-eb") == 0) {
        if (argc != 4) { usage(argv[0]); svg_free_document(doc); return 1; }
        int w, h;
        uint8_t *pix = svg_rasterize(doc, &w, &h);
        if (bmp_write(argv[3], pix, w, h) != 0)
            fprintf(stderr, "BMP write failed\n");
        else
            printf("BMP exported: %s (%dx%d)\n", argv[3], w, h);
        free(pix);
    }
    else if (strcmp(cmd, "--export_jpg") == 0 || strcmp(cmd, "-ej") == 0) {
        if (argc != 4) { usage(argv[0]); svg_free_document(doc); return 1; }
        int w, h;
        uint8_t *pix = svg_rasterize(doc, &w, &h);
        if (stbi_write_jpg(argv[3], w, h, 3, pix, 90) == 0)
            fprintf(stderr, "JPG write failed\n");
        else
            printf("JPG exported: %s (%dx%d)\n", argv[3], w, h);
        free(pix);
    }
    else {
        usage(argv[0]);
    }

    svg_free_document(doc);
    return 0;
}