#include "GeomecPythonInterface.h"

#include "SafeQueue.h"
#include "IT_Command.h"	// Command -> int, PyObject*
#include "Printer.h"
#include "PyWrapper.h"
#include "Depletion.h"
#include "IT_Material.h"
#include "GenData.h"

// before: Py_BEGIN_ALLOW_THREADS, Py_END_ALLOW_THREADS, METH_VARARGS
// now: just for structs, not functions
#include "PyInc.h" 

#include "PyLibHandler.h" // pyd::Py_BEGIN_ALLOW_THREADS, pyd::Py_END_ALLOW_THREADS

typedef PyWrapper wrapper;

SafeQueue< Command > GeomecPythonInterface::m_requests_queue;
SafeQueue< Command > GeomecPythonInterface::m_results_queue;

const char* GeomecPythonInterface::C_MODULE = "geomec";

PyMethodDef GeomecPythonInterface::GeomecMethods[] =
{
  { "send_cmd",	GeomecPythonInterface::send_cmd,	METH_VARARGS,	"Pass a request to Geomec"		},
  { "get_result",	GeomecPythonInterface::get_result,	METH_VARARGS,	"Returns the cmd result."		},
  { NULL,	NULL, 0, NULL }
};

//----------------------------------------------------------------------------------- set

namespace{


  IT_Data::TPointsV transform_3d( const TData& points_a )
  {
    IT_Data::TValuesV points_vx((double*)points_a.data, (double*)points_a.data + points_a.rows);
    IT_Data::TValuesV points_vy((double*)points_a.data + points_a.rows, (double*)points_a.data + 2 * points_a.rows);
    IT_Data::TValuesV points_vz((double*)points_a.data + 2 * points_a.rows, (double*)points_a.data + 3 * points_a.rows);

    IT_Data::TPointsV points_v(points_a.rows);
    for (int i = 0; i < points_a.rows; ++i)
    {
      points_v[i].x = points_vx[i];
      points_v[i].y = points_vy[i];
      points_v[i].z = points_vz[i];
    }
    return points_v;
  }

  IT_Data::TValuesV transform_1d( const TData& vals_a )
  {
    //IT_Data::TValuesV vals_v((double*)vals_a.data, (double*)vals_a.data + vals_a.rows);
    IT_Data::TValuesV vals_v(vals_a.rows, 0);
    for (int i = 0; i < vals_a.rows; ++i)
      vals_v[i] = *((double *)vals_a.data + 4 * i);
    return vals_v;
  }

  IT_Data::TValuesV transform_1d_bis( TData& vals_a )
  {
    IT_Data::TValuesV vals_v((double*)vals_a.data, (double*)vals_a.data + vals_a.rows);
    return vals_v;
  }

  IT_Data::TIntV transform_1i( TData& vals_a ) // TODO: verify
  {
    IT_Data::TIntV vals_v(vals_a.rows, 0);
    for (int i = 0; i < vals_a.rows; ++i)
      vals_v[i] = *((int *)vals_a.data + 4 * i);
    return vals_v;
  }

  IT_Data::TIntV transform_1i_bis( TData& vals_a ) // TODO: verify
  {
    IT_Data::TIntV vals_v((int*)vals_a.data, (int*)vals_a.data + vals_a.rows);
    return vals_v;
  }

  IT_Data::TIntV _parse_params_2( TPyObjectPtrV& py_params_v ) //FIXME: make 'transform_1i' ???
  {
    IT_Data::TIntV params;
    TPyObjectPtrV data_input_v	= wrapper::parse< TPyObjectPtrV >( py_params_v[ IT_Data::ePyTuplePos::Params ] );
    for( int i=0; i < data_input_v.size(); i++ )
    {
      int param = wrapper::parse_obj<int>( data_input_v[ i ] );
      params.push_back( param );
    }
    return params;
  }

  IT_Data::TIntV params( TPyObjectPtrV obj_v ) //FIXME: make 'transform_1i' ???
  {
    IT_Data::TIntV params;
    for( int i=0; i < obj_v.size(); i++ )
      params.push_back( wrapper::parse_obj<int>( obj_v[ i ] ) );
    return params;
  }

  Depletion* depletion( TPyObjectPtrV data_v, TPyObjectPtrV params_v )
  {
    TData points_a		= wrapper::parse_obj<TData>( data_v[ Depletion::eData::Points ] );
    TData pressures_a	= wrapper::parse_obj<TData>( data_v[ Depletion::eData::Values ] );

    IT_Data::TPointsV points_v		= transform_3d( points_a ); 
    IT_Data::TValuesV pressures_v	= transform_1d( pressures_a );

    Depletion* depletion = new Depletion();

    depletion->points(		points_v );
    depletion->pressure(	pressures_v );

    depletion->params( params( params_v ) );

    return depletion;
  }

