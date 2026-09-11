#ifndef _ATTRITHINLAYERUPSCALINGTARGETPOINTSETS_H_
#define _ATTRITHINLAYERUPSCALINGTARGETPOINTSETS_H_

class CUpscalingPointsetRangesBranch;

class CAttriUpscalingTargetPointsets : public CDialog
{
public:
	CAttriUpscalingTargetPointsets(CUpscalingPointsetRangesBranch& branch, CWnd* pParent = NULL);   // standard constructor
	virtual ~CAttriUpscalingTargetPointsets();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual void OnOK();

private:
  CUpscalingPointsetRangesBranch& m_branch;
  int m_nAll;
};

#endif // _ATTRITHINLAYERUPSCALINGTARGETPOINTSETS_H_
