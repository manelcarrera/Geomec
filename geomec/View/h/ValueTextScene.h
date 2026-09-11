#ifndef _VALUE_TEXT_SCENE_H__
#define _VALUE_TEXT_SCENE_H__

#include <Inventor/nodes/SoSeparator.h>
#include <MeshVizXLM/MbVec3.h>

class SoSwitch;
class SoText2;
class SoFont;
class SoMaterial;

class ITensor;

class ValueTextScene : public SoSeparator
{
public:

  ValueTextScene();

  void SetTextColor( float * rgb );
  void GetTextColor( float * rgb );

  void setPropertyName(const std::string & propertyName);
  void setCellCoordinates (const SbVec3f & coordinates);
  void setCellId (const std::string & meshName, size_t cellId);
  void setCellPropertyValue(double value);
  void setCellPropertyValueUnknown();

  void setCellPropertyVector(const MbVec3d & vector);

  void vectorToString( const MbVec3d & vector, std::stringstream & str );

  void setCellPropertyVectorUnknown();

  void setNodeCoordinates (const MbVec3d & coordinates);
  void setNodeId (long long nodeId);
  void setNodePropertyValue(double value);
  void setNodePropertyValueUnknown();

  void setNodePropertyVector(const MbVec3d & vector);
  void setNodePropertyVectorUnknown();

  void display();
  void hide();
  bool isDisplayed();
  void setNormalVector( const SbVec3f & normal );
  void clearNormalVector( );


  void setTensor (const ITensor * tensor);

  void setTranslation(const SbVec3d& translation);

private:
  SoFont *        m_font;
  SoMaterial *    m_material;
  SoSwitch *      m_displaySwitch;
  SoSwitch *      m_tensorSwitch;

  SoText2 *       m_nodeIndexText;
  SoText2 *       m_nodeCoordinatesText;
  SoText2 *       m_nodePropertyValueText;
  SoText2 *       m_nodePropertyVectorText;

  SoText2 *       m_cellIndexText;
  SoText2 *       m_cellCoordinatesText;
  SoText2 *       m_cellPropertyValueText;
  SoText2 *       m_cellPropertyVectorText;
  SoText2 *       m_normalText;
  std::string     m_propertyName;
  SoText2 *       m_tensorText;

  SbVec3d         m_translation;
};

#endif // _VALUE_TEXT_SCENE_H__