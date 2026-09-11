#include "stdafx.h"
#include "CrossSectionManipulator.h"
#include "OIDIMeshNodeManager.h"

namespace
{
    bool intersect(const SbBox3f& box, const SbPlane& plane)
    {
        SbVec3f minVec = box.getMin();
        SbVec3f maxVec = box.getMax();

        SbVec3f corners[] = 
        {
            SbVec3f(minVec[0], minVec[1], minVec[2]),
            SbVec3f(minVec[0], minVec[1], maxVec[2]),
            SbVec3f(minVec[0], maxVec[1], minVec[2]),
            SbVec3f(minVec[0], maxVec[1], maxVec[2]),

            SbVec3f(maxVec[0], minVec[1], minVec[2]),
            SbVec3f(maxVec[0], minVec[1], maxVec[2]),
            SbVec3f(maxVec[0], maxVec[1], minVec[2]),
            SbVec3f(maxVec[0], maxVec[1], maxVec[2]),
        };

        bool in = false;
        bool out = false;

        for(int i=0; (i < 8) && !(in && out); ++i)
        {
            if(plane.isInHalfSpace(corners[i]))
                in = true;
            else
                out = true;
        }

        return in && out;
    }
}

SO_NODE_SOURCE(CrossSectionManipulator);

void CrossSectionManipulator::draggerStartCallback(void* userData, SoDragger* dragger)
{
  CrossSectionManipulator* manip = reinterpret_cast<CrossSectionManipulator*>(userData);
  manip->onDraggerStart(dragger);
}

void CrossSectionManipulator::draggerMotionCallback(void* userData, SoDragger* dragger)
{
  CrossSectionManipulator* manip = reinterpret_cast<CrossSectionManipulator*>(userData);
  manip->onDraggerMotion(dragger);
}

void CrossSectionManipulator::draggerFinishCallback(void* userData, SoDragger* dragger)
{
  CrossSectionManipulator* manip = reinterpret_cast<CrossSectionManipulator*>(userData);
  manip->onDraggerFinish(dragger);
}

void CrossSectionManipulator::onDraggerStart(SoDragger* dragger)
{
}

void CrossSectionManipulator::onDraggerMotion(SoDragger* dragger)
{
    assert(dragger == this);

    // Compute the new plane equation from the position and orientation of the manipulator

    confineTranslation();  // keep translation within the bounding box
    SbVec3f position = translation.getValue();
    
    // Compute the normal by rotating the default normal according to the manips orientation
    SbVec3f transformedNormal;
    rotation.getValue().multVec(getDefaultNormal(), transformedNormal);

    if (m_orientationConstraint == CCrossSection::ORIENTATION_NE && std::abs(transformedNormal[2]) > FLT_EPSILON)
    {      
        transformedNormal[2] = 0;
        transformedNormal.normalize();
    }  

    setNormal(transformedNormal);

    SbPlane newPlane = SbPlane(transformedNormal, position);
    if(plane.getValue() != newPlane)
        plane = newPlane;
}

void CrossSectionManipulator::onDraggerFinish(SoDragger* dragger)
{
}

CrossSectionManipulator::CrossSectionManipulator() :
    m_rotator(0),
    m_separator(0),
    m_confined(false),
    m_orientationConstraint(CCrossSection::ORIENTATION_ANY),
    m_meshNodeManager(0)
{
  SO_NODE_CONSTRUCTOR(CrossSectionManipulator);
  SO_NODE_ADD_FIELD(plane, (getDefaultPlane()));
  SO_NODE_ADD_FIELD(normal, (getDefaultNormal()));

  addStartCallback(draggerStartCallback, this);
  addMotionCallback(draggerMotionCallback, this);
  addFinishCallback(draggerFinishCallback, this);

  setTranslation(SbVec3f(FLT_MAX, FLT_MAX, FLT_MAX));

  m_rotator = getPart ("rotator.rotator", FALSE);
  assert (m_rotator);
  m_rotator->ref();
  m_separator = new SoSeparator;
  m_separator->ref();
}

CrossSectionManipulator::~CrossSectionManipulator()
{
    m_rotator->unref();
    m_separator->unref();
}

/**
 * Sets the bounding box of the mesh. This has the added effect of setting the position of the 
 * manipulator to the center of the box, and the size to 10% of the box.
 */
void CrossSectionManipulator::setBoundingBox(const SbBox3f& bbox)
{
  m_bbox = bbox;
  float xmin, ymin, zmin, xmax, ymax, zmax;
  m_bbox.getBounds(xmin, ymin, zmin, xmax, ymax, zmax);

  // decrease the box slightly
  const float offset = std::max((xmax - xmin), (ymax - ymin)) / 2000;
  m_bboxReduced.setBounds(xmin + offset, ymin + offset, zmin + offset, xmax - offset, ymax - offset, zmax - offset);
  scaleFactor = .05f * bbox.getSize();
}

