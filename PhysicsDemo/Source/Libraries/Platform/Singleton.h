#pragma once

#include "PlatformDebug.h"

#include <memory>
#include <concepts>

namespace jm
{
	template <typename T>
	concept nonmovable = !std::movable<T>;

	template <nonmovable T>
	class singleton
	{
		using this_type = singleton<T>;
		static std::atomic_flag s_instance_flag;

	protected:

		singleton()
		{
			if constexpr (Platform::IsDebug)
			{
				bool isInstanced = std::atomic_flag_test_and_set(&this_type::s_instance_flag);
				JM_PLATFORM_ASSERT(!isInstanced, "Previous instance created, check if the flag was cleared!");
			}
			else
			{
				std::atomic_flag_test_and_set(&this_type::s_instance_flag);
			}
		}

		virtual ~singleton()
		{
			JM_PLATFORM_ASSERT(std::atomic_flag_test(&this_type::s_instance_flag), "Singleton was never constructed!");
			std::atomic_flag_clear(&this_type::s_instance_flag);
		}

	private:

		singleton(singleton&) = delete;
		singleton& operator=(singleton&) = delete;
		singleton(singleton&&) = delete;
		singleton& operator=(singleton&&) = delete;
	};
}