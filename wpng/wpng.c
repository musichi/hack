/* Bare-bones png writer. Just uncompressed rgba png, no frills. Build like:
     clang -o wpng wpng.c -Wall
*/

#include <stdint.h>
#include <stdio.h>

typedef struct { FILE* f; uint32_t crc_table[256]; uint32_t crc; } pngblock;

void pngblock_write(const void* d, int len, pngblock* b) {
  fwrite(d, 1, len, b->f);
  for (int n = 0; n < len; n++)
    b->crc = b->crc_table[(b->crc ^ ((uint8_t*)d)[n]) & 0xff] ^ (b->crc >> 8);
}

void pngblock_putu32_be(uint32_t u, pngblock* b) {
  uint8_t d[] = { u >> 24, u >> 16, u >> 8, u }; pngblock_write(d, 4, b);
}

void pngblock_start(pngblock* b, uint32_t size, const char* tag) {
  pngblock_putu32_be(size, b);
  b->crc = 0xffffffff;
  pngblock_write(tag, 4, b);
}

void pngblock_putu32_le(uint32_t u, pngblock* b) {
  uint8_t d[] = { u, u >> 8, u >> 16, u >> 24 }; pngblock_write(d, 4, b);
}

void wpng(int w, int h, const uint8_t* pix, FILE* f) {  // pix: rgba in memory
  // http://www.libpng.org/pub/png/spec/1.2/PNG-Contents.html
  pngblock b = { f };
  for (int n = 0; n < 256; n++) {
    uint32_t c = n;
    for (int k = 0; k < 8; k++)
      c = (c & 1) ? 0xedb88320L ^ (c >> 1) : c >> 1;
    b.crc_table[n] = c;
  }

  fwrite("\x89PNG\r\n\x1a\n", 1, 8, f);
  pngblock_start(&b, 13, "IHDR");  // size: IHDR has two uint32 + 5 bytes = 13
  pngblock_putu32_be(w, &b);
  pngblock_putu32_be(h, &b);
  pngblock_write("\x8\6\0\0\0", 5, &b);  // 8bpp rgba, default flags
  pngblock_putu32_be(b.crc ^ 0xffffffff, &b);  // IHDR crc32

  uint16_t data_size = w*4 + 1;  // image data + one filter byte per scanline
  pngblock_start(&b, 6 + (5 + data_size)*h, "IDAT");
  pngblock_write("\x8\x1d", 2, &b);  // deflate data, in one single block
  uint32_t a1 = 1, a2 = 0;
  for (int y = 0; y < h; ++y) {
    pngblock_write(y == h - 1 ? "\1" : "\0", 1, &b);
    pngblock_putu32_le(data_size + (~data_size << 16), &b);
    pngblock_write(&b.crc_table, 1, &b);  // filter for scanline (0: no filter)
    pngblock_write(pix + y*4*w, 4*w, &b);
    const int BASE = 65521;  // largest prime smaller than 65536
    a2 = (a1 + a2) % BASE;
    for (int n = 0; n < 4*w; n++) {
      a1 = (a1 + pix[y*4*w + n]) % BASE;
      a2 = (a1 + a2) % BASE;
    }
  }
  pngblock_putu32_be((a2 << 16) + a1, &b);  // adler32 of uncompressed data
  pngblock_putu32_be(b.crc ^ 0xffffffff, &b);  // IDAT crc32

  pngblock_start(&b, 0, "IEND");
  pngblock_putu32_be(b.crc ^ 0xffffffff, &b);  // IEND crc32
}

int main() {
  uint8_t pix[256*125*4];
  for (size_t i = 0; i < sizeof(pix); ++i) pix[i] = i*i;
  wpng(125, 256, pix, stdout);
}