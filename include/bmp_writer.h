#include <cstdint>
#ifndef BMP_WRITER_H
#define BMP_WRITER_H

int bmp_write(const char *filename, const uint8_t *pixels,
              int width, int height);

#endif /* BMP_WRITER_H */