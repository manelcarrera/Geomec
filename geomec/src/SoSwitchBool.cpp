#include "stdafx.h"
#include "SoSwitchBool.h"

#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/nodes/SoSwitch.h>

SO_ENGINE_SOURCE(SoSwitchBool);

void SoSwitchBool::evaluate()
{
  int value = on.getValue() ? SO_SWITCH_ALL : SO_SWITCH_NONE;
  SO_ENGINE_OUTPUT(whichChild, SoSFInt32, setValue(value));
}

void SoSwitchBool::initClass()
{
  SO_ENGINE_INIT_CLASS(SoSwitchBool, SoEngine, "Engine");
}

void SoSwitchBool::exitClass()
{
  SO_ENGINE_EXIT_CLASS(SoSwitchBool);
}

SoSwitchBool::SoSwitchBool()
{
  SO_ENGINE_CONSTRUCTOR(SoSwitchBool);

  SO_ENGINE_ADD_INPUT(on, (false));

  SO_ENGINE_ADD_OUTPUT(whichChild, SoSFInt32);
}

SoSwitchBool::~SoSwitchBool()
{
}