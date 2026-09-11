/********************************************************************

  RescuePillar.cpp

  The geometry is described by the control points, which are stored
  in a cSetRescuePoint.  The base point is NthObject(0) and the top
  point is NthObject(Count() - 1).

  p_1.z < p_2.z ... < p_i.z < ... p_n.z

  The values are stored in real values.  No transformation need be
  applied.  Z-values below the datum (normally mean sea level) are
  negative, above the datum are positive.  The logical geometries are:

  VERTICAL: 2 control points and is_vertical = true.  The x and y
              of the second control point is not stored.  When reading
              the x and y value of the second control point is set equal
              to the first control point.
  LINEAR:   2 control points and is_vertical = false.
  LISTRIC:  3 control points, follows spline curve.
  CURVED:   5 control points, follows a spline curve.
  OTHER:    Any number of points can be stored but automatic
              calculation of the geometry is not available.

  Truncation:
  top_truncation:  Is != 0 if the pillar is truncated to top by a
                     truncating pillar.  The LAST control point is
                     attached to the truncating pillar.
  base_truncation: Is != 0 if the pillar is truncated to base by
                     a truncating pillar.  The FIRST control point is
                     attached to the truncating pillar.

  Rod Hanks,  October 2003

  April 2005 - Fixed a bug spotted by Phil Johnson during the ILAB in
               Houston.

*********************************************************************/
#include "RescuePillar.h"
#include "RescueBuffer.h"
#include "RescuePoint.h"
#include "RescueSplineCoef.h"
#include "RescueTruncation.h"
#include "cSetRescuePoint.h"
#include "myHeaders.h"
#include <math.h>
#include <memory.h>
#include <string.h>

RESCUEBOOL RescuePillar::IsSplit() const { return dirCount > 1; }

RESCUEBOOL RescuePillar::IsTruncated() const { return (top_truncation != 0 || base_truncation != 0); }

void RescuePillar::rebuildSplineCoeffs() {
  if (splineCoefs != 0) {
    delete[] splineCoefs;
  }
  splineCoefsCount = ctrlPoints->Count();
  splineCoefs = new RescueSplineCoef[(int)(splineCoefsCount * 2)];
  RescueSplineCoef::rebuild(&splineCoefs[0], ctrlPoints, 0);
  RescueSplineCoef::rebuild(&splineCoefs[splineCoefsCount], ctrlPoints, 1);
  /*
    Spline coefficients are computed for x and y coordinates only.
    The original code from Petrel started the y splines immediately
    after the x splines, IE not leaving a space for the last, unused,
    x coef.  But this is confusing and did not result in a memory
    saving (since they made their array the same size as mine).
  */
}

RescueSplineCoef *RescuePillar::getXSplineCoefAt(RESCUEINT64 ndx) const { return &splineCoefs[0 + ndx]; }

RescueSplineCoef *RescuePillar::getYSplineCoefAt(RESCUEINT64 ndx) const { return &splineCoefs[splineCoefsCount + ndx]; }

RescuePoint *RescuePillar::getCtrlPointAt(RESCUEINT64 ndx) const { return ctrlPoints->NthObject(ndx); }

RescuePoint *RescuePillar::getMinCtrlPoint() const { return ctrlPoints->NthObject(0); }

RescuePoint *RescuePillar::getMaxCtrlPoint() const { return ctrlPoints->NthObject(ctrlPoints->Count() - 1); }

const RescuePoint RescuePillar::getMinCtrlPointRef() const { return RescuePoint(ctrlPoints->NthObject(0)); }

const RescuePoint RescuePillar::getMaxCtrlPointRef() const {
  return RescuePoint(ctrlPoints->NthObject(ctrlPoints->Count() - 1));
}

const RescuePoint RescuePillar::getMinTangentRef() const {
  /*
    Get tangent at the base of the pillar, normalized, pointing upwards.
  */
  RescueSplineCoef *xcoef = this->getXSplineCoefAt(0);
  RescueSplineCoef *ycoef = this->getYSplineCoefAt(0);
  RESCUEFLOAT dz = this->getCtrlPointAt(1)->z - this->getCtrlPointAt(0)->z;
  if (dz == 0) {
    return RescuePoint(FLT_MAX, FLT_MAX, FLT_MAX);
  } else {
    RESCUEFLOAT x = xcoef->derive(0) / dz;
    RESCUEFLOAT y = ycoef->derive(0) / dz;
    RESCUEFLOAT len = (RESCUEFLOAT)sqrt(x * x + y * y + 1);
    return RescuePoint(x / len, y / len, 1 / len);
  }
}

