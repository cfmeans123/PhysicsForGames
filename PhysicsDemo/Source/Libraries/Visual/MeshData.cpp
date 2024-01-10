#include "MeshData.h"

namespace jm::Visual
{
	InputLayout::InputLayout(std::vector<i32> attribSizes)
		: attributes(attribSizes.size())
		, elementSize(0)
	{
		u64 offset = 0;
		std::size_t index = 0;
		for (i32 attribSize : attribSizes)
		{
			JM_VISUAL_ASSERT(1 <= attribSize && attribSize <= 4);

			attributes[index] = { attribSize, offset };
			offset += attribSize;
			++index;
		}
		elementSize = offset;
	}
}
