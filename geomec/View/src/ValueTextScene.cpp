#include "stdafx.h"

#include <iomanip>

#include "ValueTextScene.h"
#include "PixelCamera.h"
#include "ITensorGroup.h"

#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoText2.h>

#include <Inventor/nodes/SoMaterial.h>

#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoResetTransform.h>
#include "GlobalUnitNode.h"
#include "LengthQuantity.h"
#include "unitnode.h"


ValueTextScene::ValueTextScene() : m_translation(0, 0, 0)
{
  addPixelSpaceCamera( this, TRUE, FALSE );

  // Font settings and material for 2D annotations.
  m_font = new SoFont();
  m_font->name = "Arial";
  m_font->size = 12;
  m_font->renderStyle = SoFont::TEXTURE;

  m_material = new SoMaterial();
  m_material->diffuseColor = SbColor(1,1,1);

  addChild( m_font );
  addChild( m_material );

  m_displaySwitch = new SoSwitch;
  addChild(m_displaySwitch);
  m_displaySwitch->whichChild = SO_SWITCH_NONE;

  SoTranslation *translation = new SoTranslation();
  translation->translation = SbVec3f( -5, 5, 0 ); 
  m_displaySwitch->addChild( translation );

  m_tensorSwitch = new SoSwitch;
  m_displaySwitch->addChild(m_tensorSwitch);

  SoTranslation * tensorTranslation = new SoTranslation();
  tensorTranslation->translation = SbVec3f( 0, 80, 0 );
  m_tensorSwitch->addChild(tensorTranslation);

  m_tensorText = new SoText2;
  m_tensorText->justification = SoText2::RIGHT;

  m_tensorSwitch->addChild(m_tensorText);
  m_tensorSwitch->whichChild = SO_SWITCH_NONE;

  m_tensorText->spacing = 1.2f;
  m_tensorText->string.set1Value(0, "Tensor                               ");
  m_tensorText->string.set1Value(1, "E                 N                   D         ");
  m_tensorText->string.set1Value(2, "E: 1234567890  1234567890  1234567890");
  m_tensorText->string.set1Value(3, "N: 1234567890  1234567890  1234567890");
  m_tensorText->string.set1Value(4, "D: 1234567890  1234567890  1234567890");

  translation = new SoTranslation();
  translation->translation = SbVec3f( 0, 20, 0 ); // going up
  m_displaySwitch->addChild(translation);

  m_cellPropertyVectorText = new SoText2();
  m_cellPropertyVectorText->justification = SoText2::RIGHT;
  m_cellPropertyVectorText->string = "Cell Property Text";
  m_displaySwitch->addChild(m_cellPropertyVectorText);

  translation = new SoTranslation();
  translation->translation = SbVec3f( 0, 20, 0 ); // going up
  m_displaySwitch->addChild(translation);
  m_cellPropertyValueText = new SoText2();
  m_cellPropertyValueText->justification = SoText2::RIGHT;
  m_cellPropertyValueText->string = "Cell Property Text";
  m_displaySwitch->addChild(m_cellPropertyValueText);

  
  m_displaySwitch->addChild(translation);
  m_cellCoordinatesText = new SoText2;
  m_cellCoordinatesText->justification = SoText2::RIGHT;
  m_cellCoordinatesText->string = "Cell Coordinates Text";
  m_displaySwitch->addChild(m_cellCoordinatesText);

  m_displaySwitch->addChild(translation); // re-using ...
  m_cellIndexText = new SoText2;
  m_cellIndexText->justification = SoText2::RIGHT;
  m_cellIndexText->string = "Cell Index Text";
  m_displaySwitch->addChild(m_cellIndexText);

  // node info 

  m_displaySwitch->addChild(translation); // re-using ...
  m_displaySwitch->addChild(translation); // re-using ...

  m_nodePropertyVectorText = new SoText2();
  m_nodePropertyVectorText->justification = SoText2::RIGHT;
  m_nodePropertyVectorText->string = "Cell Property Vector Text";
  m_displaySwitch->addChild(m_nodePropertyVectorText);

  m_displaySwitch->addChild(translation);
  m_nodePropertyValueText = new SoText2();
  m_nodePropertyValueText->justification = SoText2::RIGHT;
  m_nodePropertyValueText->string = "Node Property Text";
  m_displaySwitch->addChild(m_nodePropertyValueText);

  m_displaySwitch->addChild(translation);
  m_nodeCoordinatesText = new SoText2;
  m_nodeCoordinatesText->justification = SoText2::RIGHT;
  m_nodeCoordinatesText->string = "Node Coordinates Text";
  m_displaySwitch->addChild(m_nodeCoordinatesText);

  m_displaySwitch->addChild(translation); // re-using ...
  m_nodeIndexText = new SoText2;
  m_nodeIndexText->justification = SoText2::RIGHT;
  m_nodeIndexText->string = "Node Index Text";
  m_displaySwitch->addChild(m_nodeIndexText);

  m_displaySwitch->addChild(translation); // re-using ...
  m_normalText = new SoText2;
  m_normalText->justification = SoText2::RIGHT;
  m_normalText->string = "Dragger Orientation Text";
  m_displaySwitch->addChild(m_normalText);

  
  SoResetTransform *pResetBbox = new SoResetTransform();
  pResetBbox->whatToReset = SoResetTransform::BBOX;
  addChild( pResetBbox );
}

