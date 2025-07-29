#include "raylib.h"
#include <cstdarg>
#include <stdio.h>
#include <stdlib.h> // pour rand()
#include <time.h>   // pour srand()

const int WIN_WIDTH = 400;
const int WIN_HEIGHT = 300;

Image companionImg;
Texture2D companionTex;

Vector2 windowPos = {0, 0};
Vector2 dragOffset = {0, 0};
Vector2 velocity = {2.0f, 1.5f}; // vitesse initiale
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

  srand((unsigned int)time(NULL));

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

  int desktopWidth = 0, desktopHeight = 0;
  int monitorCount = GetMonitorCount();
  for (int i = 0; i < monitorCount; i++) {
    int monX = GetMonitorPosition(i).x;
    int monY = GetMonitorPosition(i).y;
    int monW = GetMonitorWidth(i);
    int monH = GetMonitorHeight(i);
    if (monX + monW > desktopWidth)
      desktopWidth = monX + monW;
    if (monY + monH > desktopHeight)
      desktopHeight = monY + monH;
  }

  float deltaTime = 0.0f;
  while (!WindowShouldClose()) {
    deltaTime = GetFrameTime();

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
      windowPos = GetWindowPosition(); // <-- Synchronise ici !
    }

    if (dragging && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
      Vector2 winPos = GetWindowPosition();
      int mouseScreenX = GetMouseX() + winPos.x;
      int mouseScreenY = GetMouseY() + winPos.y;
      SetWindowPosition(mouseScreenX - dragOffset.x,
                        mouseScreenY - dragOffset.y);
    } else {
      windowPos.x += velocity.x * deltaTime * 60.0f;
      windowPos.y += velocity.y * deltaTime * 60.0f;

      // Rebonds sur les bords du bureau (tous écrans)
      if (windowPos.x < 0 || windowPos.x > desktopWidth - WIN_WIDTH) {
        velocity.x = -velocity.x + ((rand() % 3) - 1); // rebond + variation
      }
      if (windowPos.y < 0 || windowPos.y > desktopHeight - WIN_HEIGHT) {
        velocity.y = -velocity.y + ((rand() % 3) - 1);
      }

      // Limite la vitesse
      if (velocity.x > 5)
        velocity.x = 5;
      if (velocity.x < -5)
        velocity.x = -5;
      if (velocity.y > 5)
        velocity.y = 5;
      if (velocity.y < -5)
        velocity.y = -5;

      if (rand() % 120 == 0) { // toutes les ~2 secondes
        velocity.x += (rand() % 3) - 1;
        velocity.y += (rand() % 3) - 1;
      }
      SetWindowPosition((int)windowPos.x, (int)windowPos.y);
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