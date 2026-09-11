/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "dimple.h"

#include "DrawDef.h"
#include "IDrawDef.h"
#include "OpenGLViewProxy.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

CDrawDefProxy::CDrawDefProxy(const IColorProxy &color_proxy, const IValueProxy &value_proxy)
    : m_pColor(&color_proxy), m_pValue(&value_proxy) {}

const IColorProxy &CDrawDefProxy::ColorProxy() const { return *m_pColor; }

void CDrawDefProxy::ColorProxy(const IColorProxy &color_proxy) { m_pColor = &color_proxy; }

const IValueProxy &CDrawDefProxy::ValueProxy() { return *m_pValue; }

void CDrawDefProxy::ValueProxy(IValueProxy &value_proxy) { m_pValue = &value_proxy; }

CDrawDefProxy::TColor CDrawDefProxy::Color(const geo::IPoint &pt) const { return m_pColor->Color(*m_pValue, pt); }
