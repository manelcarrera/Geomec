
#include "GetSideName.h"

std::string GetSideName(SIDE_ID SideID)
{

	switch(SideID)
	{
	case SIDE_ID_BOTTOM:
		 return "Bottom";
	case SIDE_ID_TOP:
		 return "Top";	
	case SIDE_ID_LEFT:
		return "Left";
	case SIDE_ID_RIGHT:
		return "Right";
	case SIDE_ID_FRONT:
		return "Front";
	case SIDE_ID_BACK:
		return "Back";
	default:
		assert(false);
	}

	return "";
}