void ValueTextScene::hide()
{
  m_displaySwitch->whichChild = SO_SWITCH_NONE;
}

void ValueTextScene::display()
{
  m_displaySwitch->whichChild = SO_SWITCH_ALL;
}

void ValueTextScene::SetTextColor( float * rgb )
{
    m_material->diffuseColor.setValue(rgb);
}

void ValueTextScene::GetTextColor( float * rgb )
{

    const SbColor & color = m_material->diffuseColor[0];
    color.getValue(rgb[0], rgb[1], rgb[2]);
}

void ValueTextScene::setPropertyName(const std::string & propertyName)
{
    m_propertyName = propertyName;
}

void ValueTextScene::setCellId(const std::string & meshName, size_t cellId)
{
  std::stringstream str;
  if ((long long) cellId >= 0)
  {
      str << "Formation/Surface: " << meshName << "      ";
      str << "Cell Id: " << std::setw (7) << std::setfill ('0') << (long long) cellId;
  }
  else
  {
      str << "Cell Id: " << std::setw (7) << std::setfill ('0') << "undefined";
  }
  m_cellIndexText->string = str.str();
}

void ValueTextScene::setCellCoordinates(const SbVec3f & coordinates)
{
    CGlobalUnitNode globalUnitNode;
    CLengthQuantity qnLength;

    std::stringstream str;
    double E, N, D;

    N = m_translation[0] + static_cast<double>(coordinates[0]);
    E = m_translation[1] + static_cast<double>(coordinates[1]);
    D = m_translation[2] + static_cast<double>(coordinates[2]);

    E = qnLength.Convert(E, globalUnitNode.unitNode().Unit(), CQuantity::SI_UNIT);
    N = qnLength.Convert(N, globalUnitNode.unitNode().Unit(), CQuantity::SI_UNIT);
    D = qnLength.Convert(D, globalUnitNode.unitNode().Unit(), CQuantity::SI_UNIT);

    std::string unit = globalUnitNode.unitNode().Unit() == CQuantity::SI_UNIT ? "m" : "ft";

    str << "Coordinates (" << unit << ")   E: " << E << ", N: " << N << ", D: " << D;
    m_cellCoordinatesText->string = str.str();
}

void ValueTextScene::setCellPropertyValue(double value)
{
    if (value == DBL_UNDEFINED) 
    {
        setCellPropertyValueUnknown();
    }
    else
    {
        std::stringstream str;
        str << "Value" << ": " << value;
        m_cellPropertyValueText->string = str.str();
    }
}

void ValueTextScene::setCellPropertyValueUnknown()
{
    std::stringstream str;
    str << "Value" << ": " << "no value";
    m_cellPropertyValueText->string = str.str();
}

void ValueTextScene::setCellPropertyVector(const MbVec3d & vector)
{
    if (vector[0] == DBL_UNDEFINED) 
    {
        setCellPropertyVectorUnknown();
    }
    else
    {
        std::stringstream str;
        vectorToString(vector, str);

        m_cellPropertyVectorText->string = str.str();
    }
}

void ValueTextScene::setCellPropertyVectorUnknown()
{
    std::stringstream str;
    str << "";
    m_cellPropertyVectorText->string = str.str();
}

