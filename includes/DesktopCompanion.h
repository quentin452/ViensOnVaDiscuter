#pragma once
#include "raylib.h"

class DesktopCompanion {
public:
  DesktopCompanion();
  ~DesktopCompanion();

  void Initialize();

  void Update();

  void Draw();

  void CenterWindow();

  bool ShouldClose() const;

private:
  Image companionImg;
  Texture2D companionTex;
  Color *pixels;

  Vector2 windowPos;
  Vector2 dragOffset;
  Vector2 velocity;
  bool dragging;
  bool passthrough;

  int desktopWidth;
  int desktopHeight;

  void HandleMovement();

  void HandleDragging();

  void HandleMousePassthrough();
};
