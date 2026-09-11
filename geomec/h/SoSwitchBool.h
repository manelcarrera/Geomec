#ifndef SOSWITCHBOOL_H_INCLUDED
#define SOSWITCHBOOL_H_INCLUDED

#include <Inventor/engines/SoSubEngine.h>
#include <Inventor/fields/SoSFBool.h>

class SoSwitchBool : public SoEngine {
  SO_ENGINE_HEADER(SoSwitchBool);

  virtual void evaluate();

public:
  static void initClass();
  static void exitClass();

  SoSwitchBool();
  ~SoSwitchBool();

  // Input field
  SoSFBool on;

  // Output
  SoEngineOutput whichChild; // (SoSFInt32) whichChild input for a SoSwitch
};

#endif
