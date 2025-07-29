#ifndef __WIN_UTILS_H__
#define __WIN_UTILS_H__

struct WinImage {
  int width;
  int height;
};

struct WinColor {
  unsigned char r, g, b, a;
};

void SetClickThrough(bool enable);
void SetWindowShapeFromTexture(const WinImage &image, const WinColor *pixels);

#endif // __WIN_UTILS_H__