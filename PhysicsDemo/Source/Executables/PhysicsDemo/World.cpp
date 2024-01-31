#include "World.h"

#include "Systems/Entity.h"
#include "Systems/Components.h"

#include "Random.h"

namespace jm
{
	void AddSphereEntity(entity_registry& registry, math::vector3_f32 const& position, math::quaternion_f32 const& rotation)
	{
		entity_id e = registry.create();
		registry.emplace<spatial3_component>(e,position, rotation);
		registry.emplace<shape_component>(e, shape_component::Sphere);
	}

	void AddBoxEntity(entity_registry& registry, math::vector3_f32 const& position, math::quaternion_f32 const& rotation, math::vector3_f32 const& extents = math::zero3)
	{
		extents;
		entity_id e = registry.create();
		registry.emplace<spatial3_component>(e, position, rotation);
		registry.emplace<shape_component>(e, shape_component::Box);
	}

	void CreateBasicWorld(entity_registry& registry)
	{
		AddSphereEntity(registry, 8.0f * math::random::unit_ball<f32>(), math::random::unit_quaternion<f32>());
		AddSphereEntity(registry, 8.0f * math::random::unit_ball<f32>(), math::random::unit_quaternion<f32>());
		AddSphereEntity(registry, 8.0f * math::random::unit_ball<f32>(), math::random::unit_quaternion<f32>());
		AddSphereEntity(registry, 8.0f * math::random::unit_ball<f32>(), math::random::unit_quaternion<f32>());
		
		AddBoxEntity(registry, 8.0f * math::random::unit_ball<f32>(), math::random::unit_quaternion<f32>());
		AddBoxEntity(registry, 8.0f * math::random::unit_ball<f32>(), math::random::unit_quaternion<f32>());
		AddBoxEntity(registry, 8.0f * math::random::unit_ball<f32>(), math::random::unit_quaternion<f32>());
	}
}
