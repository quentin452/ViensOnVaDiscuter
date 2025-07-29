#define WIN32_LEAN_AND_MEAN
#include <WinUtils.h>
#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
void *GetWindowHandle(void);
#ifdef __cplusplus
}
#endif

#ifdef _WIN32
void SetClickThrough(bool enable) {
  HWND hwnd = (HWND)GetWindowHandle();
  if (!hwnd)
    return;

  LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);

  if (enable) {
    exStyle |= WS_EX_TRANSPARENT;
  } else {
    exStyle &= ~WS_EX_TRANSPARENT;
  }

  SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);

  SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

void SetWindowShapeFromTexture(const WinImage &image, const WinColor *pixels) {
  HRGN hRegion = CreateRectRgn(0, 0, 0, 0); // Région vide initiale

  for (int y = 0; y < image.height; y++) {
    int startX = -1;
    for (int x = 0; x <= image.width; x++) {
      bool isOpaque = false;
      if (x < image.width) {
        WinColor px = pixels[y * image.width + x];
        isOpaque = px.a > 0;
      }

      if (isOpaque && startX < 0) {
        startX = x;
      } else if (!isOpaque && startX >= 0) {
        HRGN temp = CreateRectRgn(startX, y, x, y + 1);
        CombineRgn(hRegion, hRegion, temp, RGN_OR);
        DeleteObject(temp);
        startX = -1;
      }
    }
  }

  HWND hwnd = (HWND)GetWindowHandle();
  if (hwnd) {
    SetWindowRgn(hwnd, hRegion, TRUE);
  } else {
    DeleteObject(hRegion);
  }
}
#else
void SetClickThrough(bool) {
  // TODO SUPPORT MORE PLATFORMS
}
#endif