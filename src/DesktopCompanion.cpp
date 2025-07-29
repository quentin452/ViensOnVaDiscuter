#include "DesktopCompanion.h"
#include "Config.h"
#include "Utils.h"
#include <stdlib.h>
#include <time.h>

DesktopCompanion::DesktopCompanion()
    : windowPos({0, 0}), dragOffset({0, 0}), velocity({2.0f, 1.5f}), dragging(false), passthrough(false),
      desktopWidth(0), desktopHeight(0), pixels(nullptr) {
  srand((unsigned int)time(NULL));
}

DesktopCompanion::~DesktopCompanion() {
  if (pixels) {
    UnloadImageColors(pixels);
  }
  UnloadTexture(companionTex);
  UnloadImage(companionImg);

  TraceLog(LOG_INFO, "Application closed.");
}

void DesktopCompanion::Initialize() {
  SetConfigFlags(FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TOPMOST | FLAG_VSYNC_HINT);

  InitWindow(WIN_WIDTH, WIN_HEIGHT, "ViensOnVaDiscuter");
  SetWindowOpacity(1.0f);

  companionImg = LoadImage(CHARACTER_TEXTURE_PATH);
  ImageFormat(&companionImg, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
  companionTex = LoadTextureFromImage(companionImg);

  Image icon = LoadImage(ICON_PATH);
  SetWindowIcon(icon);
  UnloadImage(icon);

  GetDesktopDimensions(&desktopWidth, &desktopHeight);

  pixels = LoadImageColors(companionImg);

  CenterWindow();
}

void DesktopCompanion::CenterWindow() {
  int monitorWidth = GetMonitorWidth(0);
  int monitorHeight = GetMonitorHeight(0);
  windowPos.x = (monitorWidth - WIN_WIDTH) / 2;
  windowPos.y = (monitorHeight - WIN_HEIGHT) / 2;
  SetWindowPosition(windowPos.x, windowPos.y);
}

void DesktopCompanion::Update() {
  HandleMousePassthrough();
  HandleDragging();

  if (!dragging) {
    HandleMovement();
  }
}

void DesktopCompanion::HandleMousePassthrough() {
  Vector2 mouse = GetMousePosition();
  bool windowFocused = IsWindowFocused();

  if (windowFocused) {
    ClearWindowState(FLAG_WINDOW_MOUSE_PASSTHROUGH);
  }

  bool mouseOnOpaque = false;
  if (windowFocused && mouse.x >= 0 && mouse.x < companionImg.width && mouse.y >= 0 && mouse.y < companionImg.height) {
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
}

void DesktopCompanion::HandleDragging() {
  Vector2 mouse = GetMousePosition();
  bool windowFocused = IsWindowFocused();

  bool mouseOnOpaque = false;
  if (windowFocused && mouse.x >= 0 && mouse.x < companionImg.width && mouse.y >= 0 && mouse.y < companionImg.height) {
    Color px = pixels[(int)mouse.y * companionImg.width + (int)mouse.x];
    mouseOnOpaque = px.a > 0;
  }

  if (!dragging && mouseOnOpaque && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    Vector2 winPos = GetWindowPosition();
    dragOffset = mouse;
    windowPos = winPos;
    dragging = true;
  }

  if (dragging && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
    dragging = false;
    windowPos = GetWindowPosition();
  }

  if (dragging && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
    Vector2 winPos = GetWindowPosition();
    int mouseScreenX = GetMouseX() + winPos.x;
    int mouseScreenY = GetMouseY() + winPos.y;
    SetWindowPosition(mouseScreenX - dragOffset.x, mouseScreenY - dragOffset.y);
  }
}

void DesktopCompanion::HandleMovement() {
  float deltaTime = GetFrameTime();

  windowPos.x += velocity.x * deltaTime * 60.0f;
  windowPos.y += velocity.y * deltaTime * 60.0f;

  static float timeSinceLastChange = 0.0f;
  timeSinceLastChange += deltaTime;

  if (windowPos.x < 0 || windowPos.x > desktopWidth - WIN_WIDTH) {
    velocity.x = -velocity.x + ((rand() % 3) - 1);
  }
  if (windowPos.y < 0 || windowPos.y > desktopHeight - WIN_HEIGHT) {
    velocity.y = -velocity.y + ((rand() % 3) - 1);
  }

  if (velocity.x > 5)
    velocity.x = 5;
  if (velocity.x < -5)
    velocity.x = -5;
  if (velocity.y > 5)
    velocity.y = 5;
  if (velocity.y < -5)
    velocity.y = -5;

  if (timeSinceLastChange >= 2.0f) {
    velocity.x += (rand() % 3) - 1;
    velocity.y += (rand() % 3) - 1;
    timeSinceLastChange = 0.0f;
  }

  SetWindowPosition((int)windowPos.x, (int)windowPos.y);
}

void DesktopCompanion::Draw() {
  BeginDrawing();
  ClearBackground(BLANK);
  DrawTexture(companionTex, 0, 0, WHITE);
  EndDrawing();
}

bool DesktopCompanion::ShouldClose() const { return WindowShouldClose(); }
