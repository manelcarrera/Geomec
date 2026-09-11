// DCasing.cpp: implementation of the CDCasing class.
//
//////////////////////////////////////////////////////////////////////


#ifdef _WIN32
#define MAKESTRING2(str)  #str
#define MAKESTRING(str) MAKESTRING2(str)
#pragma message("WARNING: disable C4718 (deleting code in Qt) in " __FILE__ "[" MAKESTRING(__LINE__) "]")
#pragma warning(push)
#pragma warning(disable: 4718)
#endif

#include "DCasing.h"
#include "QuantityInclude.h"
#include "DCasingPointResultData.h"
#include "DCasingPointResult.h"
#include "DianaStartUp.h"
#include "Environment.h"
#include "DCasingDianaSignals.h"
#include "lbcx.h"

#ifdef _WIN32
#pragma warning(pop)
#endif


#define  DCASING_EXE         "ca40.exe"
#define  DCASING_INPUT_FILE  "casing.cai" 
#define  DCASING_OUTPUT_FILE "casing.cao" 
#define  DCASING_VERSION  1
#define  DCASING_REVISION 1

#define  STR_AXISYMMETRIC_SHELL_BUCKLING	"AXISYMMETRIC SHELL BUCKLING"
#define  STR_SHELL_COLUMN_BUCKLING			"SHELL COLUMN BUCKLING"
#define  STR_CRITICAL_FREE_LENGTH			"CRITICAL FREE LENGTH"
#define  STR_SHELL_OVALISATION_BUCKLING		"SHELL OVALISATION BUCKLING"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDCasing::CDCasing() :
	m_Version(DCASING_VERSION),
	m_Revision(DCASING_REVISION)
{

}

//////////////////////////////////////////////////////////////////////

