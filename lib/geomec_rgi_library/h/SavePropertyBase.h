#ifndef _SavePropertyBase_h_
#define _SavePropertyBase_h_

class RGProperty;
class RGInterface;
class CModelBase;

namespace GeomecRGI {

class CRockMechProcessor;

class CSavePropertyBase {
public:
  CSavePropertyBase(const RGProperty &rgProperty);
  virtual ~CSavePropertyBase() = 0;

  virtual bool saveProperty(RGInterface &rgi, CModelBase &modelBase, const CRockMechProcessor &rmp) = 0;

protected:
  const RGProperty &m_RGProperty;

private:
  CSavePropertyBase(const CSavePropertyBase &rhs);
  CSavePropertyBase &operator=(const CSavePropertyBase &rhs);
};

} // namespace GeomecRGI

#endif // _SavePropertyBase_h_
