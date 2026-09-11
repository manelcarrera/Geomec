#include "stdafx.h"

#include "IT_Command_Task.h"

#include "GeomecPythonInterface.h" //FIXME: Dependecies
#include "SafeQueue.h"

#include "IGm.h"
#include "IGmIT.h"
#include "IGmSur.h"

#include "GenData.h"

#include "Depletion.h"
#include "Displacement.h"
#include "IT_Data.h"
#include "IT_Material.h"
#include "IT_MaterialParameter.h"
#include "List.h"
#include "Reservoir.h"

#include "Printer.h"

#include <chrono>

namespace {

gm::model::formation::IT::reservoir::TReservoirPropertyV properties_v2() {
  gm::model::formation::IT::reservoir::TReservoirPropertyV properties_v = {
      gm::model::formation::IT::reservoir::eReservoirProperty::Thickness,
      gm::model::formation::IT::reservoir::eReservoirProperty::PoissonsRatio,
      gm::model::formation::IT::reservoir::eReservoirProperty::YoungsModulus,
      gm::model::formation::IT::reservoir::eReservoirProperty::PorePressure};
  return properties_v;
}

gm::TDisplacementComponentV components_v2() {
  gm::TDisplacementComponentV component_v = {gm::eComponent::X, gm::eComponent::Y, gm::eComponent::Z};
  return component_v;
}

enum eCmdType { Request_v1, Request_v2, Set, Unknown };

eCmdType cmd_type(Command cmd) {
  switch (cmd.cmd()) {
  case Command::IT1_List_Surface:
  case Command::IT1_List_Reservoir:
  case Command::IT1_List_Measured_Displacement:
  case Command::IT1_List_Depletion:
  case Command::IT1_Get_Reservoir:
  case Command::IT1_Get_Displacement:
  case Command::IT1_Get_Measured_Displacement:
    return Request_v1;

  case Command::Model_Elems:
  case Command::Model_Points:
  case Command::Model_Prop_Form:
  case Command::Model_Prop_Mat:
  case Command::Model_Prop_Dep:

  case Command::Res_Available:
  case Command::Res_Values:

  case Command::DB_Points:
  case Command::DB_Values:
  case Command::DB_Elems: // elems_id
  case Command::DB_PointSet_Values:
  case Command::DB_PointSet_ValuesList:
  case Command::DB_Material_List:
  case Command::DB_Material_GetValues:
  case Command::Gen_List:
    return Request_v2;

  case Command::IT1_Set_Depletion:
  case Command::IT1_Exe_Run_Geomec:
  case Command::IT1_Exe_Run_Geomec_Step:
  case Command::IT1_Exe_Wait_End_Run_Geomec:
  case Command::IT1_Exe_Save_Displacement:
  case Command::IT1_Exe_Remove_Depletion:

  case Command::Model_Dep_Add:
  case Command::Model_Dep_Remove:
  case Command::Model_Dep_Output:

  case Command::DB_PointSet_Create:
  case Command::DB_PointSet_Remove:
  case Command::DB_PointSet_AddValues:
  case Command::DB_PointSet_Link:
  case Command::DB_PointSet_Unlink:
  case Command::DB_PointSet_RemoveValues:
  case Command::DB_PointSet_ReplaceValues:

  case Command::DB_Material_Create:
  case Command::DB_Material_Remove:
  case Command::DB_Material_SetValues:
  case Command::DB_Material_Link:
  case Command::DB_Material_Unlink:
    return Set;

  default:
    Printer::instance()->debug("IT_Command_task : cmd_type() : cmd:[%s] : CASE DEFAULT", cmd.name().c_str());
    break;
  };
  return Unknown;
}

IT_Data *data_v1(Command *cmd) {
  IT_Data::TIntV p = cmd->data() ? cmd->data()->params() : IT_Data::TIntV{};

  IT_Data *data = nullptr;
  switch (cmd->cmd()) {
  case Command::IT1_List_Surface: {
    data = gm::list(gm::eList::L_Surface, p[0]);
    break;
  } // [ res_idx ]
  case Command::IT1_List_Reservoir: {
    data = gm::list(gm::eList::L_Reservoir);
    break;
  }
  case Command::IT1_List_Measured_Displacement: {
    data = gm::list(gm::eList::L_BD_Composite, gm::eComposite::C_Displacement);
    break;
  }
  case Command::IT1_List_Depletion: {
    data = gm::list(gm::eList::L_Depletion);
    break;
  }

  case Command::IT1_Get_Reservoir: {
    data = gm::model::formation::IT::reservoir::get(p[0], p[1], properties_v2());
    break;
  } // [ res_idx, sur_idx ]
  case Command::IT1_Get_Displacement: {
    data = gm::res::composite::IT::displacement::get(p[0], p[1], p[2], p[3], components_v2());
    break;
  } // [ res_idx, sur_idx, depletion_id, dis_idx, meas_dis_idx ]
  case Command::IT1_Get_Measured_Displacement: {
    data = gm::bd::composite::IT::displacement::get(p[0], p[1], p[2], components_v2());
    break;
  } // [ res_idx, meas_dis_idx, sur_idx ]
  /* NOT USED */
  case Command::IT1_Get_Material_Parameter: {
    data = gm::bd::material::surface::param(p[0], p[1], (gm::eReservoirPropMat)p[2], p[3]);
    break;
  } // [ res_idx, 0, mat_param_id, sur_idx ]
  default:
    Printer::instance()->error("IT_Command_task : data_v1() : cmd:[%s] : CASE DEFAULT", cmd->name().c_str());
    break;
  }
  return data;
}
IT_Data *data_v2(Command *cmd) {
  IT_Data::TIntV p = cmd->data() ? cmd->data()->params() : IT_Data::TIntV{};

  IT_Data *data = nullptr;
  switch (cmd->cmd()) {
  case Command::Model_Elems: {
    data = new GenData_3i(gm::model::formation::IT::elems(p[0], p[1]));
    break;
  } // [ res, sur ]
  case Command::Model_Points: {
    data = new GenData_3d(gm::model::formation::surface::points(p[0], p[1]));
    break;
  } // [ res, sur ]
  case Command::Model_Prop_Form: {
    data = new GenData_1d(gm::model::formation::surface::properties::get(p[0], p[1], (gm::eReservoirPropForm)p[2]));
    break;
  } // [ res, sur, prop ]
  case Command::Model_Prop_Mat: {
    data =
        new GenData_1d(gm::model::formation::surface::properties::get(p[0], p[1], p[2], (gm::eReservoirPropMat)p[3]));
    break;
  } // [ res, sur, dep, prop ]
  case Command::Model_Prop_Dep: {
    data =
        new GenData_1d(gm::model::formation::surface::properties::get(p[0], p[1], p[2], (gm::eReservoirPropDep)p[3]));
    break;
  } // [ res, sur, dep, prop ]

  // case Command::Res_Values:{		data = new GenData_1d( gm::res::composite::component::IT::values( p[0], p[1], p[2],
  // p[3], (gm::eComposite)p[4], (gm::eComponent)p[5] ) ); break; } // [ res, sur, dep, bd_composite_idx, composite,
  // component ]
  //  points from PS not mapped to the surface
  case Command::Res_Available: {
    data = new GenData_1i(IT_Data::TIntV{gm::res::available() ? 1 : 0});
    break;
  }
  case Command::Res_Values: {
    data =
        new GenData_1d(gm::res::composite::component::values((gm::eComposite)p[0], (gm::eComponent)p[1], p[2], p[3]));
    break;
  } // [ composite, component, bd_composite_idx, dep ]

  case Command::DB_Points: {
    data = new GenData_3d(gm::bd::composite::points((gm::eComposite)p[0], p[1]));
    break;
  } // [ composite, composite_idx ]
  case Command::DB_Values: {
    data = new GenData_1d(gm::bd::composite::component::values((gm::eComposite)p[0], p[1], (gm::eComponent)p[2]));
    break;
  } // [ composite, composite_idx, component ]
  case Command::DB_Elems: {
    data = new GenData_1i(gm::bd::composite::IT::elems(p[0], p[1], (gm::eComposite)p[2], p[3]));
    break;
  } // elems_id -> [ res, sur, composite, composite_idx ]

  case Command::DB_PointSet_Values: {
    data = new GenData_1d(gm::bd::pointset::values(p[0], p[1]));
    break;
  } // [ ps_id, values_id ]
  case Command::DB_PointSet_ValuesList: {
    data = new GenData_1i(gm::bd::pointset::values_list(p[0]));
    break;
  } // [ ps_id ]

  case Command::DB_Material_List: {
    data = gm::bd::material::list();
    break;
  }
  case Command::DB_Material_GetValues: {
    data = new GenData_1d(gm::bd::material::values(p[0]));
    break;
  } // [ idx ]

  case Command::Gen_List: {
    data = gm::list((gm::eList)p[0], p[1]);
    break;
  } // [ type, idx ]
  default:
    Printer::instance()->error("IT_Command_task : data_v2() : cmd:[%s] : CASE DEFAULT", cmd->name().c_str());
    break;
  }
  return data;
}

void exe(Command &cmd) {
  IT_Data::TIntV p = cmd.data() ? cmd.data()->params() : IT_Data::TIntV{};

  switch (cmd.cmd()) // [ SETS / COMMANDS ]
  {
  // v1
  case Command::IT1_Set_Depletion: {
    gm::model::depletion::IT::pressure::add(p[0], (gm::eDepletionType)p[1], p[2], p[3] == 1,
                                            static_cast<Depletion *>(cmd.data()));
    break;
  }
  case Command::IT1_Exe_Run_Geomec:
    gm::model::run();
    break;
  case Command::IT1_Exe_Run_Geomec_Step:
    gm::model::run_step();
    break;
  case Command::IT1_Exe_Wait_End_Run_Geomec:
    gm::model::wait_end_run();
    break;
  case Command::IT1_Exe_Save_Displacement:
    break;
  case Command::IT1_Exe_Remove_Depletion: {
    gm::model::depletion::remove();
    break; /*LAST*/
  }

  // v2
  case Command::DB_PointSet_Create: {
    GenData_3d *d = static_cast<GenData_3d *>(cmd.data());
    gm::bd::pointset::create(p[0], d->values()); //( id ) (points)
    break;
  }
  case Command::DB_PointSet_Remove: {
    gm::bd::pointset::remove(p[0]);
    break;
  }
  case Command::DB_PointSet_AddValues: {
    GenData_1d *d = static_cast<GenData_1d *>(cmd.data());
    gm::bd::pointset::add_values(p[0], (gm::eDepletionType)p[1], d->values()); //( id, depletion_type ) (values)
    break;
  }
  case Command::DB_PointSet_RemoveValues: {
    gm::bd::pointset::remove_vals(p[0], p[1]);
    break;
  } // (ps_id, values_idx)
  case Command::DB_PointSet_ReplaceValues: {
    GenData_1d *d = static_cast<GenData_1d *>(cmd.data());
    gm::bd::pointset::replace_values(p[0], d->values(), p[1]); //( id, pos ) (values)
    break;
  }
  case Command::DB_PointSet_Link:
  case Command::DB_PointSet_Unlink: {
    gm::bd::pointset::link(p[0], -1, p[1], p[2], cmd.cmd() == Command::DB_PointSet_Link);
    break;
  } //-1: values_id
  case Command::Model_Dep_Add: {
    gm::model::depletion::add(p[1] == 1);
    break;
  } // ( output )
  case Command::Model_Dep_Remove: {
    gm::model::depletion::remove(p[1]);
    break;
  } // ( id )
  case Command::Model_Dep_Output: {
    gm::model::depletion::output(p[1], p[1] == 1);
    break;
  } // ( id, output )

  case Command::DB_Material_List: {
    gm::bd::material::list();
    break;
  }
  case Command::DB_Material_Create: {
    gm::bd::material::create(p[0]);
    break;
  }
  case Command::DB_Material_Remove: {
    gm::bd::material::remove(p[0]);
    break;
  }
  case Command::DB_Material_SetValues: {
    gm::bd::material::values(p[0], static_cast<IT::Material *>(cmd.data()));
    break;
  }
  case Command::DB_Material_Link: {
    gm::bd::material::link(p[0], p[1]);
    break;
  } // ( idx, res_id )
  case Command::DB_Material_Unlink: {
    gm::bd::material::unlink(p[0], p[1]);
    break;
  } // (idx, res_id)

  default:
    Printer::instance()->error("IT_Command_task : exe() : cmd:[%s] : CASE DEFAULT", cmd.name().c_str());
    break;
  }
}
} // namespace

