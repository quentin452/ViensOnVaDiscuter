#define WIN32_LEAN_AND_MEAN

#include <raylib.h>

#include <Utils.h>

#include <Config.h>

#include <stdio.h>

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
