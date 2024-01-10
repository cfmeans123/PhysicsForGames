#pragma once

#include "VisualDebug.h"

#include "Platform/PlatformCore.h"

#include <vector>
#include <map>

namespace jm::Visual
{
	struct InputAttribute
	{
		i32 size;
		u64 offset;
	};

	struct RawBuffer
	{
		byte_list data;
		std::size_t size{};

		void Append(RawBuffer&& otherBuffer)
		{
			data.insert(data.end(), otherBuffer.data.begin(), otherBuffer.data.end());
			size += otherBuffer.size;
		}
	};

	class InputLayout
	{
	public:
		std::vector<InputAttribute> attributes;
		u64 elementSize;

		InputLayout() = default;
		InputLayout(std::vector<i32> attribSizes);
	};

	class ComponentLayout : InputLayout
	{
	private:
		std::vector<byte_list> components;
		std::size_t elementCount;

	public:

		ComponentLayout(InputLayout const& inputLayout)
			: InputLayout(inputLayout)
			, components(attributes.size())
			, elementCount(0)
		{
		}

		template <typename T>
		void AddComponent(uSize index, std::vector<T> elements)
		{
			JM_VISUAL_ASSERT(sizeof(T) == attributes[index].size * sizeof(float));

			if (elementCount == 0)
			{
				elementCount = elements.size();
			}
			else
			{
				JM_VISUAL_ASSERT(elementCount == elements.size());
			}

			byte* first = (byte*)(elements.data() + 0);
			byte* last = (byte*)(elements.data() + elements.size());
			components[index] = byte_list{ first, last };

			JM_VISUAL_ASSERT(components[index].size() == elementCount * attributes[index].size * sizeof(float));
		}

		RawBuffer GetVertexBuffer() const
		{
			JM_VISUAL_ASSERT(components.size() == attributes.size());
			byte_list rawData(elementCount * elementSize * sizeof(float));

			for (std::size_t elementIndex = 0; elementIndex < elementCount; ++elementIndex)
			{
				for (std::size_t attributeIndex = 0; attributeIndex < attributes.size(); ++attributeIndex)
				{
					const auto& attribute = attributes[attributeIndex];
					auto& componentData = components[attributeIndex];
					auto attributeWidth = attribute.size * sizeof(float);
					JM_VISUAL_ASSERT(componentData.size() == elementCount * attribute.size * sizeof(float));

					auto attributeOffset = attribute.offset * sizeof(float);
					auto elementOffset = elementIndex * elementSize * sizeof(float);
					memcpy(rawData.data() + attributeOffset + elementOffset, componentData.data() + elementIndex * attributeWidth, attributeWidth);
				}
			}

			return { rawData, elementCount };
		}
	};
}
