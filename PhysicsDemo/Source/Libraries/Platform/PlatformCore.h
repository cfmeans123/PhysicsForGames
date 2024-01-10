#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstddef>
#include <vector>
#include <string>


#define JM_ON_WINDOWS 0
#define JM_ON_32BIT 0
#define JM_ON_64BIT 0

#ifdef _WIN32
#undef JM_ON_WINDOWS
#define JM_ON_WINDOWS 1
#	ifdef _WIN64
#undef JM_ON_64BIT
#define JM_ON_64BIT 1
#	else
#undef JM_ON_32BIT
#define JM_ON_32BIT 1
#	endif
#else
#	error "Unknown OS!"
#endif


#define bitsizeof(Type) \
static_cast<std::size_t>(sizeof(Type) * 8)


namespace jm
{
	constexpr bool OnWindows = static_cast<bool>(JM_ON_WINDOWS);
	constexpr bool On32bit = static_cast<bool>(JM_ON_32BIT);
	constexpr bool On64bit = static_cast<bool>(JM_ON_64BIT);

	using byte = std::byte;
	using byte_list = std::vector<std::byte>;

	using i8 = int8_t;
	using i16 = int16_t;
	using i32 = int32_t;
	using i64 = int64_t;

	static_assert(bitsizeof(i8) == 8);
	static_assert(bitsizeof(i16) == 16);
	static_assert(bitsizeof(i32) == 32);
	static_assert(bitsizeof(i64) == 64);

	using u8 = uint8_t;
	using u16 = uint16_t;
	using u32 = uint32_t;
	using u64 = uint64_t;

	static_assert(bitsizeof(u8) == 8);
	static_assert(bitsizeof(u16) == 16);
	static_assert(bitsizeof(u32) == 32);
	static_assert(bitsizeof(u64) == 64);

#if JM_ON_64BIT
	using iIndex = i32;
	using iSize = i64;
#else
	using iIndex = i16;
	using iSize = i32;
#endif

	using uSize = std::size_t;

	static_assert(sizeof(iSize) == sizeof(void*));
	static_assert(sizeof(uSize) == sizeof(void*));

	using f32 = float_t;
	using f64 = double_t;

	static_assert(bitsizeof(f32) == 32);
	static_assert(bitsizeof(f64) == 64);




	using cstring = char const*;

	inline std::string operator + (std::string& left, cstring right)
	{
		std::string sum(left);
		sum.append(right);
		return sum;
	}

	inline std::string operator + (cstring left, std::string& right)
	{
		std::string sum(left);
		sum.append(right);
		return sum;
	}

	//uses djb2 by Dan Bernstein 
	constexpr u64 HashStringTo64bitInteger(cstring hashedString)
	{
		u64 hash = 5381;
		i32 c = 0;

		while (static_cast<bool>(c = *hashedString++))
		{
			hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
		}

		return hash;
	}

	template <typename... Types>
	constexpr u32 StringLength(cstring format, Types... values)
	{
		return std::snprintf(nullptr, 0, format, values...) + 1;
	}

	constexpr bool StringsEqual(cstring p_A, cstring p_B)
	{
		return HashStringTo64bitInteger(p_A) == HashStringTo64bitInteger(p_B);
	}

	namespace Platform
	{

		struct ScreenPosition
		{
			iSize X = 0; //left position, increase right
			iSize Y = 0; //top position, increase down

			ScreenPosition offset(ScreenPosition other)
			{
				return { X - other.X, Y - other.Y };
			}
		};

		struct PixelArea
		{
			iSize Width = 100;
			iSize Height = 100;

			f32 GetAspectRatio() const
			{
				return static_cast<f32>(Width) / static_cast<f32>(Height);
			}
		};

	}
}