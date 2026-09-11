#ifndef _BoundarySpecific_h_
#define _BoundarySpecific_h_

template <typename B> BOOL EnableBoundarySpecificWindow(CDialog *dialog, int dialogItem, BOOL bEnable = TRUE) {
  return FALSE;
}

template <typename B> BOOL SetBoundarySpecificReadOnly(CDialog *dialog, int dialogItem, BOOL bReadOnly = TRUE) {
  return FALSE;
}

template <typename B> void SetBoundarySpecificWindowText(CDialog *dialog, int dialogItem, LPCTSTR string) {}

template <typename B>
void BoundarySpecificDDX_Point(CGeomecAttriTemp<B> *dialog, CDataExchange *pDX, int nID_X, int nID_Y, int nID_Z,
                               geo::IPoint &point, int nID_UNIT1, int nID_UNIT2, int nID_UNIT3) {}

template <typename B> void BoundarySpecificDDX_Text(CDataExchange *pDX, int nIDC, double &value) {}

// The specialization of these functions for the template argument CHexaBoundary
// are defined in the file HexaBoundary_Delegate.cpp. Including it in this
// header-file results in linker errors.

#endif // _BoundarySpecific_h_
