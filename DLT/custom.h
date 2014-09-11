#pragma once

void printFloatv(int mode, char *title);
void normalize_3D(float original[][3], float processed[][3], size_t n, float param[3 + 1]);
void normalize_2D(int original[][2], float processed[][2], size_t n, float param[2 + 1]);