  IT::Material* material( TPyObjectPtrV data_v, TPyObjectPtrV params_v )
  {
    TData ids_a		= wrapper::parse_obj<TData>( data_v[ IT::Material::eData::Ids ] );
    TData values_a	= wrapper::parse_obj<TData>( data_v[ IT::Material::eData::Values ] );

    IT_Data::TIntV ids_v		= transform_1i_bis( ids_a ); 
    IT_Data::TValuesV values_v	= transform_1d_bis( values_a );

    IT::Material* material = new IT::Material();

    material->ids(		ids_v );
    material->values(	values_v );

    material->params( params( params_v ) );

    return material;
  }

  GenData_3d* gen_3d( PyObject* data_py, TPyObjectPtrV params_v )
  {
    GenData_3d* data = new GenData_3d( transform_3d( wrapper::parse_obj<TData>( data_py ) ), params( params_v ) );
    return data;
  }
  GenData_1d* gen_1d( PyObject* data_py, TPyObjectPtrV params_v )
  {
    GenData_1d* data = new GenData_1d( transform_1d( wrapper::parse_obj<TData>( data_py ) ), params( params_v ) );
    return data;
  }
}

namespace{

  enum eData{ Cmd_, Cmd_Params, Cmd_Params_Data };
  eData type_( Command::eType cmd_id )
  {
    switch( cmd_id )
    {
      case Command::IT1_Control_Quit:
      case Command::IT1_List_Reservoir:
      case Command::IT1_List_Depletion:
      case Command::IT1_List_Measured_Displacement:
      case Command::IT1_Exe_Run_Geomec:
      case Command::IT1_Exe_Run_Geomec_Step:
      case Command::IT1_Exe_Wait_End_Run_Geomec:
      case Command::Control_Ini:
      case Command::Control_End:
      case Command::Res_Available:
      case Command::DB_Material_List:
        return Cmd_;
      case Command::IT1_Set_Depletion:				// points and values
      case Command::DB_PointSet_Create:				// points
      case Command::DB_PointSet_AddValues:			// values
      case Command::DB_PointSet_ReplaceValues:		// values
      case Command::DB_Material_SetValues:			// material model params
        return Cmd_Params_Data;
      default:
        return Cmd_Params;
    }
  }
}

// FORMAT: ( cmd_id, ([p1], [p2],...[pn] ), (data,[data2]) )
PyObject* GeomecPythonInterface::send_cmd( PyObject *self, PyObject *args )
{
  // ceval.h
  /*
  #define Py_BEGIN_ALLOW_THREADS { \
            PyThreadState *_save; \
            _save = PyEval_SaveThread();
#define Py_BLOCK_THREADS        PyEval_RestoreThread(_save);
#define Py_UNBLOCK_THREADS      _save = PyEval_SaveThread();
#define Py_END_ALLOW_THREADS    PyEval_RestoreThread(_save); \
                 }*/

  //pyd::Py_BEGIN_ALLOW_THREADS

  {
    PyThreadState *_save;
    _save = pyd::PyEval_SaveThread();


  Command cmd;
  
  TPyObjectPtrV cmd_v = wrapper::parse< TPyObjectPtrV >( args );

  Command::eType cmd_id	= (Command::eType)wrapper::parse_obj<int>( cmd_v[ IT_Data::ePyTuplePos::Cmd_ ] );
  
  TPyObjectPtrV params_v	= cmd_v.size() > 1 ? wrapper::parse< TPyObjectPtrV >( cmd_v[ IT_Data::ePyTuplePos::Params ] ) : TPyObjectPtrV{};
  
  TPyObjectPtrV data_v = cmd_v.size() > 2 ? wrapper::parse< TPyObjectPtrV >( cmd_v[ IT_Data::ePyTuplePos::Data_ ] ) : TPyObjectPtrV{}; // depletion -> 2x obj, phase II -> only 1x obj

  switch( type_( cmd_id ) )
  {
    case Cmd_:			cmd = Command( cmd_id, nullptr ); break;
    case Cmd_Params:	cmd = Command( cmd_id, new IT_Data( params( params_v ) ) ); break;
    case Cmd_Params_Data:
    {
      IT_Data* data = nullptr;
      switch( cmd_id )
      {
        case Command::IT1_Set_Depletion:			data = depletion(	data_v,		params_v ); break; //Depletion
        //Phase II
        case Command::DB_PointSet_Create:			data = gen_3d(		data_v[0],	params_v ); break;

        case Command::DB_PointSet_AddValues:	
        case Command::DB_PointSet_ReplaceValues:	data = gen_1d(		data_v[0],	params_v ); break;

        case Command::DB_Material_SetValues:		data = material(	data_v,		params_v ); break; //FIXME
        default:									Printer::instance()->debug("GeomecPythonInterface : send_cmd : data gen TO IMPLEMENT : cmd:[%s]", cmd.name().c_str() ); break;	
      }
      cmd = Command( cmd_id, data ); //FIXME: Param needed anymore?
      break;
    }
  } // !Cmd_Quit

  GeomecPythonInterface::m_requests_queue.enqueue( cmd );
  
  //pyd::Py_END_ALLOW_THREADS
    pyd::PyEval_RestoreThread(_save);
  }

  return wrapper::none();
}

