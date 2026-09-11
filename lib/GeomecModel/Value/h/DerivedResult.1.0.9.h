#ifndef _DerivedResult_1_0_9_h_
#define _DerivedResult_1_0_9_h_

namespace rpn {

class CRpnStack;

} // namespace rpn

class CStreamVersion;

void loadStream109(rpn::CRpnStack &stack, std::stringstream &stream, CStreamVersion &version);
void saveStream109(rpn::CRpnStack &stack, std::stringstream &stream);

#endif // _DerivedResult_1_0_9_h_
