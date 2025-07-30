#define WIN32_LEAN_AND_MEAN

#include <raylib.h>

#include <Utils.h>

#include <Config.h>

#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void InitLogging() {
  FILE *logFile = fopen(LOG_FILE_PATH, "w");
  if (logFile)
    fclose(logFile);

  SetTraceLogLevel(LOG_ALL);
  SetTraceLogCallback(FileLogCallback);
}

void FileLogCallback(int logType, const char *text, va_list args) {
  FILE *logFile = fopen(LOG_FILE_PATH, "a");
  if (logFile) {
    vfprintf(logFile, text, args);
    fprintf(logFile, "\n");
    fclose(logFile);
  }
}

void GetDesktopDimensions(int *width, int *height) {
  *width = 0;
  *height = 0;
  int monitorCount = GetMonitorCount();

  for (int i = 0; i < monitorCount; i++) {
    int monX = GetMonitorPosition(i).x;
    int monY = GetMonitorPosition(i).y;
    int monW = GetMonitorWidth(i);
    int monH = GetMonitorHeight(i);

    if (monX + monW > *width)
      *width = monX + monW;

    if (monY + monH > *height)
      *height = monY + monH;
  }
}

// Fonctions simplifiées pour le mode DVD uniquement
// Ces fonctions ne sont plus nécessaires dans le mode DVD simple

void ApplyEdgeAttraction(float *velocityX, float *velocityY, float posX, float posY, int screenWidth,
                         int screenHeight) {
  // Obtenir le mode DVD depuis la configuration
  bool dvdMode = GetConfigBool("DVD_MODE", DVD_MODE);

  if (!dvdMode) {
    return; // Si DVD_MODE est désactivé, pas d'attraction
  }

  // En mode DVD, on utilise une vitesse constante et des rebonds simples
  // Cette fonction sera appelée pour les rebonds aux bords dans HandleMovement
  // On n'applique pas d'attraction continue ici, juste des ajustements de vitesse

  float dvdSpeed = GetConfigFloat("DVD_SPEED", DVD_SPEED);

  // Normaliser la vitesse actuelle au DVD_SPEED configuré
  float currentSpeed = sqrt((*velocityX) * (*velocityX) + (*velocityY) * (*velocityY));

  if (currentSpeed > 0.1f) { // Éviter la division par zéro
    float speedRatio = dvdSpeed / currentSpeed;
    *velocityX *= speedRatio;
    *velocityY *= speedRatio;
  } else {
    // Si la vitesse est nulle, donner une direction aléatoire avec la vitesse DVD
    float angle = (rand() % 360) * 3.14159f / 180.0f;
    *velocityX = cos(angle) * dvdSpeed;
    *velocityY = sin(angle) * dvdSpeed;
  }
}
// Variables globales pour stocker la configuration DVD
static float dvdSpeed = DVD_SPEED;
static bool dvdMode = DVD_MODE;
static bool configLoaded = false;

void LoadConfig() {
  FILE *configFile = nullptr;

  const char *configPaths[] = {
      "companion.config",
  };

  for (size_t i = 0; i < sizeof(configPaths) / sizeof(configPaths[0]); i++) {
    configFile = fopen(configPaths[i], "r");
    if (configFile != nullptr) {
      TraceLog(LOG_INFO, "Found configuration file at: %s", configPaths[i]);
      break;
    }
  }

  if (configFile == nullptr) {
    TraceLog(LOG_INFO, "Configuration file not found, creating companion.config with default values");

    // Créer le fichier de configuration avec les valeurs par défaut (mode DVD)
    configFile = fopen("companion.config", "w");
    if (configFile != nullptr) {
      fprintf(configFile, "# Configuration du comportement du compagnon de bureau\n");
      fprintf(configFile, "# Ce fichier permet de personnaliser le comportement de mouvement\n\n");

      fprintf(configFile, "# Mode DVD Logo - le compagnon rebondit sur les bords comme un logo DVD (true/false)\n");
      fprintf(configFile, "# true = mouvement DVD style avec rebonds sur tous les bords\n");
      fprintf(configFile, "# false = mouvement libre sans rebonds automatiques\n");
      fprintf(configFile, "DVD_MODE=true\n\n");

      fprintf(configFile, "# Vitesse de base du compagnon en mode DVD (pixels par seconde)\n");
      fprintf(configFile, "DVD_SPEED=80.0\n\n");

      fclose(configFile);

      // Initialiser les valeurs par défaut
      dvdMode = true;
      dvdSpeed = 80.0f;

      configLoaded = true;
      TraceLog(LOG_INFO, "Default configuration file created successfully");
      return;
    }
    TraceLog(LOG_ERROR, "Failed to create configuration file, using default values");
    dvdMode = DVD_MODE;
    dvdSpeed = DVD_SPEED;
    configLoaded = true;
    return;
  }

  const int LINE_SIZE = 256;
  char line[LINE_SIZE];
  while (fgets(line, sizeof(line), configFile) != nullptr) {
    // Ignorer les commentaires et lignes vides
    if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
      continue;
    }

    // Parser les lignes de configuration
    const int KEY_SIZE = 64;
    char key[KEY_SIZE];
    char value[KEY_SIZE];
    if (sscanf(line, "%63[^=]=%63s", key, value) == 2) {
      // Stocker seulement les valeurs DVD
      if (strcmp(key, "DVD_MODE") == 0) {
        dvdMode = (strcmp(value, "true") == 0);
      } else if (strcmp(key, "DVD_SPEED") == 0) {
        dvdSpeed = static_cast<float>(atof(value));
      }
    }
  }

  fclose(configFile);
  configLoaded = true;

  // Log des valeurs chargées pour vérification
  TraceLog(LOG_INFO, "Configuration loaded from companion.config");
  TraceLog(LOG_INFO, "DVD_MODE: %s", dvdMode ? "true" : "false");
  TraceLog(LOG_INFO, "DVD_SPEED: %.2f", dvdSpeed);
}

float GetConfigFloat(const char *name, float defaultValue) {
  if (!configLoaded) {
    LoadConfig();
  }

  if (strcmp(name, "DVD_SPEED") == 0) {
    return dvdSpeed;
  }

  TraceLog(LOG_WARNING, "Config value '%s' not found, using default: %.2f", name, defaultValue);
  return defaultValue;
}

bool GetConfigBool(const char *name, bool defaultValue) {
  if (!configLoaded) {
    LoadConfig();
  }

  if (strcmp(name, "DVD_MODE") == 0) {
    return dvdMode;
  }

  TraceLog(LOG_WARNING, "Config boolean '%s' not found, using default: %s", name, defaultValue ? "true" : "false");
  return defaultValue;
}

// Pas de fonction GetConfigInt nécessaire pour le mode DVD simple