PyTuple* GeomecPythonInterface::_build_result( Command cmd )
{
  TPyObjectPtrV obj_v;
  
  IT_Data* data = cmd.data();
  if( data )
  {
    TDataV tuple = data->tuple();
    for( int i=0; i < tuple.size(); i++ ) // objects
    {
      TData& obj = tuple[ i ];

      switch( obj.data_type ) //TODO: check out int and double cases
      {
        case DataType::String:	obj_v.push_back( wrapper::build<const char*>(	(const char*)obj.data ) );	break;
        case DataType::Double: 	obj_v.push_back( wrapper::build<double>(		*(double*)obj.data ) );		break;
        case DataType::Integer:	obj_v.push_back( wrapper::build<int>(			*(int*)obj.data ) );		break;
        default:				obj_v.push_back( wrapper::build<TData>(	obj ) );							break;
      }
    }

    delete data; // class Reservoir, Displacement : public Data
  }

  PyObject* cmd_id_obj	= wrapper::build< int >( cmd.cmd() );
  PyTuple* params_tuple	= wrapper::build< TPyObjectPtrV >( obj_v );
  PyTuple* result			= wrapper::build< TPyObjectPtrV >( TPyObjectPtrV{ cmd_id_obj, params_tuple } );

  return result;
}

PyTuple* GeomecPythonInterface::_build_result_ack( Command cmd ) 
{
  TPyObjectPtrV obj_v;
  
  obj_v.push_back( wrapper::build< int >( cmd.cmd() ) );
  obj_v.push_back( wrapper::build< const char* >( "ok" ) );
    
  return wrapper::build< TPyObjectPtrV >( obj_v );
}

PyTuple* GeomecPythonInterface::_build_result_nack( Command cmd ) 
{
  TPyObjectPtrV obj_v;
  
  obj_v.push_back( wrapper::build< int >( cmd.cmd() ) );
  obj_v.push_back( wrapper::build< const char* >( "nok" ) );
    
  return wrapper::build< TPyObjectPtrV >( obj_v );
}

PyObject* GeomecPythonInterface::get_result( PyObject *self, PyObject *args )
{
  TPyObjectPtrV obj_v;

  bool bEnd = false;
  bool bIni = false;
  bool bErr = false;
  //Printer::instance()->debug("res -> Py_res_Q");
  Printer::instance()->debug("");
  Printer::instance()->debug("Getting results...");
  while( !bEnd )
  {
    Command cmd;
    //Py_BEGIN_ALLOW_THREADS
    {
      PyThreadState *_save;
      _save = pyd::PyEval_SaveThread();

    cmd = GeomecPythonInterface::m_results_queue.dequeue();
    //Printer::instance()->debug("res -> Py_res_Q [%s]", cmd.name().c_str() );
    //Printer::instance()->debug("RESULT [%s]", cmd.name().c_str() );
    //Py_END_ALLOW_THREADS
      pyd::PyEval_RestoreThread(_save);
    }

    Command::eType cmd_id = cmd.cmd();
    switch( cmd_id )
    {
      case Command::eType::Control_Ini:
        bIni = true;
        obj_v = {};
        break;
      case Command::eType::Control_End:
        bErr = !bIni;
        bEnd = true;
        break;
      case Command::eType::IT1_Exe_Run_Geomec:
      case Command::eType::IT1_Exe_Run_Geomec_Step:
      case Command::eType::IT1_Exe_Wait_End_Run_Geomec:
      case Command::eType::IT1_Set_Depletion:		//FIXME: generalize
        //return _build_result_ack( cmd );
        if( bIni )
          obj_v.push_back( _build_result_ack( cmd ) );
        else
          bErr = bEnd = true;
        break;
      default:
        //return _build_result( cmd );
        if( bIni )
          obj_v.push_back( _build_result( cmd ) );
        else
          bErr = bEnd = true;
        break;
    }
  }
  Printer::instance()->debug("... results available");
  return bErr ? nullptr : wrapper::build< TPyObjectPtrV >( obj_v );
}
