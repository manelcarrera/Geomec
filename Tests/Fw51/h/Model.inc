#include <qtextstream.h>
#include <string>
#include <qfile.h>
#include "ModelOperationStack.h"


namespace
{

class ModelTestData
{
  QTextStream &os;
public:
  QTextStream& OS() { return os; }

  ModelTestData(QTextStream &os) : os(os) {}

  void PrintTree(const IModelObject& object, int nIndent = 0)
  {
	  // Print yourself and your child
	  for(int i = 0; i < nIndent; i++)
		  os << "   ";
  //	std::string str(object.Name());
	  QString mystr = object.text();
	  os << mystr << endl;
	  for(int j = 0; j < object.referenceSize(); j++)
	  {
		  for(int i = 0; i < (nIndent + 1); i++)
  			os << "   ";
	  	os << "ref. " << object.referenceAt(j).text() << endl;
	  }
 	  for(int k = 0; k < object.childSize(); k++)
		  PrintTree(object.childAt(k), nIndent + 1);
  }

  void PrintUndoRedoStatus( CDocumentBase& document )
  {
    os << "Can";
    if( !document.operationStack().canUndo() ) os << "not";
    os << " undo. Can";
    if( !document.operationStack().canRedo() ) os << "not";
    os <<" redo.\n";
  }
    
};


typedef test_fw51::TestInitInfo<ModelTestData> ModelTestInitInfo;
typedef test_fw51::TestInfo<ModelTestData> ModelTestInfo;

using namespace test_lib;

class Fw51ModelTest : public test_fw51::Fw51BaseTestTemplate<ModelTestInfo>
{
  CSoilModelFactory soilModelFactory;
protected:

