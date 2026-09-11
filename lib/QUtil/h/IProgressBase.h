/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#pragma once

// #include "GeometryExports.h"

class QString;

// class GEOMETRY_EXPORT  IProgressBase
class IProgressBase {
public:
  IProgressBase();
  virtual ~IProgressBase();

  // set the total number of steps
  virtual void AddSteps(int nSteps);

  // increment
  virtual void Step(int nSteps = 1);

  // set the title of the progress indicator
  virtual void StatusMessage(const QString &sMessage);

  virtual void SetProgress(int nCurrent);

  //
  // only for dual
  //
  virtual void NextJob(const QString &secondaryTitle);

  virtual void Enable(bool bEnable);
};