IT_Command_Task::IT_Command_Task(GeomecPythonInterface *gm_iface_, Command _cmd, HWND hWnd_)
    : gm_iface(gm_iface_), cmd(_cmd), hWnd(hWnd_) {}

tbb::task *IT_Command_Task::execute() {
  if (gm::model::get()) {
    Printer::instance()->debug("Req : [%s] Treating (in a separated thread)", cmd.name().c_str());
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    IT_Data::TIntV p = cmd.data() ? cmd.data()->params() : IT_Data::TIntV{};
    Printer::instance()->debug("Params: %s", gm::format(p).c_str());

    // IT_Data::TIntV p = cmd.data() ? cmd.data()->params() : IT_Data::TIntV{};
    IT_Data *data = nullptr;

    eCmdType type_ = cmd_type(cmd);
    switch (type_) {
    case Request_v1:
    case Request_v2: {
      data = type_ == Request_v1 ? data_v1(&cmd) : data_v2(&cmd);
      if (cmd.data()) // delete of memory allocated with the new performed in the request (to send the parameters)
      {
        delete cmd.data();
        cmd.data(nullptr);
      }
      cmd.data(data);
      break;
    }
    case Set: {
      exe(cmd);
      // remove 'set' / 'do' type commands data
      // for 'get' type command removal is performed after building the python tuple
      if (cmd.data()) {
        delete cmd.data();
        cmd.data(nullptr);
      }
      break;
    }
    default:
      Printer::instance()->error("IT_Command_task : execute() : cmd:[%s] : CASE DEFAULT", cmd.name().c_str());
    };

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    Printer::instance()->debug("Req: %s | Treated: %.2fs.", cmd.name().c_str(),
                               std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / 1000.0);
    Printer::instance()->debug("res -> C++_res_Q");

    gm_iface->results_queue().enqueue(cmd);
    // PostMessage( hWnd, WM_POP_FOO, 0, 0 );			// Notify GUI thread that result is available.
  } else {
    cmd.data(nullptr);
    gm_iface->results_queue().enqueue(cmd);
  }
  gm_iface->m_processing = false;
  return NULL;
}