const RescuePoint RescuePillar::getMaxTangentRef() const {
  RESCUEINT64 i = splineCoefsCount - 2;
  RescueSplineCoef *xcoef = this->getXSplineCoefAt(i);
  RescueSplineCoef *ycoef = this->getYSplineCoefAt(i);
  RESCUEFLOAT dz = getCtrlPointAt(i + 1)->z - getCtrlPointAt(i)->z;
  if (dz == 0) {
    return RescuePoint(FLT_MAX, FLT_MAX, FLT_MAX);
  } else {
    RESCUEFLOAT x = xcoef->derive(1) / dz;
    RESCUEFLOAT y = ycoef->derive(1) / dz;
    RESCUEFLOAT len = (RESCUEFLOAT)sqrt(x * x + y * y + 1);
    return RescuePoint(x / len, y / len, 1 / len);
  }
}

RescuePoint *RescuePillar::getMinTangent() {
  /*
    Get tangent at the base of the pillar, normalized, pointing upwards.
    I realize this is a lot more annoying because you have to dispose of
    the RescuePoint but it is necessary for Java.
  */
  return new RescuePoint(this->getMinTangentRef());
}

RescuePoint *RescuePillar::getMaxTangent() { return new RescuePoint(this->getMaxTangentRef()); }

RescuePoint *RescuePillar::getByZ(RESCUEFLOAT z) const { return new RescuePoint(this->getRefByZ(z)); }

const RescuePoint RescuePillar::getRefByZ(RESCUEFLOAT z) const {
  if (z == FLT_MAX) {
    return RescuePoint(FLT_MAX, FLT_MAX, FLT_MAX);
  } else {
    RESCUEINT64 num_ctrl_points = ctrlPoints->Count();
    if (num_ctrl_points == 2) {
      if (isVertical) {
        RescuePoint p(this->getMinCtrlPoint());
        p.z = z;
        return p;
      } else {
        const RescuePoint min = this->getMinCtrlPointRef();
        const RescuePoint max = this->getMaxCtrlPointRef();
        return RescuePoint(min + (max - min) * (z - min.z) / (max.z - min.z));
      }
    }
    /*
      Below, use a spline curve.
    */
    if (z <= this->getMinCtrlPoint()->z) {
      const RescuePoint &min = this->getMinCtrlPointRef();
      if (z == min.z) {
        return min;
      } else {
        const RescuePoint &tangent = this->getMinTangentRef();
        if (tangent.z == 0) {
          return min;
        } else {
          RESCUEFLOAT t = (z - min.z) / tangent.z;
          return min + tangent * t;
        }
      }
    } else if (z >= this->getMaxCtrlPoint()->z) {
      const RescuePoint &max = this->getMaxCtrlPointRef();
      if (z == max.z) {
        return max;
      } else {
        const RescuePoint &tangent = this->getMaxTangentRef();
        if (tangent.z == 0) {
          return max;
        } else {
          RESCUEFLOAT t = (z - max.z) / tangent.z;
          return max + tangent * t;
        }
      }
    } else {
      RESCUEINT64 i = 1;
      while (z > this->getCtrlPointAt(i)->z) {
        i++;
      }
      i--;
      /*
        Find index number of nearest control point.
      */
      RescueSplineCoef *xcoef = this->getXSplineCoefAt(i);
      RescueSplineCoef *ycoef = this->getYSplineCoefAt(i);

      RescuePoint *min = this->getCtrlPointAt(i);
      RescuePoint *max = this->getCtrlPointAt(i + 1);

      RESCUEFLOAT dz = z - min->z;
      RESCUEFLOAT Dz = max->z - min->z;

      if (Dz == 0) {
        return RescuePoint(FLT_MAX, FLT_MAX, FLT_MAX);
      } else {
        dz /= Dz;
        RESCUEFLOAT x = xcoef->interpolate(min->x, dz);
        RESCUEFLOAT y = ycoef->interpolate(min->y, dz);
        return RescuePoint(x, y, z);
      }
    }
  }
}

