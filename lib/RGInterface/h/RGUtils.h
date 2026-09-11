// Copyright 2011, Shell Global Solutions International B.V.
// All rights reserved. This document and the data and information contained herein is CONFIDENTIAL.
// Neither the whole nor any part of this document may be copied, modified or distributed in any
// form without the prior written consent of the copyright owner.

#pragma once

#include <cfloat>
#include <climits>
#include <limits.h>
#include <memory>
#include <string>

class RescueGeometry;
class RescueModel;
class RGDepletionStage;
class RescueTimeStepGroup;

namespace RGUtils {
// Rescue model related
RescueGeometry *getRescueGridByProperty(std::unique_ptr<RescueModel> &, const std::string &);

int nullInteger();
double nullReal();

bool isNull(const int &);
bool isNull(const double &);
} // namespace RGUtils

inline int RGUtils::nullInteger() { return INT_MAX; }
inline double RGUtils::nullReal() { return FLT_MAX; }

inline bool RGUtils::isNull(const int &x) { return x == nullInteger(); }
inline bool RGUtils::isNull(const double &x) { return x == nullReal(); }
