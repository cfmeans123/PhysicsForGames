#include "Simulation.h"
#include "Physics.h"

#include "MathTypes.h"
#include "Components.h"

namespace jm
{
	constexpr f32 Damping = 0.9995f;
	constexpr math::vector3<f32> Gravity = { 0.f, -9.81f, 0.f };
	constexpr math::vector2<f32> Gravity2 = { Gravity.x, Gravity.y};

	void integrate(entity_registry& registry, f32 delta_time)
	{
		{
			auto lin_sim_view = registry.view<spatial2_component, linear_body2_component>();
			for (auto&& [entity, spatial, linear] : lin_sim_view.each())
			{
				math::vector2_f32 acceleration = Gravity2 + linear.applied_force * linear.inverse_mass;
				math::euler_integration(linear.velocity, acceleration, deltaTime);
				linear.velocity *= Damping;
				math::euler_integration(spatial.position, linear.velocity, deltaTime);
			}
		}
		{
			auto lin_sim_view = registry.view<spatial3_component, linear_body3_component>();
			for (auto&& [entity, spatial, linear] : lin_sim_view.each())
			{
				math::vector3_f32 acceleration = Gravity + linear.applied_force * linear.inverse_mass;
				math::euler_integration(linear.velocity, acceleration, deltaTime);
				linear.velocity *= Damping;
				math::euler_integration(spatial.position, linear.velocity, deltaTime);
			}
		}
	}
}