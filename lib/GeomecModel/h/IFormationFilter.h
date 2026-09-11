#ifndef _IFormationFilter_h_
#define _IFormationFilter_h_

class CFormationBase;

// If you do not want to have all the formations in the CSelectFormationsDlg,
// create a class that's derived from this one and implement the FormationValid
// function.
class IFormationFilter {
public:
  virtual ~IFormationFilter() = 0;
  virtual bool FormationValid(const CFormationBase &formation) const = 0;
};

#endif // _IFormationFilter_h_
