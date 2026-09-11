#include "stdafx.h"

#include <MeshViz/nodes/PoDomain.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMatrixTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoText2.h>

#include "InfoLabel.h"

void InfoLabel::build()
{
    m_separator = new SoSeparator;
    m_matrixTransform = new SoMatrixTransform;
    m_translation = new SoTranslation;
    m_crossHairMaterial = new SoMaterial;
    m_crossHairMaterial->diffuseColor = SbColor(1.0f, 1.0f, 1.0f);
    m_crossHairMaterial->transparency = .7f;
    m_crossHair = new SoLineSet;
    m_crossHair->numVertices.set1Value(0, 2);
    m_crossHair->numVertices.set1Value(1, 2);
    m_crossHairVertices = new SoVertexProperty;
    m_crossHair->vertexProperty = m_crossHairVertices;

    m_labelMaterial = new SoMaterial;
    m_labelMaterial->diffuseColor = SbColor(1.0f, 1.0f, 1.0f);
    m_font = new SoFont;
    m_font->name="arial.ttf";
    m_font->size = 12.0f;
    m_text = new SoText2;
    m_text->spacing = 1.5f;

    m_separator->addChild(m_matrixTransform);
    m_separator->addChild(m_crossHairMaterial);
    m_separator->addChild(m_crossHair);
    m_separator->addChild(m_translation);
    m_separator->addChild(m_labelMaterial);
    m_separator->addChild(m_font);
    m_separator->addChild(m_text);

    addChild(m_separator);
}

InfoLabel::InfoLabel()
    : m_textOffsetX(0.0f)
    , m_textOffsetY(0.0f)
{
    build();
}

void InfoLabel::setLabelColor(float r, float g, float b)
{
    m_labelMaterial->diffuseColor = SbColor(r, g, b);
}

void InfoLabel::setup(PoDomain* domain, float aspect)
{
    SbVec3f minVec3 = domain->min.getValue();
    SbVec3f maxVec3 = domain->max.getValue();
    m_minVec = SbVec2f(minVec3[0], minVec3[1]);
    m_maxVec = SbVec2f(minVec3[0] + aspect * (maxVec3[0] - minVec3[0]), maxVec3[1]);

    m_textOffsetX = .01f * (maxVec3[0] - minVec3[0]);
    m_textOffsetY = .01f * (maxVec3[1] - minVec3[1]);

    SoMatrixTransform* transform = domain->getMatrixTransform();
    m_separator->replaceChild(m_matrixTransform, transform);
    m_matrixTransform = transform;
}

void InfoLabel::setPosition(float x, float y)
{
    m_translation->translation = SbVec3f(x + m_textOffsetX, y + m_textOffsetY, 0.0f);

    m_crossHairVertices->vertex.set1Value(0, SbVec3f(m_minVec[0], y, 0.0f));
    m_crossHairVertices->vertex.set1Value(1, SbVec3f(m_maxVec[0], y, 0.0f));
    m_crossHairVertices->vertex.set1Value(2, SbVec3f(x, m_minVec[1], 0.0f));
    m_crossHairVertices->vertex.set1Value(3, SbVec3f(x, m_maxVec[1], 0.0f));
}

void InfoLabel::setText(const char* text)
{
    clearText();
    m_text->string.setValue(text);
}

void InfoLabel::addText(const char* text)
{
    int num = m_text->string.getNum();
    m_text->string.set1Value(num, text);
}

void InfoLabel::show(bool visible)
{
    whichChild = visible ? SO_SWITCH_ALL : SO_SWITCH_NONE;
}

void InfoLabel::clearText()
{
    m_text->string.deleteValues(0, -1);
}

void InfoLabel::setCrossHairColor( float color[3] )
{
    m_crossHairMaterial->diffuseColor = SbColor(color);
}
