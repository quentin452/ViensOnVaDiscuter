#pragma once
#include <raylib.h>

class DesktopCompanion {
public:
  DesktopCompanion();
  ~DesktopCompanion();

  void Initialize();

  // Ajouter deltaTime comme paramètre
  void Update(float deltaTime);

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

  float timeSinceLastChange;
  float timeSinceLastVelocityChange;

  float moveAccumulator;
  float lastMouseCheckTime;
  float mouseCheckInterval;
  bool lastOpaqueState;

  Vector2 renderOffset;
  Vector2 companionPos;

  void HandleMovement(float deltaTime);

  void HandleDragging(const bool *mouseOnOpaque);

  void HandleMousePassthrough(const bool *mouseOnOpaque);

  bool IsMouseOnOpaquePixel() const;
};
