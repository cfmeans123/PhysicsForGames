#include "Collision.h"
#include "Components.h"
#include "Math/Geometry.h"

namespace jm
{
    struct SphereCollider
    {
        entity_id entity;
        math::sphere3<f32> sphere;
    };
    struct BoxCollider
    {
        entity_id entity;
        math::box3<f32> box;
    };

    void resolve_collisions(entity_registry& registry)
    {
        //create colliders
        auto shape_entity_view = registry.view<const shape_component, const spatial3_component>();
       
        std::vector<SphereCollider> spheres;
        std::vector<BoxCollider> boxes;

        for (auto&& [entity, shape, spatial] : shape_entity_view.each())
        {
            switch (shape)
            {
            case shape_component::Sphere:
                spheres.push_back({ entity, math::sphere3{spatial.position, 1.0f} });  //assume unit radius spheres
                break;
            default:
                break;
            }
        }


        //check for collisions
        for (size_t idx = 0; idx < spheres.size(); ++idx)
        {
            auto& b = spheres[idx];
            for (size_t jdx = idx + 1; jdx < spheres.size(); ++jdx)
            {
                auto& a = spheres[jdx];
                if (math::intersect(a.sphere, b.sphere))
                {
                    //do something
                }
            }

            for (size_t jdx = idx + 1; jdx < boxes.size(); ++jdx)
            {
                auto& a = boxes[jdx];
                if (math::intersect(b.sphere, a.box))
                {
                    //do something
                }
            }

        }


        //resolve collisions
    }
}