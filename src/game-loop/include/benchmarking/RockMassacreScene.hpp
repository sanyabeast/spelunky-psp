#pragma once

#include "components/generic/HorizontalOrientationComponent.hpp"
#include "components/generic/PhysicsComponent.hpp"
#include "components/generic/ActivableComponent.hpp"
#include "components/generic/ItemCarrierComponent.hpp"
#include "benchmarking/Scene.hpp"
#include "Level.hpp"
#include "TileBatch.hpp"
#include "MapTile.hpp"
#include "EntityRegistry.hpp"
#include "prefabs/effects/Explosion.hpp"
#include "other/ParticleGenerator.hpp"
#include "prefabs/items/Shotgun.hpp"
#include "prefabs/items/Arrow.hpp"
#include "prefabs/items/Rock.hpp"
#include "prefabs/npc/Caveman.hpp"

#include <vector>

class RockMassacreScene : public Scene
{
public:
    virtual void update(const FrameStats& frame_stats,
                        ModelViewCamera& model_view_camera,
                        ScreenSpaceCamera& screen_space_camera) override
    {
        model_view_camera.adjust_to_bounding_box(_level_center_x, _level_center_y);
        model_view_camera.adjust_to_level_boundaries(Consts::LEVEL_WIDTH_TILES, Consts::LEVEL_HEIGHT_TILES);
        model_view_camera.update_gl_modelview_matrix();

        auto& registry = EntityRegistry::instance().get_registry();

        //_arrow_spawn_timer_ms += frame_stats.total_delta_time_ms;

        if (_arrow_spawn_timer_ms > 1000)
        {
            _arrow_spawn_timer_ms = 0;

            for (int y = 1; y < Consts::LEVEL_HEIGHT_TILES - 1; y++)
            {
                if (!(y % 2))
                {
                    {
                        const auto arrow_entity = prefabs::Rock::create(Consts::LEVEL_WIDTH_TILES - 1, y + 0.5f);
                        auto &physics = registry.get<PhysicsComponent>(arrow_entity);
                        physics.set_x_velocity(-0.35);
                        physics.set_max_x_velocity(5);
                        physics.set_bounciness(0.99f);
                    }
                    {
                        const auto arrow_entity = prefabs::Rock::create(1, y + 0.5f);
                        auto &physics = registry.get<PhysicsComponent>(arrow_entity);
                        physics.set_x_velocity(0.35);
                        physics.set_max_x_velocity(5);
                        physics.set_bounciness(0.99f);
                    }
                }
            }
        }
    }

    void spawn_entities() override
    {
        auto& registry = EntityRegistry::instance().get_registry();
        auto& tile_batch = Level::instance().get_tile_batch();

        for (int x = 1; x < Consts::LEVEL_HEIGHT_TILES - 1; x++)
        {
            for (int y = 1; y < Consts::LEVEL_HEIGHT_TILES - 1; y++)
            {
                if (y % 2)
                {
                    tile_batch.map_tiles[x][y]->match_tile(MapTileType::CAVE_UP_DOWN_ORIENTED);
                }
                else
                {
                    if (x % 4 == 0)
                    {
                        prefabs::Caveman::create(x, y);
                    }
                }
            }
        }

        tile_batch.generate_frame();
        tile_batch.generate_cave_background();
        tile_batch.batch_vertices();
        tile_batch.add_render_entity(registry);
    }

    const char* scene_title() const override
    {
        return "Shotguns (benchmarking)";
    }
private:
    int _arrow_spawn_timer_ms{1001};
    const float _level_center_x = Consts::LEVEL_WIDTH_TILES / 2.f;
    const float _level_center_y = Consts::LEVEL_WIDTH_TILES / 2.f;
};