// DCasingTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "..\DCasing\DCasing.h"
#include "CasSignalReciever.h"
#include <iostream>

int main(int argc, char *argv[]) {
  CDCasing myObj;
  CDCasingGeneralParameters &Parameters = myObj.GetGeneralParameters();

  // just a dummy reciever all strings are send to cout
  CCasingSignalReciever rc(&myObj);

  // connect reciever to casing object
  bool ret = myObj.connect(&myObj, SIGNAL(OnMessage(QString)), &rc, SLOT(OnMessage(QString)));
  ret = myObj.connect(&myObj, SIGNAL(OnError(QString)), &rc, SLOT(OnError(QString)));
  ret = myObj.connect(&myObj, SIGNAL(OnWarning(QString)), &rc, SLOT(OnWarning(QString)));
  ret = myObj.connect(&myObj, SIGNAL(OnFinished()), &rc, SLOT(OnFinished()));

  // fill casing object with bogus data

  Parameters.PipeLength().SetValue(2.7432);
  Parameters.OuterDiameter().SetValue(0.1778);
  Parameters.WallThickness().SetValue(0.013716);
  Parameters.JointSpacing().SetValue(10.16);
  Parameters.RequiredDrifting().SetValue(0.15);
  Parameters.DriftingDiameter().SetValue(0.1778);
  Parameters.ModelOutDiamBoreDiamRatio().SetValue(100);

  Parameters.SteelYieldStrain().SetValue(0.00329);
  Parameters.SteelPoissonsRatio().SetValue(0.3);
  Parameters.LinearCoeffStressStrain().SetValue(1.0);
  Parameters.PowerCoeffStressStrain().SetValue(0.35);
  Parameters.HardeningCoeffStressStrain().SetValue(0.06);

  Parameters.SteelPipeYoungsModulus().SetValue(210000);
  Parameters.PipeFrictionCoeff().SetValue(0.1);

  Parameters.CementYoungsModulus().SetValue(20000);
  Parameters.CementPoissonRatio().SetValue(0.2);
  Parameters.MaxAxialCompStrain().SetValue(0.12);

  Parameters.DeformationTheory("DEFOR");
  Parameters.ModelLateralBoundary("FIXED");
  Parameters.NumberOfLoadSteps(100);
  Parameters.MaximumWaveNumber(2);
  Parameters.MaxAxialHalfWaves(200);

  // create casing points and fill with bogus data
  for (long counter = 0; counter < 8; counter++) {

    CDCasingPoint &Point = *new CDCasingPoint(/*&myObj*/);
    myObj.AddPoint(&Point);

    if (counter < 4) {
      Point.ElasticSupportYoungsMod().SetValue(5000);
      Point.ElasticSupportPoissonRatio().SetValue(.15);
    } else {
      Point.ElasticSupportYoungsMod().SetValue(15000);
      Point.ElasticSupportPoissonRatio().SetValue(.25);
    }

    Point.InternalPressure().SetValue(15 + (.25 * counter));
    Point.ExternalPressure().SetValue(20 + (.25 * counter));
    Point.Epscom().SetValue(0.02);
  }

  // set diana-path and other paths..
  myObj.SetDianaPath("V:\\");
  myObj.SetDianaSharePath("U:\\");
  myObj.SetWorkingDir("C:\\temp\\casing");

  // start analysis and recieve signals
  myObj.StartAnalysis();

  return 1;
}
