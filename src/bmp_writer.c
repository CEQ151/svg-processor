/*=====================================================================
  bmp_writer.c  –  24-bit BMP（无压缩）
  =====================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include "bmp_writer.h"

#pragma pack(push,1)
typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BITMAPFILEHEADER;

typedef struct {
    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPelsPerMeter;
    int32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BITMAPINFOHEADER;
#pragma pack(pop)

int bmp_write(const char *filename, const uint8_t *pixels,
              int width, int height) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) return -1;

    int row_padded = (width * 3 + 3) & ~3;
    int image_size = row_padded * height;

    BITMAPFILEHEADER bfh = {0};
    bfh.bfType = 0x4D42;               // "BM"
    bfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + image_size;
    bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    BITMAPINFOHEADER bih = {0};
    bih.biSize = sizeof(BITMAPINFOHEADER);
    bih.biWidth = width;
    bih.biHeight = -height;            // top-down
    bih.biPlanes = 1;
    bih.biBitCount = 24;
    bih.biCompression = 0;
    bih.biSizeImage = image_size;

    fwrite(&bfh, sizeof(bfh), 1, fp);
    fwrite(&bih, sizeof(bih), 1, fp);

    uint8_t pad[3] = {0,0,0};
    for (int y = 0; y < height; ++y) {
        const uint8_t *row = pixels + y * width * 3;
        fwrite(row, 3 * width, 1, fp);
        fwrite(pad, 1, row_padded - width*3, fp);
    }
    fclose(fp);
    return 0;
}