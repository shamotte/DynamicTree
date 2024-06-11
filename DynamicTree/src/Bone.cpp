#include "Bone.h"
std::vector<bone> treeSkeleton;

void bone::update()
{
	if (lenght < 1.40)
		lenght =  atan(symulation_time - creation_time);
}
