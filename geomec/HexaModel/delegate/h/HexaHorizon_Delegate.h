#ifndef _HexaHorizon_Delegate_h_
#define _HexaHorizon_Delegate_h_

class CHexaHorizon;
class CHexaHorizonEntry;

#include "3DHorizon_Delegate.h"

class CHexaHorizon_Delegate : public C3DHorizon_Delegate
{
public:
  CHexaHorizon_Delegate(CHexaHorizon* hexaHorizon);

  virtual bool Attributes();

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  virtual bool CanDestroy() const;
  virtual bool Destroy();

private:
  CHexaHorizon_Delegate(const CHexaHorizon_Delegate& rhs);
  CHexaHorizon_Delegate& operator = (const CHexaHorizon_Delegate& rhs);

  CHexaHorizon* m_hexaHorizon;

  REGISTER_DELEGATE(CHexaHorizon, CHexaHorizon_Delegate);
};

class CHexaHorizonEntry_Delegate :
  public CStorageNodeEntry_Delegate <CHexaHorizon>
{
public:
  CHexaHorizonEntry_Delegate(CHexaHorizonEntry* hexaHorizonEntry);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);
  void CreateHorizon();
  bool CanCreateHorizon() const;

private:
  CHexaHorizonEntry_Delegate(const CHexaHorizonEntry_Delegate& rhs);
  CHexaHorizonEntry_Delegate& operator = (
    const CHexaHorizonEntry_Delegate& rhs);

  CHexaHorizonEntry* m_hexaHorizonEntry;

  REGISTER_DELEGATE(CHexaHorizonEntry, CHexaHorizonEntry_Delegate);
};

#endif  // _HexaHorizon_Delegate_h_