RESCUEFLOAT *RescuePillar::NodeValues(RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k) {
  RESCUEFLOAT *myReturn = 0;
  RESCUEFLOAT x, y, z;
  Values(0, i, j, k, x, y, z);
  myReturn = new RESCUEFLOAT[3];
  myReturn[0] = x;
  myReturn[1] = y;
  myReturn[2] = z;
  return myReturn;
}

void RescuePillar::Values(RescueGeometry *geometry, RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k, RESCUEINT64 corner,
                          RESCUEFLOAT &x, RESCUEFLOAT &y, RESCUEFLOAT &z) {
  if (zValues != 0) {
    // KFLETCHER ILAB changes start (with copious explanations for me...)
    // IJK are cell IJK. cnr is cell corner. Cnrs 0-3 of cell IJK are in
    // node  k-layer K; Cnrs 4-7 of cell IJK are in node k-layer K+1. Therefore
    // for corner = 4-7 we need to increment the value of k.

    // The cells in dir 0-3 wrt node IJK are in layer K; the cells in dir 4-7
    // wrt node IJK are in layer K+1. NB the rescueGeometry method that calls
    // this method has taken care of getting the right pillar that contains
    // the zValues for this cell IJK-cornerconbination (ie g pillar(I,J+1) or
    // pillar(I+1,J) etc). Because of which pillar we are in, the k++ change below
    // and the dir numbering, the correct dir value is numerically equal to the
    // corner value, so we can use corner in the indexing of zValues below. BUT
    // for a unsplit pillar we only have the one zStack, ie dir (or corner) has
    // to be zero. So for the unsplit case we set corner == 0 AFTER we have
    // adjusted k to get the top nodes of the cell if required.

    //   if (dirCount <= corner)
    //  {
    //     corner = 0;
    //   }
    //   if (corner < 4)
    //   {
    //     k++;
    //   }
    if (corner >= 4) {
      k++;
    }
    if (dirCount <= corner) {
      corner = 0;
    }

    // KFLETCHER ILAB changes end

    if (k >= 0 && k < zCount) {
      /****** Malcolm Wilkins changes for handling truncations begin here. **

        In his original code, all of the RescueTruncation::SelfLimit() references
        were to RescueTruncation::Limit().

        **************************************************************************/
      z = zValues[corner][k];
      // Handle truncations
      RescuePillar *pillar = this;
      if (top_truncation != 0 || base_truncation != 0) {
        RescuePillar *top_pillar = 0;
        RescuePillar *base_pillar = 0;
        if (top_truncation != 0 && !top_truncation->isSelfTruncating() &&
            geometry->VertexIs(top_truncation->I(), top_truncation->J()) == RescueGeometry::R_SPLINE_LINE)
          top_pillar = geometry->DemandPillar(top_truncation->I(), top_truncation->J());
        if (base_truncation != 0 && !base_truncation->isSelfTruncating() &&
            geometry->VertexIs(base_truncation->I(), base_truncation->J()) == RescueGeometry::R_SPLINE_LINE)
          base_pillar = geometry->DemandPillar(base_truncation->I(), base_truncation->J());
        if (top_pillar != 0 || base_pillar != 0) {

          // Is z in elevation or depth?
          RescueModel *model = geometry->ParentModel();
          RescueCoordinateSystem *coordinateSystem = model->CoordinateSystem();
          RescueCoordinateSystem::Orientation orientation = coordinateSystem->OrientationIs();
          bool z_is_elevation =
              (orientation == RescueCoordinateSystem::LDF || orientation == RescueCoordinateSystem::LDB ||
               orientation == RescueCoordinateSystem::RDF || orientation == RescueCoordinateSystem::RDB);

          if (top_pillar != 0 && ((z_is_elevation && z > top_truncation->SelfLimit()) ||
                                  (!z_is_elevation && z < top_truncation->SelfLimit())))
            pillar = top_pillar;
          if (base_pillar != 0 && ((z_is_elevation && z < base_truncation->SelfLimit()) ||
                                   (!z_is_elevation && z > base_truncation->SelfLimit())))
            pillar = base_pillar;
        }
      }
/****** Malcolm Wilkins changes for handling truncations end here. **/
#if 0
      RescuePoint *pnt = this->getByZ(zValues[corner][k]);
#else
      RescuePoint *pnt = pillar->getByZ(zValues[corner][k]);
#endif
      x = pnt->x;
      y = pnt->y;
      z = pnt->z;
      delete pnt;
    } else {
      throw "Request for corner values is out of range.";
    }
  }
}