void ValueTextScene::setNodeId(long long nodeId)
{
    std::stringstream str;
    if (nodeId < 0)
        str << "Node Id: " << std::setw(8) << "Undefined";
    else
        str << "Node Id: " << std::setw(8) << std::setfill ('0') << nodeId;
    m_nodeIndexText->string = str.str();
}

void ValueTextScene::setNodeCoordinates( const MbVec3d & coordinates )
{
    CGlobalUnitNode globalUnitNode;
    CLengthQuantity qnLength;

    std::stringstream str;
    double E, N, D;

    N = m_translation[0] + static_cast<double>(coordinates[0]);
    E = m_translation[1] + static_cast<double>(coordinates[1]);
    D = m_translation[2] + static_cast<double>(coordinates[2]);

    E = qnLength.Convert(E, globalUnitNode.unitNode().Unit(), CQuantity::SI_UNIT);
    N = qnLength.Convert(N, globalUnitNode.unitNode().Unit(), CQuantity::SI_UNIT);
    D = qnLength.Convert(D, globalUnitNode.unitNode().Unit(), CQuantity::SI_UNIT);

    std::string unit = globalUnitNode.unitNode().Unit() == CQuantity::SI_UNIT ? "m" : "ft";

    str << "Coordinates (" << unit << ")   E: " << E << ", N: " << N << ", D: " << D;
    m_nodeCoordinatesText->string = str.str();
}

void ValueTextScene::setNodePropertyValue(double value)
{
    if (value == DBL_UNDEFINED) 
    {
        setNodePropertyValueUnknown();
    }
    else
    {
        std::stringstream str;
        str << "Value" << ": " << value;
        m_nodePropertyValueText->string = str.str();
    }
}

void ValueTextScene::setNodePropertyValueUnknown()
{
    std::stringstream str;
    str << "Value" << ": " << "no value";
    m_nodePropertyValueText->string = str.str();
}

void ValueTextScene::setNodePropertyVector(const MbVec3d & vector)
{
    if (vector[0] == DBL_UNDEFINED) 
    {
        setNodePropertyVectorUnknown();
    }
    else
    {
        std::stringstream str;
        vectorToString(vector, str);
        m_nodePropertyVectorText->string = str.str();
    }
}

void ValueTextScene::setNodePropertyVectorUnknown()
{
    m_nodePropertyVectorText->string = "";
}

void ValueTextScene::vectorToString( const MbVec3d & vector, std::stringstream & str )
{
    double n, e, d, max;

    max = (std::max (std::max (std::abs(vector[0]), std::abs(vector[1])), std::abs(vector[2]) )) / 10000;

    e = (abs(vector[1]) < max) ? 0 : vector[1];
    n = (abs(vector[0]) < max) ? 0 : vector[0];
    d = (abs(vector[2]) < max) ? 0 : vector[2];

    str << "Vector  E: " << std::setw(10) << std::setprecision(8) << e << ", N: " << n << ", D: " << d;
}

void ValueTextScene::setNormalVector( const SbVec3f & vector )
{
   std::stringstream str;

   str << "Dragger orientation E: " << std::setw(7) << std::setprecision(3) << std::fixed << vector[1] << ", N: " << vector[0] << ", D: " << vector[2];

   m_normalText->string = str.str();
}

void ValueTextScene::clearNormalVector(  )
{
    m_normalText->string = "";
}

void ValueTextScene::setTensor( const ITensor * tensor )
{
    if (!tensor)
    {
        m_tensorSwitch->whichChild = SO_SWITCH_NONE;
    }
    else
    {
        m_tensorSwitch->whichChild = SO_SWITCH_ALL;

       std::stringstream strE, strN, strD;
       strE << "E: " << std::setw(12) << std::setprecision(8) << tensor->YY() << "  " << tensor->XY() << "  " << tensor->YZ();
       m_tensorText->string.set1Value(2, strE.str());

       strN << "N: " << std::setw(12) << std::setprecision(8) << tensor->XY() << "  " << tensor->XX() << "  " << tensor->XZ();
       m_tensorText->string.set1Value(3, strN.str());

       strD << "D: " << std::setw(12) << std::setprecision(8) << tensor->YZ() << "  " << tensor->XZ() << "  " << tensor->ZZ();
       m_tensorText->string.set1Value(4, strD.str());     
    }
}


void ValueTextScene::setTranslation(const SbVec3d& translation)
{
  m_translation.setValue(translation[0], translation[1], translation[2]);
}