#pragma once

#include "entt/single_include/entt/entt.hpp"

namespace jm
{
	using entity_id = entt::entity;
	using entity_registry = entt::registry;
	constexpr auto null_entity_id = entt::null;
}