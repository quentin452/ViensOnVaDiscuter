#include <Config.h>
#include <DesktopCompanion.h>
#include <Utils.h>
#include <WinUtils.h>
#include <cmath>
#include <stdlib.h>
#include <time.h>

DesktopCompanion::DesktopCompanion()
    : windowPos({0, 0}), dragOffset({0, 0}), velocity({2.0f, 1.5f}), dragging(false), passthrough(false),
      desktopWidth(0), desktopHeight(0), pixels(nullptr), timeSinceLastVelocityChange(0.0f), moveAccumulator(0.0f) {
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
  SetConfigFlags(FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TOPMOST);

  InitWindow(WIN_WIDTH, WIN_HEIGHT, "ViensOnVaDiscuter");
  SetWindowOpacity(1.0f);

  LoadConfig();

  companionImg = LoadImage(CHARACTER_TEXTURE_PATH);
  ImageFormat(&companionImg, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
  companionTex = LoadTextureFromImage(companionImg);

  Image icon = LoadImage(ICON_PATH);
  SetWindowIcon(icon);
  UnloadImage(icon);

  GetDesktopDimensions(&desktopWidth, &desktopHeight);

  pixels = LoadImageColors(companionImg);

  WinImage wimg;
  wimg.width = companionImg.width;
  wimg.height = companionImg.height;

  int pixelCount = companionImg.width * companionImg.height;
  WinColor *wpixels = new WinColor[pixelCount];
  for (int i = 0; i < pixelCount; ++i) {
    wpixels[i].r = pixels[i].r;
    wpixels[i].g = pixels[i].g;
    wpixels[i].b = pixels[i].b;
    wpixels[i].a = pixels[i].a;
  }

  SetWindowShapeFromTexture(wimg, wpixels);

  delete[] wpixels;

  CenterWindow();
}

void DesktopCompanion::CenterWindow() {
  int monitorWidth = GetMonitorWidth(0);
  int monitorHeight = GetMonitorHeight(0);
  windowPos.x = (monitorWidth - WIN_WIDTH) / 2;
  windowPos.y = (monitorHeight - WIN_HEIGHT) / 2;
  SetWindowPosition(windowPos.x, windowPos.y);
}

bool DesktopCompanion::IsMouseOnOpaquePixel() const {
  Vector2 mouse = GetMousePosition();
  if (mouse.x >= 0 && mouse.x < companionImg.width && mouse.y >= 0 && mouse.y < companionImg.height) {
    Color px = pixels[(int)mouse.y * companionImg.width + (int)mouse.x];
    return px.a > 0;
  }
  return false;
}

void DesktopCompanion::Update(float deltaTime) {
  const bool mouseOnOpaque = IsMouseOnOpaquePixel();
  HandleMousePassthrough(&mouseOnOpaque);
  HandleDragging(&mouseOnOpaque);

  if (!dragging) {
    HandleMovement(deltaTime);
  }
}

void DesktopCompanion::HandleMousePassthrough(const bool *mouseOnOpaque) {
  if (!dragging) {
    if (*mouseOnOpaque) {
      if (passthrough) {
        SetClickThrough(false);
        passthrough = false;
      }
    } else {
      if (!passthrough) {
        SetClickThrough(true);
        passthrough = true;
      }
    }
  } else {
    SetClickThrough(false);
    passthrough = false;
  }
}
void DesktopCompanion::HandleDragging(const bool *mouseOnOpaque) {
  Vector2 mouse = GetMousePosition();

  if (!dragging && *mouseOnOpaque && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
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
    static Vector2 lastWindowPos = {-1, -1};
    Vector2 newPos = {mouseScreenX - dragOffset.x, mouseScreenY - dragOffset.y};
    if ((int)newPos.x != (int)lastWindowPos.x || (int)newPos.y != (int)lastWindowPos.y) {
      SetWindowPosition((int)newPos.x, (int)newPos.y);
      lastWindowPos = newPos;
    }
  }
}

void DesktopCompanion::HandleMovement(float deltaTime) {
  // Vérifier si le mode DVD est activé
  bool dvdMode = GetConfigBool("DVD_MODE", true);

  if (dvdMode) {
    // Mode DVD Logo - mouvement simple avec rebonds
    float speed = GetConfigFloat("DVD_SPEED", 80.0f);

    // Déplacer le compagnon
    windowPos.x += velocity.x * speed * deltaTime;
    windowPos.y += velocity.y * speed * deltaTime;

    // Vérifier les collisions avec les bords et faire rebondir
    bool bounced = false;

    // Rebond horizontal
    if (windowPos.x <= 0) {
      windowPos.x = 0;
      velocity.x = 1.0f; // Rebondir vers la droite
      bounced = true;
    } else if (windowPos.x >= desktopWidth - WIN_WIDTH) {
      windowPos.x = desktopWidth - WIN_WIDTH;
      velocity.x = -1.0f; // Rebondir vers la gauche
      bounced = true;
    }

    // Rebond vertical
    if (windowPos.y <= 0) {
      windowPos.y = 0;
      velocity.y = 1.0f; // Rebondir vers le bas
      bounced = true;
    } else if (windowPos.y >= desktopHeight - WIN_HEIGHT) {
      windowPos.y = desktopHeight - WIN_HEIGHT;
      velocity.y = -1.0f; // Rebondir vers le haut
      bounced = true;
    }

    // Normaliser la vélocité pour garder une vitesse constante
    if (!bounced) {
      float length = sqrtf(velocity.x * velocity.x + velocity.y * velocity.y);
      if (length > 0) {
        velocity.x /= length;
        velocity.y /= length;
      }
    }

  } else {
    // Mode attraction/répulsion original
    float baseSpeed = GetConfigFloat("BASE_MOVEMENT_SPEED", 60.0f);

    windowPos.x += velocity.x * deltaTime * baseSpeed;
    windowPos.y += velocity.y * deltaTime * baseSpeed;

    timeSinceLastVelocityChange += deltaTime;

    // Gérer les collisions avec les bords et corriger la position
    bool bounced = false;

    // Collision avec les bords horizontaux
    if (windowPos.x < 0) {
      windowPos.x = 0;                                        // Forcer la position dans les limites
      velocity.x = std::abs(velocity.x) + ((rand() % 3) - 1); // Assurer une vitesse positive
      bounced = true;
    } else if (windowPos.x > desktopWidth - WIN_WIDTH) {
      windowPos.x = desktopWidth - WIN_WIDTH;                  // Forcer la position dans les limites
      velocity.x = -std::abs(velocity.x) + ((rand() % 3) - 1); // Assurer une vitesse négative
      bounced = true;
    }

    // Collision avec les bords verticaux
    if (windowPos.y < 0) {
      windowPos.y = 0;                                        // Forcer la position dans les limites
      velocity.y = std::abs(velocity.y) + ((rand() % 3) - 1); // Assurer une vitesse positive
      bounced = true;
    } else if (windowPos.y > desktopHeight - WIN_HEIGHT) {
      windowPos.y = desktopHeight - WIN_HEIGHT;                // Forcer la position dans les limites
      velocity.y = -std::abs(velocity.y) + ((rand() % 3) - 1); // Assurer une vitesse négative
      bounced = true;
    }

    // Assurer une vitesse minimale après rebond pour éviter les blocages
    if (bounced) {
      if (std::abs(velocity.x) < 1.0f) {
        velocity.x = (velocity.x >= 0) ? 1.0f : -1.0f;
      }
      if (std::abs(velocity.y) < 1.0f) {
        velocity.y = (velocity.y >= 0) ? 1.0f : -1.0f;
      }
    }

    // Limiter la vitesse maximale
    if (velocity.x > 5)
      velocity.x = 5;
    if (velocity.x < -5)
      velocity.x = -5;
    if (velocity.y > 5)
      velocity.y = 5;
    if (velocity.y < -5)
      velocity.y = -5;

    // Changement de direction aléatoire périodique basé sur la configuration
    float directionChangeInterval = GetConfigFloat("DIRECTION_CHANGE_INTERVAL", 2.0f);
    if (timeSinceLastVelocityChange >= directionChangeInterval) {
      velocity.x += (rand() % 3) - 1;
      velocity.y += (rand() % 3) - 1;
      timeSinceLastVelocityChange = 0.0f;

      // Appliquer l'attraction vers les bords selon la configuration
      ApplyEdgeAttraction(&velocity.x, &velocity.y, windowPos.x, windowPos.y, desktopWidth, desktopHeight);

      // Assurer qu'on n'a pas une vitesse nulle
      if (std::abs(velocity.x) < 0.5f) {
        velocity.x = (rand() % 2 == 0) ? 1.0f : -1.0f;
      }
      if (std::abs(velocity.y) < 0.5f) {
        velocity.y = (rand() % 2 == 0) ? 1.0f : -1.0f;
      }
    }
  }

  // Mettre à jour la position de la fenêtre seulement si elle a changé
  static Vector2 lastWindowPos = {-1, -1};
  if ((int)windowPos.x != (int)lastWindowPos.x || (int)windowPos.y != (int)lastWindowPos.y) {
    SetWindowPosition((int)windowPos.x, (int)windowPos.y);
    lastWindowPos = windowPos;
  }
}
void DesktopCompanion::Draw() {
  BeginDrawing();
  ClearBackground(BLANK);
  DrawTexture(companionTex, 0, 0, WHITE);
  EndDrawing();
}

bool DesktopCompanion::ShouldClose() const { return WindowShouldClose(); }
