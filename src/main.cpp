#include "raylib.h"
#include <cstdarg>
#include <stdio.h>

const int WIN_WIDTH = 400;
const int WIN_HEIGHT = 300;

Image companionImg;
Texture2D companionTex;

Vector2 windowPos = {0, 0};
Vector2 dragOffset = {0, 0};
bool dragging = false;

void FileLogCallback(int logType, const char *text, va_list args) {
  FILE *logFile = fopen("viens.log", "a");
  if (logFile) {
    vfprintf(logFile, text, args);
    fprintf(logFile, "\n");
    fclose(logFile);
  }
}

void CenterWindow() {
  int monitorWidth = GetMonitorWidth(0);
  int monitorHeight = GetMonitorHeight(0);
  windowPos.x = (monitorWidth - WIN_WIDTH) / 2;
  windowPos.y = (monitorHeight - WIN_HEIGHT) / 2;
  SetWindowPosition(windowPos.x, windowPos.y);
}

int main() {
  FILE *logFile = fopen("viens.log", "w");
  if (logFile)
    fclose(logFile);

  SetTraceLogLevel(LOG_ALL);
  SetTraceLogCallback(FileLogCallback);
  SetConfigFlags(FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_UNDECORATED |
                 FLAG_WINDOW_TOPMOST | FLAG_VSYNC_HINT);
  InitWindow(WIN_WIDTH, WIN_HEIGHT, "ViensOnVaDiscuter");

  companionImg = LoadImage("assets/textures/character.png");
  ImageFormat(&companionImg, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
  companionTex = LoadTextureFromImage(companionImg);

  Image icon = LoadImage("assets/exe/icon.png");
  SetWindowIcon(icon);
  UnloadImage(icon);

  SetWindowOpacity(1.0f);

  CenterWindow();

  Color *pixels = LoadImageColors(companionImg);

  bool passthrough = false;

  while (!WindowShouldClose()) {
    Vector2 mouse = GetMousePosition();
    bool windowFocused = IsWindowFocused();
    if (windowFocused) {
      ClearWindowState(FLAG_WINDOW_MOUSE_PASSTHROUGH);
    }

    bool mouseOnOpaque = false;
    if (windowFocused && mouse.x >= 0 && mouse.x < companionImg.width &&
        mouse.y >= 0 && mouse.y < companionImg.height) {
      Color px = pixels[(int)mouse.y * companionImg.width + (int)mouse.x];
      mouseOnOpaque = px.a > 0;
    }

    if (!dragging) {
      if (mouseOnOpaque && passthrough) {
        ClearWindowState(FLAG_WINDOW_MOUSE_PASSTHROUGH);
        passthrough = false;
      } else if (!mouseOnOpaque && !passthrough) {
        SetWindowState(FLAG_WINDOW_MOUSE_PASSTHROUGH);
        passthrough = true;
      }
    }

    // Drag
    if (!dragging && mouseOnOpaque && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      Vector2 winPos = GetWindowPosition();
      dragOffset = mouse;
      windowPos = winPos;
      dragging = true;
    }

    if (dragging && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
      dragging = false;
    }

    if (dragging && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
      Vector2 winPos = GetWindowPosition();
      int mouseScreenX = GetMouseX() + winPos.x;
      int mouseScreenY = GetMouseY() + winPos.y;
      SetWindowPosition(mouseScreenX - dragOffset.x,
                        mouseScreenY - dragOffset.y);
    }

    BeginDrawing();
    ClearBackground(BLANK);
    DrawTexture(companionTex, 0, 0, WHITE);
    EndDrawing();
  }

  // Nettoyage
  UnloadTexture(companionTex);
  UnloadImageColors(pixels);
  UnloadImage(companionImg);
  CloseWindow();
  TraceLog(LOG_INFO, "Application closed.");
  return 0;
}