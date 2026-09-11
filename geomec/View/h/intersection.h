#pragma once
#include <vector>

#include "Inventor\SbVec.h"

bool selfIntersects(const std::vector<SbVec2f> &polygon);