void RescuePillar::Values(RescueGeometry *geometry, RESCUEINT64 i, RESCUEINT64 j, RESCUEINT64 k, RESCUEFLOAT &x,
                          RESCUEFLOAT &y, RESCUEFLOAT &z) {
  // IJK are node indices; this is only called for unsplit pillars;
  // Node IJK is cnr 0 of cell IJK
  Values(geometry, i, j, k, 0, x, y, z);
}

RESCUEFLOAT RescuePillar::KValue(RESCUEINT64 k, RESCUEINT64 dir) {
  RESCUEFLOAT myReturn = FLT_MAX;
  Values(k, dir, myReturn);
  return myReturn;
}

RESCUEFLOAT RescuePillar::KValue(RESCUEINT64 k) {
  RESCUEFLOAT myReturn = FLT_MAX;
  Values(k, myReturn);
  return myReturn;
}

void RescuePillar::Values(RESCUEINT64 k, RESCUEINT64 dir, RESCUEFLOAT &z) {
  if (k >= 0 && k < zCount && zValues != 0) {
    if (dir >= dirCount) {
      dir = 0;
    }
    z = zValues[dir][k];
  }
}

void RescuePillar::Values(RESCUEINT64 k, RESCUEFLOAT &z) {
  if (k >= 0 && k < zCount && zValues != 0) {
    if (dirCount == 1) {
      z = zValues[0][k];
    } else {
      z = 0;
      RESCUEINT64 loop;
      for (loop = 0; loop < dirCount; loop++) {
        z += zValues[loop][k];
      }
      z /= (RESCUEFLOAT)dirCount;
    }
  }
}

void RescuePillar::ZValue(RESCUEINT64 k, RESCUEFLOAT newZValue) {
  if (k >= 0 && k < zCount) {
    if (dirCount == 1) {
      zValues[0][k] = newZValue;
    } else {
      RESCUEINT64 loop;
      for (loop = 0; loop < dirCount; loop++) {
        zValues[loop][k] = newZValue;
      }
    }
  }
}

void RescuePillar::ZStack(RescueGeometry *geometry, RESCUEFLOAT *newZValues) {
  RESCUEINT64 howMany = geometry->Grid()->Axis(2)->Count64();
  if (zValues != 0 && zCount != howMany) {
    RESCUEINT64 loop;
    for (loop = 0; loop < dirCount; loop++) {
      delete[] zValues[loop];
    }
    delete[] zValues;
    zValues = 0;
  }
  if (zValues == 0) {
    zValues = new RESCUEFLOAT *[1];
    zValues[0] = new RESCUEFLOAT[(size_t)howMany];
  }
  zCount = howMany;
  dirCount = 1;
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++) {
    zValues[0][loop] = newZValues[loop];
  }
}

void RescuePillar::ZStack(RescueGeometry *geometry, RESCUEINT64 dir, RESCUEFLOAT *newZValues) {
  RESCUEFLOAT *defaultZs = 0;
  RESCUEINT64 howMany = geometry->Grid()->Axis(2)->Count64();
  if (zValues != 0 && (zCount != howMany || dirCount != 8)) {
    if (zValues[0] != 0 && howMany == zCount) {
      defaultZs = zValues[0];
    }
    RESCUEINT64 loop;
    for (loop = 0; loop < dirCount; loop++) {
      if (zValues[loop] != defaultZs) {
        delete[] zValues[loop];
      }
    }
    delete[] zValues;
    zValues = 0;
  }
  if (zValues == 0) {
    zValues = new RESCUEFLOAT *[8];
    RESCUEINT64 dLoop;
    for (dLoop = 0; dLoop < 8; dLoop++) {
      zValues[dLoop] = new RESCUEFLOAT[(size_t)howMany];
      if (defaultZs != 0) {
        RESCUEINT64 loop;
        for (loop = 0; loop < howMany; loop++) {
          zValues[dLoop][loop] = defaultZs[loop];
        }
      }
    }
  }
  zCount = howMany;
  dirCount = 8;
  RESCUEINT64 loop;
  for (loop = 0; loop < howMany; loop++) {
    zValues[dir][loop] = newZValues[loop];
  }
  if (defaultZs != 0) {
    delete[] defaultZs;
  }
}

