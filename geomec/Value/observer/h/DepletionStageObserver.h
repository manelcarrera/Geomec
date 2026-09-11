#ifndef _DepletionStageObserver_h_
#define _DepletionStageObserver_h_

template <typename DIALOG>
  class CDepletionStageObserver : public IListObject
{
  const CDepletionStage& m_stage;
  DIALOG& m_dlg;
public:
  CDepletionStageObserver(const CDepletionStage& stage, DIALOG& dlg, CListCtrl& ctrl)
  : IListObject(ctrl), m_stage(stage), m_dlg(dlg)
  {
    ctrl.SetItemData(ctrl.GetItemCount() - 1, (DWORD_PTR) this);
    Update();
  }

  virtual unsigned int Icon() const
  {
    return m_stage.IconId();
  }

  virtual QString Text() const
  {
    return m_stage.Name();
  }

  virtual unsigned int StateIcon() const
  {
    if(m_dlg.TimeStep(m_stage))
      return IDI_CHECKED;
    return IDI_UNCHECKED;
  }

  virtual void ToggleState()
  {
    m_dlg.TimeStep(m_stage, !m_dlg.TimeStep(m_stage));
    Update();

    if (!m_dlg.UpdateData())
  {
    m_dlg.TimeStep(m_stage, !m_dlg.TimeStep(m_stage));
    Update();
  }
  }
};

#endif // _DepletionStageObserver_h_
