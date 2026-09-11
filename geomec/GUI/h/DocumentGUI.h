#ifndef _DocumentGUI_h_
#define _DocumentGUI_h_

class CDocumentGUI {
public:
  CDocumentGUI(CDocumentBase *documentBase);
  virtual ~CDocumentGUI();

  CDocumentBase *model() const;

private:
  CDocumentGUI(const CDocumentGUI &rhs);
  CDocumentGUI &operator=(const CDocumentGUI &rhs);
  CDocumentBase *m_documentBase;
};

#endif // _DocumentGUI_h_