void RescuePillar::SwapKAxis(RESCUEINT64 kNodes) {
  if (zValues != 0) {
    RESCUEINT64 dLoop;
    for (dLoop = 0; dLoop < dirCount; dLoop++) {
      RESCUEINT64 fNdx = 0;
      RESCUEINT64 eNdx = zCount - 1;
      while (fNdx < eNdx) {
        RESCUEFLOAT temp = zValues[dLoop][fNdx];
        zValues[dLoop][fNdx] = zValues[dLoop][eNdx];
        zValues[dLoop][eNdx] = temp;
        fNdx++;
        eNdx--;
      }
    }
  }
}

void RescuePillar::SwapIJAxis(bool swapI, RESCUEINT64 iLowBound, RESCUEINT64 iCount, bool swapJ, RESCUEINT64 jLowBound,
                              RESCUEINT64 jCount) {
  if (top_truncation != 0) {
    top_truncation->SwapIJAxis(swapI, iLowBound, iCount, swapJ, jLowBound, jCount);
  }
  if (base_truncation != 0) {
    base_truncation->SwapIJAxis(swapI, iLowBound, iCount, swapJ, jLowBound, jCount);
  }
}

void RescuePillar::AddControlPoint(RESCUEFLOAT x, RESCUEFLOAT y, RESCUEFLOAT z) {
  // After you add no more than 5 control points,
  // use rebuildSplineCoeffs() to make the spline
  // coefficients.
  if (ctrlPoints->Count() < 5) {
    (*ctrlPoints) += new RescuePoint(x, y, z);
  }
}

void RescuePillar::AddControlPoint(RESCUEFLOAT x, RESCUEFLOAT y, RESCUEFLOAT z, RESCUEFLOAT xb, RESCUEFLOAT xc,
                                   RESCUEFLOAT xd, RESCUEFLOAT yb, RESCUEFLOAT yc, RESCUEFLOAT yd) {
  // Using this method you may add as many
  // control points as you wish.  You pass
  // both the point and the spline.  DO NOT
  // use rebuildSplineCoeffs() with more than
  // 5 control points.
  //
  // The spline coefficients associated with
  // the last point probably aren't used, but
  // you have to specify something anyway.
  (*ctrlPoints) += new RescuePoint(x, y, z);
  RESCUEINT64 howMany = ctrlPoints->Count();
  RescueSplineCoef *newSplineCoefs = new RescueSplineCoef[(int)(howMany * 2)];
  if (splineCoefsCount > 0) {
    memcpy(newSplineCoefs, splineCoefs, sizeof(RescueSplineCoef) * (size_t)splineCoefsCount);
    /*
      Copy the x.
    */
    memcpy(&newSplineCoefs[howMany], &splineCoefs[splineCoefsCount],
           sizeof(RescueSplineCoef) * (size_t)splineCoefsCount);
    delete[] splineCoefs;
  }
  splineCoefs = newSplineCoefs;
  splineCoefs[splineCoefsCount].b = xb;
  splineCoefs[splineCoefsCount].c = xc;
  splineCoefs[splineCoefsCount].d = xd;
  RESCUEINT64 yNdx = (splineCoefsCount * 2) + 1;
  splineCoefs[yNdx].b = yb;
  splineCoefs[yNdx].c = yc;
  splineCoefs[yNdx].d = yd;
  splineCoefsCount = howMany;
}

void RescuePillar::SetTopTruncation(RescueTruncation *newTop) {
  if (top_truncation != 0) {
    delete top_truncation;
  }
  top_truncation = newTop;
}

void RescuePillar::SetBaseTruncation(RescueTruncation *newBase) {
  if (base_truncation != 0) {
    delete base_truncation;
  }
  base_truncation = newBase;
}

RescuePillar::RescuePillar()
    : ctrlPoints(new cSetRescuePoint()), top_truncation(0), base_truncation(0), splineCoefs(0), splineCoefsCount(0),
      isVertical(FALSE), zCount(0), dirCount(0), zValues(0) {}