CDCasing::~CDCasing()
{
	RemoveAll();
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// interface
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

CDCasingPoint& CDCasing::AddPoint(CDCasingPoint & point)
{
	CDCasingPoint * Point = &point;
		
	m_points.resize(m_points.size()+1);
	m_points.insert(m_points.size()-1,Point);

	return *(m_points[m_points.size()-1]);

}

//////////////////////////////////////////////////////////////////////

CDCasingPoint& CDCasing::Point(int index)
{
	
	if (index >= m_points.size())
	{
		//error return overhere 
	}

	return (*m_points[index]);
}

//////////////////////////////////////////////////////////////////////
	
void CDCasing::RemovePoint(int index)
{
	//check the index
	if (index >= m_points.size())
	{
		//error throw exception
		return;
	}
	
	m_points.remove(index);
}

//////////////////////////////////////////////////////////////////////

void CDCasing::RemoveAll()
{
	m_points.clear();
}

//////////////////////////////////////////////////////////////////////

unsigned long CDCasing::NumberfPoints() const
{
	return m_points.count();
}

//////////////////////////////////////////////////////////////////////
void CDCasing::AbortCalculation()
{
	//TODO 
}

void CDCasing::HandleMessage(QString str)
{
	emit OnMessage(str);

}

void CDCasing::HandleError(QString str)
{
	emit OnError(str);
}

void CDCasing::HandleWarning(QString str)
{
	emit OnWarning(str);
}

void CDCasing::HandleFinished()
{
	ReadOutputFile("casing.cao");
	emit OnFinished(); 
}


void CDCasing::StartAnalysis()
{
	CDianaStartUp* dsu = CDianaStartUp::instance();

	//disconnect();

	// MCR: what does this do?
	CDCasingDianaSignals dCasingDianaSignals(CDianaStartUp::instance(), this);


	dsu->SetDianaEnv();

	// MCR: This needs to be set as it's not the standard one set in the step before
	dsu->SetDianaEnv( 
		CEnvironment::STP0, 
		dsu->GetDianaEnv( CEnvironment::DIAPATH ) + "/binseg/ap/" + DCASING_EXE );

	// who sets this?
	QString FFDIR = dsu->GetDianaEnv( "FFDIR" );
	WriteInputFile( FFDIR + "/" + DCASING_INPUT_FILE );

	dsu->var( CDianaStartUp::BASE, "casing" );
	dsu->var( CDianaStartUp::DISPLAY, "yes" );

	dsu->RunDiana(); // params not standard: "casing" and true

}
//////////////////////////////////////////////////////////////////////

void CDCasing::WriteInputFile(const QString & FileName)
{
	QFile varFile(FileName);
	QString Unit;
	QString value;

  bool retval = varFile.open(QIODevice::WriteOnly | QIODevice::Text);
		
	if (!retval) //return error
		return;
	
	//construct a textstream to parse the data
	QTextStream TextStream(&varFile);
	
	
	//write the header
	TextStream<<DCASING_INPUT_HEADER<<'\n';
	
	//write the VERSION BLOCK
	TextStream<<DCASING_INPUT_VERSION_HEADER <<'\n';
	TextStream<<ConstructItem(DCASING_INPUT_VERSION_VERSION,m_Version)<<'\n';
	TextStream<<ConstructItem(DCASING_INPUT_VERSION_REVISION,m_Revision)<<"\n\n";

	//write the WELLDATA Block
	TextStream<<DCASING_INPUT_WELLDATA_HEADER<<'\n';
	TextStream<<ConstructItem(DCASING_INPUT_NUMBER_OF_WELLPOINTS ,m_points.size())<<"\n\n";

	//write the GEOMETRY Block
	TextStream<<DCASING_INPUT_GEOMETRY_HEADING<<'\n';
	
	CLengthQuantity PipeLength(m_DCasingGeneralParameters.OuterDiameter());
	PipeLength.SetValue(m_DCasingGeneralParameters.OuterDiameter().Value()*m_DCasingGeneralParameters.PipeLengthOD().Value());

	TextStream<<ConstructItem(DCASING_INPUT_PIPE_LENGTH ,PipeLength.Value(),PipeLength.UnitName())<<'\n';
	
	TextStream<<ConstructItem(DCASING_INPUT_PIPE_STEEL_OUTER ,m_DCasingGeneralParameters.OuterDiameter().Value(),m_DCasingGeneralParameters.OuterDiameter().UnitName())<<'\n';
	TextStream<<ConstructItem(DCASING_INPUT_PIPE_STEEL_THICKNESS ,m_DCasingGeneralParameters.WallThickness().Value(),m_DCasingGeneralParameters.WallThickness().UnitName())<<'\n';
	TextStream<<ConstructItem(DCASING_INPUT_PIPE_STEEL_JOINT_SPACING  ,m_DCasingGeneralParameters.JointSpacing().Value(),m_DCasingGeneralParameters.JointSpacing().UnitName())<<'\n';
	TextStream<<ConstructItem(DCASING_INPUT_MIN_REQ_DRIFT_DIAM, m_DCasingGeneralParameters.RequiredDrifting().Value(),m_DCasingGeneralParameters.RequiredDrifting().UnitName())<<'\n';
	TextStream<<ConstructItem(DCASING_INPUT_DRIFTING_DIAMETER ,m_DCasingGeneralParameters.DriftingDiameter().Value(),m_DCasingGeneralParameters.DriftingDiameter().UnitName())<<'\n';
	TextStream<<ConstructItem(DCASING_INPUT_OUTER_INNER_RADIUS_RATIO,m_DCasingGeneralParameters.ModelOutDiamBoreDiamRatio().Value())<<"\n\n";

	//write the MATERIAL Block
	TextStream<<DCASING_INPUT_MATERIAL_HEADING<<'\n';
	TextStream<<ConstructItem(DCASING_INPUT_YIELD_STRAIN_PIPE_STEEL  ,m_DCasingGeneralParameters.SteelYieldStrain().Value())<<'\n';
	TextStream<<ConstructItem(DCASING_INPUT_POISSON_RATIO,m_DCasingGeneralParameters.SteelPoissonsRatio().Value())<<'\n';
	TextStream<<ConstructItem(DCASING_INPUT_LINEAR_COEFF_OF_PIPE,m_DCasingGeneralParameters.LinearCoeffStressStrain().Value())<<'\n';
	TextStream<<ConstructItem(DCASING_INPUT_POWER_COEFF_OF_PIPE ,m_DCasingGeneralParameters.PowerCoeffStressStrain().Value())<<'\n';
	TextStream<<ConstructItem(DCASING_INPUT_HARDENING_COEFF_OF_PIPE , m_DCasingGeneralParameters.HardeningCoeffStressStrain().Value())<<'\n';
	TextStream<<ConstructItem(DCASING_INPUT_STEEL_PIPE_YOUNGS_MODULUS  ,m_DCasingGeneralParameters.SteelPipeYoungsModulus().Value(),m_DCasingGeneralParameters.SteelPipeYoungsModulus().UnitName(CDoubleQuantity::SI_UNIT))<<'\n';
	TextStream<<ConstructItem(DCASING_INPUT_STEEL_PIPE_FRICTION_COEF  ,m_DCasingGeneralParameters.PipeFrictionCoeff().Value() ) <<'\n';

	TextStream<<ConstructItem(DCASING_INPUT_DEFORMATION_THEORY,m_DCasingGeneralParameters.DeformationTheory())<<'\n';
	TextStream<<ConstructItem(DCASING_INPUT_CEMENT_YOUNGS_MODULUS,m_DCasingGeneralParameters.CementYoungsModulus().Value(),m_DCasingGeneralParameters.CementYoungsModulus().UnitName())<<'\n';
	TextStream<<ConstructItem(DCASING_INPUT_POISSON_RATIO_OF_CEMENT   ,m_DCasingGeneralParameters.CementPoissonRatio().Value())<<"\n\n";
	
	//write the LOADING Block
	TextStream<<DCASING_INPUT_LOADING_HEADING<<'\n';
	TextStream<<ConstructItem(DCASING_INPUT_MAX_AXIAL_COMPR_STRAIN,m_DCasingGeneralParameters.MaxAxialCompStrain().Value())<<'\n';
	TextStream<<ConstructItem(DCASING_INPUT_LAT_BOUNDARYCONITION ,m_DCasingGeneralParameters.ModelLateralBoundary())<<"\n";
	TextStream<<ConstructItem(DCASING_INPUT_LOADING_NOFORMATION ,m_DCasingGeneralParameters.NoFormationCementOnly())<<"\n\n";
	
	//write the solutionProcessparmaters
	TextStream<<DCASING_INPUT_SOLUTION_PROCESS_PARAM<<'\n';
	TextStream<<ConstructItem(DCASING_INPUT_TOTAL_NR_OF_INCREMENTS ,m_DCasingGeneralParameters.NumberOfLoadSteps())<<'\n';
	TextStream<<ConstructItem(DCASING_INPUT_MAX_CIRCUMFER_WAVE_NR,m_DCasingGeneralParameters.MaximumWaveNumber())<<'\n';
	TextStream<<ConstructItem(DCASING_INPUT_MAX_NR_AXIAL_HALF_WAVES ,m_DCasingGeneralParameters.MaxAxialHalfWaves())<<"\n\n";

	//write the non uniform heading
	TextStream<<DCASING_INPUT_NON_UNIFORM_HEADING<<'\n';

	const long pos = 5;
	const long OFFSET = QString(DCASING_INPUT_WELL_POINT).length() - pos; 

	TextStream<<QString("%1").arg(DCASING_INPUT_WELL_POINT)<<QString("%1").arg(DCASING_INPUT_SUP_MOD,13 - OFFSET)<<QString("%1").arg(DCASING_INPUT_SUP_POIS,11)
			  <<QString("%1").arg(DCASING_INPUT_PRESSURE_IN,14)<<QString("%1").arg(DCASING_INPUT_PRESSURE_EX,15)<<QString("%1").arg(DCASING_INPUT_EPS_COM,11)<<'\n';
	
	
	QString Value;

	for (int counter = 0 ; counter < m_points.size();++counter)
	{
	
		TextStream<<QString("%1").arg(QString("%1").arg(counter+1),-5);
		
//		TextStream<<QString("%1").arg(QString("%1").arg(m_points[counter]->ElasticSupportYoungsMod().Value(),0,'E',5) + QString(" %1").arg(m_points[counter]->ElasticSupportYoungsMod().UnitName().begin()),-20);
//		TextStream<<QString("%1").arg(QString("%1").arg( m_points[counter] ->ElasticSupportPoissonRatio().Value(), 0, 'g', 5 ),-20);
//		TextStream<<QString("%1").arg(QString("%1").arg(m_points[counter]->InternalPressure().Value(),0,'E',5) + QString(" %1").arg(m_points[counter]->InternalPressure().UnitName().begin()),-20);
//		TextStream<<QString("%1").arg(QString("%1").arg(m_points[counter]->ExternalPressure().Value(),0,'E',5) + QString(" %1").arg(m_points[counter]->ExternalPressure().UnitName().begin()),-20)<<'\n';
	
		QString strValue;

		strValue = QString("%1").arg(m_points[counter]->ElasticSupportYoungsMod().Value());
		TextStream<<QString("%1").arg(strValue,-10);
		strValue = m_points[counter]->ElasticSupportYoungsMod().UnitName().c_str();
		TextStream<<QString("%1").arg(strValue,-5);
		
		TextStream<<QString("%1").arg(QString("%1").arg(m_points[counter] ->ElasticSupportPoissonRatio().Value()),-10);
		
		strValue = QString("%1").arg(m_points[counter]->InternalPressure().Value());
		TextStream<<QString("%1").arg(strValue,-10);
		strValue = m_points[counter]->InternalPressure().UnitName().c_str();
		TextStream<<QString("%1").arg(strValue,-5);
	
		strValue = QString("%1").arg(m_points[counter]->ExternalPressure().Value());
		TextStream<<QString("%1").arg(strValue,-10);
		strValue = m_points[counter]->ExternalPressure().UnitName().c_str();
		TextStream<<QString("%1").arg(strValue,-5);

		TextStream<<QString("%1").arg(QString("%1").arg(m_points[counter] ->Epscom().Value()),-10)<<'\n';

	}

	//close the file
	varFile.close();
}	

//////////////////////////////////////////////////////////////////////

void CDCasing::ReadOutputFile(const QString & FileName)
{
	QString ReadLine;
	QFile varFile(FileName);

  bool retval = varFile.open(QIODevice::ReadOnly | QIODevice::Text);
	
	if (!retval) //error return
		return;
	
	ReadSupportedValues(varFile);
	ReadUnsupportedValues(varFile);
}

//////////////////////////////////////////////////////////////////////

CDCasingGeneralParameters & CDCasing::GetGeneralParameters()
{
	return m_DCasingGeneralParameters;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// private member methods
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CDCasing::ReadSupportedValues( QFile & varFile)
{
	//Read the cemented casing values
	QString ReadString;
	QString ResultName;
	
	varFile.reset();

	QTextStream TextStream(&varFile);
	DCasingPointSupportedResult varDCasingPointSupportedResult;
		
	ReadString = TextStream.readLine(); 
	
  while (ReadString.indexOf(SUPPORTEDHEADERTITLE,0,Qt::CaseInsensitive) < 0 && !TextStream.atEnd())
		ReadString = TextStream.readLine();

	if (TextStream.atEnd()) 
		return;

  while(ReadString.indexOf(HEADERDESCRIPTION,0,Qt::CaseSensitive) < 0 && !TextStream.atEnd())
		ReadString = TextStream.readLine();

	if (TextStream.atEnd()) 
		return;
	
	//goto the next line 
	ReadString = TextStream.readLine();
	//find the description tag
	
	while(true)
	{	
    while(ReadString.indexOf(RESULTHEADERDESCRIPTION,0,Qt::CaseSensitive) < 0 && !TextStream.atEnd() && ReadString.indexOf(HEADERDESCRIPTION,0,Qt::CaseSensitive) < 0)
			ReadString = TextStream.readLine();
	
    if (TextStream.atEnd() || ReadString.indexOf(HEADERDESCRIPTION,0,Qt::CaseSensitive) != -1) 
			return;

		TrimQString(ReadString);
		
		ReadString = ReadString.right(ReadString.length() - RESULTHEADERDESCRIPTION.length());
		int i = ReadString.indexOf(RESULTHEADERDESCRIPTION);
		ResultName = ReadString.left(i);
		
		TrimQString(ResultName);
		
    while (ReadString.indexOf(POINTDESCRIPTION,0,Qt::CaseInsensitive) < 0 && !TextStream.atEnd())
			ReadString = TextStream.readLine();

		short ResultType;

		if(ResultName.contains(STR_AXISYMMETRIC_SHELL_BUCKLING))
			ResultType=AXISYMMETRIC_SHELL_BUCKLING;
		else if(ResultName.contains(STR_SHELL_COLUMN_BUCKLING))
			ResultType=SHELL_COLUMN_BUCKLING;
		else if(ResultName.contains(STR_CRITICAL_FREE_LENGTH))
			ResultType=CRITICAL_FREE_LENGTH;
		else if(ResultName.contains(STR_SHELL_OVALISATION_BUCKLING))
			ResultType=SHELL_OVALISATION_BUCKLING;
		else
			assert(false);


		for (int counter = 0 ; counter < m_points.size();++counter)
		{
			bool Valid = true;
			QString varString;
		
			TextStream>>i;				
		
			TextStream>>varString;
			if (!CastValue(varDCasingPointSupportedResult.m_LB,varString))
				Valid = false;
						
			TextStream>>varString;
			if (!CastValue(varDCasingPointSupportedResult.m_EPS1buck,varString))
				Valid = false;
			// we want positive compressive strains...
			varDCasingPointSupportedResult.m_EPS1buck = -varDCasingPointSupportedResult.m_EPS1buck;
			
			TextStream>>varString;
			if (!CastValue(varDCasingPointSupportedResult.m_EPS1loss_of_clear,varString))
				Valid = false;
			// we want positive compressive strains...
			varDCasingPointSupportedResult.m_EPS1loss_of_clear = -varDCasingPointSupportedResult.m_EPS1loss_of_clear;
			
			varDCasingPointSupportedResult.m_Status = (Valid) ? 0 : 1;
			
			m_points[counter]->GetCDCasingPointResult().GetDCasingPointSupportedResultMap()[ResultType] = varDCasingPointSupportedResult;
			
			//remove the leftover of the line from the stream
			ReadString = TextStream.readLine();
		}
	}

}

//////////////////////////////////////////////////////////////////////

void CDCasing::ReadUnsupportedValues( QFile & varFile)
{
	//Read the cemented casing values
	QString ReadString;
	QString ResultName;

	varFile.reset();
	
	QTextStream TextStream(&varFile);
	DCasingPointUnsupportedResult varDCasingPointUnsupportedResult;

	ReadString = TextStream.readLine(); 
	
  while (ReadString.indexOf(UNSUPPORTEDHEADERTITLE,0,Qt::CaseInsensitive) < 0 && !TextStream.atEnd())
		ReadString = TextStream.readLine();

	if (TextStream.atEnd()) 
		return;
	
  while(ReadString.indexOf(HEADERDESCRIPTION,0,Qt::CaseSensitive) < 0 && !TextStream.atEnd())
		ReadString = TextStream.readLine();

	if (TextStream.atEnd()) 
		return;
	
	//goto the next line 
	ReadString = TextStream.readLine();
	//find the description tag
	
	while(true)
	{	
    while(ReadString.indexOf(RESULTHEADERDESCRIPTION,0,Qt::CaseSensitive) < 0 && !TextStream.atEnd() && ReadString.indexOf(HEADERDESCRIPTION,0,Qt::CaseSensitive) < 0)
			ReadString = TextStream.readLine();
	
    if (TextStream.atEnd() || ReadString.indexOf(HEADERDESCRIPTION,0,Qt::CaseSensitive) != -1) 
			return;

		TrimQString(ReadString);
	
		ReadString = ReadString.right(ReadString.length() - RESULTHEADERDESCRIPTION.length());
		int i = ReadString.indexOf(RESULTHEADERDESCRIPTION);
		ResultName = ReadString.left(i);
		
		TrimQString(ResultName);
		
    while (ReadString.indexOf(POINTDESCRIPTION,0,Qt::CaseInsensitive) < 0 && !TextStream.atEnd())
			ReadString = TextStream.readLine();
	
		short ResultType;

		if(ResultName.contains(STR_AXISYMMETRIC_SHELL_BUCKLING))
			ResultType=AXISYMMETRIC_SHELL_BUCKLING;
		else if(ResultName.contains(STR_SHELL_COLUMN_BUCKLING))
			ResultType=SHELL_COLUMN_BUCKLING;
		else if(ResultName.contains(STR_CRITICAL_FREE_LENGTH))
			ResultType=CRITICAL_FREE_LENGTH;
		else if(ResultName.contains(STR_SHELL_OVALISATION_BUCKLING))
			ResultType=SHELL_OVALISATION_BUCKLING;
		else
			assert(false);
		
		for (int counter = 0 ; counter < m_points.size();++counter)
		{
			bool Valid = true;
			QString varString;
		
			TextStream>>i;				
			
			TextStream>>i;				
			TextStream>>varString;
			if (!CastValue(varDCasingPointUnsupportedResult.m_LB1,varString))
				Valid = false;

			TextStream>>varString;
			if (!CastValue(varDCasingPointUnsupportedResult.m_LB2,varString))
				Valid = false;
		
			varDCasingPointUnsupportedResult.m_Status = (Valid) ? 0 : 1;
			
			
			m_points[counter]->GetCDCasingPointResult().GetDCasingPointUnsupportedResultMap().insert(ResultType,varDCasingPointUnsupportedResult);
			
			//remove the leftover of the line from the stream
			ReadString = TextStream.readLine();
		}
	}

}