  Fw51ModelTest()
  {
    m_ModelsPath.append("Model/");
    m_OutputPath.append("Model/");
  }
};


TEST_P(Fw51ModelTest, ModelBasics)
{
  const ModelTestInitInfo *inits = GetParam().inits;

  QFile of(m_Output.c_str());
  EXPECT_TRUE(of.open(QIODevice::WriteOnly | QIODevice::Truncate)) << "Failed to open output file " << m_Output << std::endl;
  QTextStream os(&of);

  ModelTestData testData(os);

  for (int i = 0; i < TEST_INIT_INFO_MAXSIZE; ++i)
  {
    if (!inits[i].init)
      break;

    inits[i].init(testData);
  }

  of.close();

  EXPECT_TRUE(test_lib::FileCompare::IdenticalText(m_Expected, m_Output));
}



void init_parent_child(ModelTestData &testData)
{
	QTextStream &os = testData.OS();

	// Creating the model
	os << "Creating a document ..." << endl;
	CSoilModel *pModel = new CSoilModel("Soilmodel", os);
	pModel->create();
	pModel->newDocument();
	// Create two materials
	CMaterial* material = new CMaterial("Clay", os);
	material->create( &pModel->Materials());
	material = new CMaterial("Sand", os);
	material->create( &pModel->Materials());
	// Create tree horizons
	CHorizon *pLast = new CHorizon("200 m", os);
	pLast->create( &pModel->Horizons());
	CHorizon* horizon2 = new CHorizon("0 m", os);
	horizon2->create( &pModel->Horizons(), pLast);
	CHorizon *pDeleteHorizon = new CHorizon("100 m", os);
	pDeleteHorizon->create( &pModel->Horizons(), pLast);
	testData.PrintTree(*pModel);
	os << endl;

	// Finding a child
	QString names[]= { "Clay", "Rock" };
	for ( int i = 0; i < 2; ++i ) {
		os << "Finding child \"" << names[i] << "\" in CSoilModel::Materials(): " << endl;
		IModelObject* child = pModel->Materials().findChild( names[i] );
		if ( child ) {
			os << "\tChild found: " << child->text() << endl;
		} else {
			os << "\tChild not found" << endl;
		}
	}
	os << endl;
	
	// Removing a horizon
	os << "Delete horizon : " << pDeleteHorizon->objectName() << endl;
	pDeleteHorizon->destroy();
	os << endl;

	// Create an extra material tree
	os << "Create extra material tree" << endl;
	CMaterialContainer *pContainer = new CMaterialContainer("Global Material", os);
	pContainer->create( pModel);
	material = new CMaterial("Mud", os);
	material->create( pContainer);
	CMaterial *pRock = new CMaterial("Rock", os);
	pRock->create( pContainer);
	testData.PrintTree(*pModel);
	os << endl;

	// Move rock to other three
	os << "Reparent rock from global to model" << endl;
	pRock->reParent(&pModel->Materials());
	testData.PrintTree(*pModel);
	os << endl;

	// Move rock out of document and back in again
	os << "Make rock parentless" << endl;
	pRock->reParent( 0 );
	testData.PrintTree( *pModel );
	os << endl << "Move rock back into global" << endl;
	pRock->reParent( &pModel->Materials() );
	testData.PrintTree( *pModel );
	os << endl;

	// Deleting the document
	os << "Deleting the document ..." << endl;
	pModel->closeDocument();
}

void init_reference(ModelTestData &testData)
{
	QTextStream &os = testData.OS();
	// Creating the model
	os << "Creating a document ..." << endl;
	CSoilModel *pModel = new CSoilModel("Soilmodel", os);
	pModel->create();
	pModel->newDocument();
	// Create two materials
	CMaterial* pClay = new CMaterial("Clay", os);
	pClay->create( &pModel->Materials() );
	CMaterial* pSand = new CMaterial("Sand", os);
	pSand->create( &pModel->Materials() );
	// Create tree horizons
	CHorizon* horizon1 = new CHorizon("0 m", os);
	horizon1->create( &pModel->Horizons() );
	CHorizon* horizon2 = new CHorizon("100 m", os);
	horizon2->create( &pModel->Horizons() );
	CHorizon* horizon3 = new CHorizon("200 m", os);
	horizon3->create( &pModel->Horizons() );
	// Create the formations entry
	CFormation *pUpper = new CFormation("Upper", os);
	pUpper->create( &pModel->Formations() );
	pUpper->Upper(&pModel->Horizons().at(0));
	pUpper->Lower(&pModel->Horizons().at(1));
	CFormation *pLower = new CFormation("Lower", os);
	pLower->create( &pModel->Formations() );
	pLower->Upper(&pModel->Horizons().at(1));
	pLower->Lower(&pModel->Horizons().at(2));
	testData.PrintTree(*pModel);
	os << endl;

	// Add a material
	os << "Add Sand to upper and Clay to lower formation" << endl;
	pUpper->Material(pSand);
	assert(pUpper->Material() == pSand);
	assert(pUpper->isReferenced(*pSand) == 2);	// because 0 is hor 0m and 1 is hor 100m
	pLower->Material(pClay);
	assert(pLower->Material() == pClay);
	assert(pLower->isReferenced(*pClay) == 2);	// because 0 is hor 100m and 1 is hor 100m

	testData.PrintTree(*pUpper);
	os << endl;

	// Delete material Sand
	os << "Delete Sand material" << endl;
	pSand->destroy();
	assert(pUpper->Material() == 0);
	testData.PrintTree(pModel->Formations());
	os << endl;

	// Unreference material sand
	os << "Unreference Clay material" << endl;
	pLower->removeReference(*pClay);
	assert(pLower->Material() == 0);
	testData.PrintTree(pModel->Formations());
	os << endl;

	// Deleting the model
	pModel->closeDocument();
}

void init_save_load_stream(ModelTestData &testData)
{
  QTextStream &os = testData.OS();

  TestLib::CurrentOutput(&os);

  os << "Creating document" << endl;
  CSoilModel* pModel = new CSoilModel("SoilModel", os);
  pModel->create();
  pModel->newDocument();
  os << endl;

  os << "Populating document" << endl;
  // Create two materials
  CMaterial* pClay = new CMaterial("Clay", os);
  pClay->create( &pModel->Materials() );
  CMaterial* pSand = new CMaterial("Sand", os);
  pSand->create( &pModel->Materials() );
  // Create tree horizons
  CHorizon* horizon1 = new CHorizon("0 m", os);
  horizon1->create( &pModel->Horizons() );
  CHorizon* horizon2 = new CHorizon("100 m", os);
  horizon2->create( &pModel->Horizons() );
  CHorizon* horizon3 = new CHorizon("200 m", os);
  horizon3->create( &pModel->Horizons() );
  // Create the formations entry
  CFormation *pUpper = new CFormation("Upper", os);
  pUpper->create( &pModel->Formations() );
  pUpper->Upper(&pModel->Horizons().at(0));
  pUpper->Lower(&pModel->Horizons().at(1));
  pUpper->Material(pSand);
  CFormation *pLower = new CFormation("Lower", os);
  pLower->create( &pModel->Formations() );
  pLower->Upper(&pModel->Horizons().at(1));
  pLower->Lower(&pModel->Horizons().at(2));
  pLower->Material(pClay);
  testData.PrintTree(*pModel);
  os << endl;

  // Save model
  os << "Saving document ... " << endl;
  IProgressBase progress;
  pModel->saveDocument("MyDoc.dat", progress);
  os << endl;

  // Deleting model
  os << "Closing document" << endl;
  pModel->closeDocument();
  os << endl;

  // Create document
  os << "Creating document" << endl;
  pModel = new CSoilModel("SoilModel", os);
  pModel->create();
  os << endl;

  // Loading model
  os << "Loading document ..." << endl;
  pModel->loadDocument("MyDoc.dat", progress);
  testData.PrintTree(*pModel);
  os << endl;

  // Deleting model
  os << "Closing document" << endl;
  pModel->closeDocument();
  os << endl;

  TestLib::CurrentOutput(0);

}

void init_undo_redo(ModelTestData &testData)
{
  QTextStream &os = testData.OS();

  TestLib::CurrentOutput(&os);

  // Creating the model
  os << "Creating a document ..." << endl;
  CSoilModel *pModel = new CSoilModel("Soilmodel", os);
  pModel->create();
  pModel->newDocument();
  assert(pModel->Materials().parent());
  // Create two materials
  CMaterial* pClay = new CMaterial("Clay", os);
  pClay->create( &pModel->Materials() );
  CMaterial* pSand = new CMaterial("Sand", os);
  pSand->create( &pModel->Materials() );
  // Create tree horizons
  CHorizon* horizon1 = new CHorizon("0 m", os);
  horizon1->create( &pModel->Horizons() );
  CHorizon* horizon2 = new CHorizon("100 m", os);
  horizon2->create( &pModel->Horizons() );
  CHorizon* horizon3 = new CHorizon("200 m", os);
  horizon3->create( &pModel->Horizons() );
  // Create the formations entry
  CFormation *pUpper = new CFormation("Upper", os);
  pUpper->create( &pModel->Formations() );
  pUpper->Upper(&pModel->Horizons().at(0));
  pUpper->Lower(&pModel->Horizons().at(1));
  CFormation *pLower = new CFormation("Lower", os);
  pLower->create( &pModel->Formations() );
  pLower->Upper(&pModel->Horizons().at(1));
  pLower->Lower(&pModel->Horizons().at(2));

  pModel->operationStack().setEnabled(true);
  testData.PrintUndoRedoStatus(*pModel); 
  testData.PrintTree(*pModel);
  os << endl;

  os << "Delete material clay ...\n";
  pClay->destroy();
  testData.PrintUndoRedoStatus(*pModel);
  testData.PrintTree(*pModel);

  os << "Undo delete material clay ...\n";
  pModel->operationStack().undo();
  testData.PrintUndoRedoStatus(*pModel);
  testData.PrintTree(*pModel);

  os << "Redo delete material clay ...\n";
  pModel->operationStack().redo();
  testData.PrintUndoRedoStatus(*pModel);
  testData.PrintTree(*pModel);

  os << "Undo delete material clay ...\n";
  pModel->operationStack().undo();
  testData.PrintUndoRedoStatus(*pModel);
  testData.PrintTree(*pModel);

  os << "Delete Upper formation...\n";
  pUpper->destroy();  
  testData.PrintUndoRedoStatus(*pModel);
  os << "Delete horizon Horizon1 ...\n";
  horizon1->destroy();
  testData.PrintUndoRedoStatus(*pModel);
  testData.PrintTree(*pModel);
  os << "Undo delete horizon Horizon1 ...\n";
  pModel->operationStack().undo();
  testData.PrintUndoRedoStatus(*pModel);
  os << "Undo delete upper formation ... \n";
  pModel->operationStack().undo();
  testData.PrintUndoRedoStatus(*pModel);
  testData.PrintTree(*pModel);

  // code below was commented out by TNO
/*
  // Add a material
  os << "Add Sand to upper and Clay to lower formation" << endl;
  pUpper->Material(pSand);
  assert(pUpper->Material() == pSand);
  assert(pUpper->isReferenced(*pSand) == 2);  // because 0 is hor 0m and 1 is hor 100m
  pLower->Material(pClay);
  assert(pLower->Material() == pClay);
  assert(pLower->isReferenced(*pClay) == 2);  // because 0 is hor 100m and 1 is hor 100m

  testData.PrintTree(*pUpper);
  os << endl;

  // Delete material Sand
  os << "Delete Sand material" << endl;
  pSand->destroy();
  assert(pUpper->Material() == 0);
  testData.PrintTree(pModel->Formations());
  os << endl;

  // Unreference material sand
  os << "Unreference Clay material" << endl;
  pLower->removeReference(*pClay);
  assert(pLower->Material() == 0);
  testData.PrintTree(pModel->Formations());
  os << endl;
*/
  // Deleting the model
  pModel->closeDocument();      

  TestLib::CurrentOutput(0);
}




const ModelTestInfo model_tests[] = {
  { "parent_child", { { 0, init_parent_child }, { 0, 0 } } },
  { "reference", { { 0, init_reference }, { 0, 0 } } },
  { "save_load_stream", { { 0, init_save_load_stream }, { 0, 0 } } },
  { "undo_redo", { { 0, init_undo_redo }, { 0, 0 } } }
};


INSTANTIATE_TEST_CASE_P(MainRun, Fw51ModelTest, ::testing::ValuesIn(model_tests));







}
