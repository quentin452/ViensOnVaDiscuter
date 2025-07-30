#pragma once
#include <cstdarg>

void InitLogging();

void FileLogCallback(int logType, const char *text, va_list args);

void GetDesktopDimensions(int *width, int *height);

// Fonctions pour le comportement d'attraction vers les bords
bool ShouldPreferEdges();
float GetDistanceFromNearestEdge(float x, float y, int screenWidth, int screenHeight);
bool IsInEdgeZone(float x, float y, int screenWidth, int screenHeight);
void ApplyEdgeAttraction(float *velocityX, float *velocityY, float posX, float posY, int screenWidth, int screenHeight);

// Fonctions pour le comportement d'attraction vers les coins/extrémités
bool ShouldPreferCorners();
float GetDistanceFromNearestCorner(float x, float y, int screenWidth, int screenHeight);
bool IsInCornerZone(float x, float y, int screenWidth, int screenHeight);
// Configuration dynamique
void LoadConfig();
float GetConfigFloat(const char *name, float defaultValue);
int GetConfigInt(const char *name, int defaultValue);
bool GetConfigBool(const char *name, bool defaultValue);