/**
 * Sets the plane. The orientation of the manipulator is changed to match the plane orientation,
 * and the current position of the manipulator is projected onto the plane.
 */
void CrossSectionManipulator::setPlane(const SbVec3f & point_, const SbVec3f & normal_)
{
    setNormal(normal_);
    setTranslation(point_);

    SbPlane newPlane = SbPlane(normal_, point_);
    plane = newPlane;
}

void CrossSectionManipulator::touch()
{
    plane.touch();
    normal.touch();
    translation.touch();
    rotation.touch();
}

void CrossSectionManipulator::initClass()
{
  SO_NODE_INIT_CLASS(CrossSectionManipulator, SoJackDragger, "JackDragger");
}

SbVec3f CrossSectionManipulator::getDefaultBasePoint()
{
  return SbVec3f(0.0f, 0.0f, 0.0f);
}

SbVec3f CrossSectionManipulator::getDefaultNormal()
{
  return SbVec3f(0.0f, 1.0f, 0.0f);
}

SbPlane CrossSectionManipulator::getDefaultPlane()
{
  return SbPlane(getDefaultNormal(), getDefaultBasePoint());
}

void CrossSectionManipulator::limitRotation( CCrossSection::OrientationType orientation )
{
    m_orientationConstraint = orientation;
    if (orientation == CCrossSection::ORIENTATION_DEPTH ||
        orientation == CCrossSection::ORIENTATION_CURRENT ||
        orientation == CCrossSection::ORIENTATION_EASTING ||
        orientation == CCrossSection::ORIENTATION_NORTHING)
    {
        setPart ("rotator.rotator", m_separator);
    }
    else
    {
        setPart ("rotator.rotator", m_rotator);
    }
}

void CrossSectionManipulator::removeConstraints()
{
    limitRotation(CCrossSection::ORIENTATION_ANY);
}

void CrossSectionManipulator::setTranslation( const SbVec3f & position )
{
    translation = m_savedTranslation = position;
}

void CrossSectionManipulator::setNormal( const SbVec3f & normalVec )
{
    normal = normalVec;
    SbRotation rot(getDefaultNormal(), normalVec);
    rotation = rot;
}

void CrossSectionManipulator::confineTranslation()
{
    SbVec3f confinedTranslation = translation.getValue();
    if (isConfined())
    {
        if (!m_meshNodeManager->DisplayedVolumesContains(translation.getValue()) && !m_meshNodeManager->DisplayedVolumesContains(m_savedTranslation))
        {
            confinedTranslation = m_meshNodeManager->getDisplayedVolumesPoint();
        }
        else
        {
            SbVec3f outsidePosition = translation.getValue();
            SbVec3f insidePosition = m_savedTranslation;

            if (!m_meshNodeManager->DisplayedVolumesContains(outsidePosition))
            {
                assert (m_meshNodeManager->DisplayedVolumesContains(insidePosition));
                int count = 5;
                while (--count >= 0)
                {
                    SbVec3f  displacement = outsidePosition - insidePosition;

                    SbVec3f newPosition = insidePosition + displacement / 2;

                    if (!m_meshNodeManager->DisplayedVolumesContains(newPosition))
                    {
                        outsidePosition = newPosition;
                    }
                    else
                    {
                        insidePosition = newPosition;
                    }
                }
                confinedTranslation = insidePosition;
            }
        }
    }
    else
    {
        if (!m_bbox.intersect(translation.getValue()) && !m_bbox.intersect(m_savedTranslation.getValue()))
        {
            confinedTranslation = m_bbox.getCenter();
            return;
        }
        else
        {
            SbVec3f outsidePosition = translation.getValue();
            SbVec3f insidePosition = m_savedTranslation;

            if (!m_bbox.intersect(outsidePosition))
            {
                while (m_bboxReduced.intersect(insidePosition)) // we want the inside position between box and bbox reduced
                {
                    SbVec3f  displacement = outsidePosition - insidePosition;

                    SbVec3f newPosition = insidePosition + displacement / 2;

                    if (!m_bbox.intersect(newPosition))
                    {
                        outsidePosition = newPosition;
                    }
                    else
                    {
                        insidePosition = newPosition;
                    }
                }
                confinedTranslation = insidePosition;
            }
        }
    }
    setTranslation(confinedTranslation);
}

bool CrossSectionManipulator::isConfined() const
{
    return m_confined;
}

void CrossSectionManipulator::setConfined( bool isConfined )
{
    m_confined = isConfined;
    updateDraggerPosition();
}

void CrossSectionManipulator::setMeshNodeManager( OIDIMeshNodeManager * meshNodeManager )
{
    m_meshNodeManager = meshNodeManager;
}

void CrossSectionManipulator::updateDraggerPosition()
{
    onDraggerMotion(this);
}
