
#include "IDualProgressBase.h"

IDualProgressBase::IDualProgressBase() : CProgressBase() {}

IDualProgressBase::~IDualProgressBase() {}

void IDualProgressBase::AddSteps(int steps) { CProgressBase::AddSteps(steps); }

void IDualProgressBase::Step(int steps) { CProgressBase::Step(steps); }

void IDualProgressBase::StatusMessage(const QString &message) { CProgressBase::StatusMessage(message); }

void IDualProgressBase::NextJob(const QString & /*secondaryTitle*/) {}
