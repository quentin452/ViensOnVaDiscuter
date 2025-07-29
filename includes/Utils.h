#pragma once
#include <cstdarg>

void InitLogging();

void FileLogCallback(int logType, const char *text, va_list args);

void GetDesktopDimensions(int *width, int *height);