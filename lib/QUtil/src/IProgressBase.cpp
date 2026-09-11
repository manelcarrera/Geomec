 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */

#include "IProgressBase.h"

#include "Printer.h"

namespace
{
  Printer* printer = Printer::instance(Printer::RunP);
}

IProgressBase::IProgressBase(){}
IProgressBase::~IProgressBase(){}
void IProgressBase::AddSteps(int /*nSteps*/){ printer->error("IProgressBase::AddSteps -> NOT IMPLEMENTED"); }
void IProgressBase::Step(int /*nSteps*/){ printer->error("IProgressBase::Step -> NOT IMPLEMENTED"); }
void IProgressBase::StatusMessage(const QString& /*sMessage*/){ printer->error("IProgressBase::StatusMessage -> NOT IMPLEMENTED"); }
void IProgressBase::SetProgress(int /*nCurrent*/){ printer->error("IProgressBase::SetProgress -> NOT IMPLEMENTED"); }
void IProgressBase::Enable(bool /* bEnable */){ printer->error("IProgressBase::Enable -> NOT IMPLEMENTED"); }
//
//only for dual
//
void IProgressBase::NextJob(const QString& /*secondaryTitle*/){ printer->error("IProgressBase::NextJob -> NOT IMPLEMENTED"); }
