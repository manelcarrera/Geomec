#ifndef _isMaterialConnected_h_
#define _isMaterialConnected_h_

class CModelBase;
class IMaterialBase;

/*
 *  This functionality was intended for inclusion in the function
 *  IMaterialServerTempl <MATERIAL, PARENT> ::OnNeighbourModified(...).
 *  Due to a circular dependency the source code was extracted to a separate
 *  source- and header-file.
 */

bool isMaterialConnected(CModelBase *modelBase, IMaterialBase *material);

#endif // _isMaterialConnected_h_
