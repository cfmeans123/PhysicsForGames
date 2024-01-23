#pragma once

#include "Visual.h"

namespace jm::Visual
{
	RawBuffer GenerateCoordinateAxes2(const InputLayout& layout);

	RawBuffer GenerateBox(const InputLayout& layout, f32 diameter = 2.0f);

	RawBuffer GenerateDisk(const InputLayout& layout, f32 diameter = 2.0f);

	RawBuffer GeneratePlane(const InputLayout& layout);

	RawBuffer GenerateCoordinateAxes3(const InputLayout& layout);

	RawBuffer GenerateCube(const InputLayout& layout, f32 diameter = 2.0f);

	RawBuffer GenerateSphere(const InputLayout& layout, f32 diameter = 2.0f);
}
