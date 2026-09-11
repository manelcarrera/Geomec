#pragma once
#include "Point.h"
#include <vector>

// Abstract class to define actions that take a list of world coordinates
class CWorldAction {
public:
  virtual ~CWorldAction() = 0;

  virtual void operator()(const std::vector<geo::CPoint> &) = 0;
};