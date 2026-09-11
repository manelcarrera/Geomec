#pragma once

class IDeformation {
public:
  IDeformation() {};

  virtual void Invalidate() = 0;
};