#include "Bone.h"
std::vector<bone> treeSkeleton;

void bone::update()
{
	lenght = multiplayer * atan(symulation_time - creation_time);
}
