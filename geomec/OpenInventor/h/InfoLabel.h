#ifndef _InfoLabel_h_
#define _InfoLabel_h_

class PoDomain;

class SoMatrixTransform;
class SoTranslation;
class SoMaterial;
class SoLineSet;
class SoVertexProperty;
class SoFont;
class SoText2;

#include <Inventor/nodes/SoSwitch.h>

class InfoLabel : public SoSwitch
{
    SoSeparator* m_separator;
    SoMatrixTransform* m_matrixTransform;
    SoTranslation* m_translation;
    SoMaterial* m_crossHairMaterial;
    SoLineSet* m_crossHair;
    SoVertexProperty* m_crossHairVertices;

    SoMaterial* m_labelMaterial;
    SoFont* m_font;
    SoText2* m_text;

    SbVec2f m_minVec;
    SbVec2f m_maxVec;

    float m_textOffsetX;
    float m_textOffsetY;

    void build();

public:

    InfoLabel();

    void setup(PoDomain* domain, float aspect=1.0f);

    void setPosition(float x, float y);

    void setText(const char* text);
    void addText(const char* text);
    void clearText();

    void show(bool visible);
    void setLabelColor(float r, float g, float b);     
    void setCrossHairColor( float color[3] );
};

#endif  // _InfoLabel_h_
