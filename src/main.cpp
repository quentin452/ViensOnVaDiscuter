#include "raylib.h"

const int WIN_WIDTH = 400;
const int WIN_HEIGHT = 300;

Vector2 windowPos = {0, 0};
Vector2 dragOffset = {0, 0};
bool dragging = false;

void CenterWindow()
{
    int monitorWidth = GetMonitorWidth(0);
    int monitorHeight = GetMonitorHeight(0);
    windowPos.x = (monitorWidth - WIN_WIDTH) / 2;
    windowPos.y = (monitorHeight - WIN_HEIGHT) / 2;
    SetWindowPosition(windowPos.x, windowPos.y);
}

void DrawCompanion()
{
    ClearBackground(BLANK);
    
    DrawEllipse(190, 110, 90, 30, WHITE);
    DrawEllipseLines(190, 110, 90, 30, BLACK);
    
    DrawTriangle(
        {220, 140}, 
        {190, 170}, 
        {240, 140}, 
        WHITE
    );
    DrawTriangleLines(
        {220, 140}, 
        {190, 170}, 
        {240, 140}, 
        BLACK
    );
    
    DrawText("Viens on va discuter!", 110, 100, 16, BLACK); 
    
    DrawCircle(200, 200, 30, Color{100, 150, 255, 255});
    DrawCircleLines(200, 200, 30, Color{50, 100, 200, 255});
    
    DrawCircle(193, 198, 5, BLACK);  // oeil gauche
    DrawCircle(207, 198, 5, BLACK);  // oeil droit
    
    DrawLineEx({185, 210}, {200, 215}, 2, Color{50, 100, 200, 255});
    DrawLineEx({200, 215}, {215, 210}, 2, Color{50, 100, 200, 255});
}

int main()
{
    SetConfigFlags(FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TOPMOST | FLAG_WINDOW_MOUSE_PASSTHROUGH);
    InitWindow(WIN_WIDTH, WIN_HEIGHT, "ViensOnVaDiscuter");
    
    Image icon = LoadImage("assets/exe/icon.png"); 
    SetWindowIcon(icon);
    UnloadImage(icon);
    
    SetTargetFPS(60);
    
    SetWindowOpacity(1.0f);
    
    CenterWindow();
    
    ClearWindowState(FLAG_WINDOW_MOUSE_PASSTHROUGH);
    
    while (!WindowShouldClose())
    {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Vector2 mousePos = GetMousePosition();
            dragOffset.x = mousePos.x;
            dragOffset.y = mousePos.y;
            dragging = true;
        }
        
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            dragging = false;
        }
        
        if (dragging && IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            Vector2 mousePos = GetMousePosition();
            Vector2 mouseDelta = {mousePos.x - dragOffset.x, mousePos.y - dragOffset.y};
            
            windowPos.x += mouseDelta.x;
            windowPos.y += mouseDelta.y;
            SetWindowPosition(windowPos.x, windowPos.y);
            
            dragOffset = mousePos;
        }
        
        BeginDrawing();
        DrawCompanion();
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}