RescuePillar::~RescuePillar() {
  delete ctrlPoints;
  if (splineCoefs != 0) {
    delete[] splineCoefs;
  }
  if (top_truncation != 0) {
    delete top_truncation;
  }
  if (base_truncation != 0) {
    delete base_truncation;
  }
  if (zValues != 0) {
    RESCUEINT64 loop;
    for (loop = 0; loop < dirCount; loop++) {
      delete[] zValues[loop];
    }
    delete[] zValues;
  }
}

void RescuePillar::Archive(RescueContext *context, RESCUEINT64 kLayers, FILE *archiveFile, RESCUEBOOL compress) {
  ctrlPoints->Archive(context, archiveFile);
  if (splineCoefs == 0 || splineCoefsCount == 0) {
    myfprintf(context, archiveFile, (RESCUEINT64)0);
  } else {
    myfprintf(context, archiveFile, splineCoefsCount);
    RESCUEINT64 loop;
    for (loop = 0; loop < splineCoefsCount * 2; loop++) {
      splineCoefs[loop].Archive(context, archiveFile);
    }
  }
  if (top_truncation == 0) {
    myfprintf(context, archiveFile, (RESCUEINT64)0);
  } else {
    myfprintf(context, archiveFile, (RESCUEINT64)1);
    top_truncation->Archive(context, archiveFile);
  }
  if (base_truncation == 0) {
    myfprintf(context, archiveFile, (RESCUEINT64)0);
  } else {
    myfprintf(context, archiveFile, (RESCUEINT64)1);
    base_truncation->Archive(context, archiveFile);
  }
  myfprintf(context, archiveFile, isVertical);
  myfprintf(context, archiveFile, zCount);
  if (context->FileVersion() >= 35) {
    myfprintf(context, archiveFile, dirCount);
  }
  if (zCount > 0) {
    if (context->FileVersion() >= 35) {
      RESCUEINT64 loop;
      for (loop = 0; loop < dirCount; loop++) {
        myfprintf(context, archiveFile, zValues[loop], zCount, compress);
      }
    } else {
      myfprintf(context, archiveFile, zValues[0], zCount, compress);
    }
  }
  if (context->FileVersion() >= 37) {
    myfprintf(context, archiveFile, "EOD");
  }
}

RescuePillar::RescuePillar(RescueContext *context, FILE *archiveFile, RESCUEBOOL compress)
    : ctrlPoints(new cSetRescuePoint()), top_truncation(0), base_truncation(0), splineCoefs(0), splineCoefsCount(0),
      isVertical(FALSE), zCount(0), dirCount(0), zValues(0) {
  ctrlPoints->UnArchive(context, archiveFile);
  myfscanf(context, archiveFile, &splineCoefsCount);
  if (splineCoefsCount != 0) {
    splineCoefs = new RescueSplineCoef[(int)(splineCoefsCount * 2)];
    RESCUEINT64 loop;
    for (loop = 0; loop < splineCoefsCount * 2; loop++) {
      splineCoefs[loop].UnArchive(context, archiveFile);
    }
  }
  RESCUEINT64 dummyFlag;
  myfscanf(context, archiveFile, &dummyFlag);
  if (dummyFlag == 1) {
    top_truncation = new RescueTruncation(context, archiveFile);
  }
  myfscanf(context, archiveFile, &dummyFlag);
  if (dummyFlag == 1) {
    base_truncation = new RescueTruncation(context, archiveFile);
  }
  myfscanf(context, archiveFile, &isVertical);
  myfscanf(context, archiveFile, &zCount);
  if (context->ReadFileVersion() < 35) {
    if (zCount > 0) {
      dirCount = 1;
    }
  } else {
    myfscanf(context, archiveFile, &dirCount);
  }
  if (zCount > 0) {
    zValues = new RESCUEFLOAT *[(size_t)dirCount];
    RESCUEINT64 loop;
    for (loop = 0; loop < dirCount; loop++) {
      zValues[loop] = new RESCUEFLOAT[(size_t)zCount];
      myfscanf(context, archiveFile, zValues[loop], zCount, compress);
    }
  }
  if (context->ReadFileVersion() >= 37) {
    RESCUECHAR myString[255];

    myfgets(context, myString, 255, archiveFile);
    while (strcmp(myString, "EOD") != 0) {
      RescueBuffer buf(context, archiveFile);
      myfgets(context, myString, 255, archiveFile);
    }
  }
}
