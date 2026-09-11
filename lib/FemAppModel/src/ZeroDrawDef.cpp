#include "ZeroDrawDef.h"

CZeroDrawDef::CZeroDrawDef() : IDrawDef() {}

CZeroDrawDef::~CZeroDrawDef() {}

std::vector<CZeroDrawDef::TColor> CZeroDrawDef::Color(const geo::IObject & /*object*/) const {
  std::vector<TColor> empty;

  return empty;
}

bool CZeroDrawDef::Visible() const { return false; }

bool CZeroDrawDef::PolyFillFront() const { return false; }

bool CZeroDrawDef::PolyFillBack() const { return false; }

bool CZeroDrawDef::LineStipple() const { return false; }

float CZeroDrawDef::LineWidth() const { return 0.0; }

float CZeroDrawDef::PointSize() const { return 0.0; }

bool CZeroDrawDef::PolyDrawFront() const { return false; }

bool CZeroDrawDef::PolyDrawBack() const { return false; }

int CZeroDrawDef::DepthFunc() const { return 0; }

bool CZeroDrawDef::CurvedDraw() const { return false; }