void Launch_IT_Command_Task(GeomecPythonInterface *gm_iface, Command _cmd, HWND _hWnd) {
  IT_Command_Task *t = new (tbb::task::allocate_root()) IT_Command_Task(gm_iface, _cmd, _hWnd);
  tbb::task::enqueue(*t);
}

void contiguous_data_TESTS() {
  // FIXME: remove
  /*int cols=3;
  TValuesTemp<double,cols> values;
  values.v[0]=1.1;*/

  /*double a[] = {1,2,3};
  Data::TValuesSet values( 3, a );

  double b[] = {4,5,6};
  Data::TValuesSet values2( 4, b );

  Data::TValuesSetV values_v = { values, values2 };

  double v[6];

  memcpy( v, values_v.data(), 6*sizeof( double ) );
  Printer::instance()->debug("%.f %.f %.f %.f %.f %.f",v[0],v[1],v[2],v[3],v[4],v[5]);

  typedef struct { double p1; double p2; double p3; } TTriF;
  std::vector< TTriF > tri_v = { {1,2,3}, {7,8,9} };
  memcpy( v, tri_v.data(), 6*sizeof( double ) );
  Printer::instance()->debug("%.f %.f %.f %.f %.f %.f",v[0],v[1],v[2],v[3],v[4],v[5]);

  int size = sizeof( values );
  int size2 = sizeof( TTriF );

  typedef struct { double p1; double p2; double p3; double p4; } TTetraF;

  union UValues
  {
    TTriF tri;
    TTetraF tetra;
  };

  UValues vals;
  vals.tetra = TTetraF{ 1,2,3,4 };

  UValues vals2;
  vals2.tetra = TTetraF{ 9,10,11,12 };

  std::vector< UValues > union_v={vals,vals2};

  memcpy( v, union_v.data(), 6*sizeof( double ) );
  Printer::instance()->debug("%.f %.f %.f %.f %.f %.f",v[0],v[1],v[2],v[3],v[4],v[5]);*/

  /*typedef struct { double v1; }									T1Value;
  typedef struct { double v1; double v2; }						T2Values;
  typedef struct { double v1; double v2; double v3; }				T3Values;
  typedef struct { double v1; double v2; double v3; double v4; }	T4Values;

  union TValuesU
  {
    T1Value v1;
    T2Values v2;
    T3Values v3;
    T4Values v4;
  };

  TValuesU vals;
  vals.v2 = T2Values{ 1.,2. };

  TValuesU vals2;
  vals2.v2 = T2Values{ 3.,4. };

  std::vector< TValuesU > union_v={vals,vals2};

  double v[ 4 ];
  memcpy( v, union_v.data(), 4*sizeof( double ) );
  Printer::instance()->debug("%.f %.f %.f %.f",v[0],v[1],v[2],v[3]);*/

  /*class d
  {
    d(){}
    double data[2];
    void add(double _d)
  };*/
}
