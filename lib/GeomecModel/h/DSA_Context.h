#pragma once

#include <QDateTime>

#include "IDCAnalysisStatus.h" //DianaCore
#include "AnalysisType.h" //GeomecModel

//OIV
class COpenInventorSceneNode;
class OIV2DWellPathGraphScene;
class OIV2DHistoryScene;

//run
class CAnalysisType;

class CDianaRunController;
class IExecuteDianaDialog;
class IDianaXWrapper;
class ISaveModel;
class IRetrieveDianaFileNames;


struct DSA_Context
{
  enum eType{ LicenseCheck, Regular };

  // input
  struct input_
  {
    CAnalysisType::TAnalysisType ana_type;
    bool is_write_input_files;
    bool is_write_quad_dat;
  } input;

  struct more_
  {
    QString path;
    std::string title;
    bool silent;
    eType type;
  } more;

  struct out_
  {
    // references
    bool is_success;
    dia::IAnalysisStatusContainer statuses;

    bool save_succeed;
    std::string path;
  } out;

  // scenes
  struct scene_
  {
    COpenInventorSceneNode* main;
    OIV2DHistoryScene* history;
    OIV2DWellPathGraphScene* wellpath;
  } scene;

  // For logs	
  struct time_
  {
    QDateTime start;

    struct save_
    {
      QDateTime start;
      QDateTime end;
    } save;
  } time;

  // drc: Diana Run Controller
  CDianaRunController* drc;
  IExecuteDianaDialog* dlg;
  IDianaXWrapper* dxw;
  ISaveModel* save;
  IRetrieveDianaFileNames* retrieve;

  DSA_Context();
  ~DSA_Context();
};

