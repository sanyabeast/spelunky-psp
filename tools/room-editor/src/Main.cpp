#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>

#include "raylib.h"
#include "MapTileType.hpp"
#include "MapTileToString.hpp"
#include "Serialize.hpp"

namespace
{
    Texture2D tiles[static_cast<std::size_t>(MapTileType::_SIZE)];
    std::size_t selected_tile_index = -1;
    Camera2D menu_camera = { 0 };
    Camera2D workspace_grid_camera = { 0 };
    Vector2 mouse_point = { 0.0f, 0.0f };

    const std::size_t grid_width = 10;
    const std::size_t grid_height = 10;

    MapTileType grid[grid_width][grid_height] = {MapTileType::NOTHING};
    const float tile_width = 16;
    const float tile_height = 16;
    const int screenWidth = 1280;
    const int screenHeight = 720;

    bool has_selected_tile() { return selected_tile_index != -1; }
    Texture2D& get_selected_tile_texture() { assert(has_selected_tile()); return tiles[selected_tile_index]; }
    Texture2D& get_tile_texture(MapTileType type) { return tiles[static_cast<std::size_t>(type)]; }

    void load_tiles(const std::string& assetsPath)
    {
        for (auto index = static_cast<std::size_t>(MapTileType::NOTHING); index < static_cast<std::size_t>(MapTileType::_SIZE); index++)
        {
            const std::string path = assetsPath + "/assets/tilesheets/level-tiles/" + std::to_string(index) + ".png";
            std::cout << "Loading tile from: " << path << std::endl;

            auto& tile = tiles[index];

            tile = LoadTexture(path.c_str());
            std::cout << "Params: " << tile.width << ", " << tile.height << ", " << tile.id << std::endl;
        }
    }

    void display_menu()
    {
        Vector2 position {0, 0}; // By convention, it's a left-upper corner of the rectangle

        std::size_t index = 0;

        for (const auto& tile : tiles)
        {
            Rectangle dimensions{0, 0, tile_width, tile_height};

            if (CheckCollisionPointRec(mouse_point, {menu_camera.zoom * position.x, menu_camera.zoom * position.y,
                                                     menu_camera.zoom * dimensions.width, menu_camera.zoom * dimensions.height}))
            {
                selected_tile_index = index;

                if (IsMouseButtonDown(MouseButton::MOUSE_LEFT_BUTTON))
                {
                    DrawTextureRec(tile, {0, 0, dimensions.width * 1.1f, dimensions.height * 1.1f}, position, BLUE);
                }
                else
                {
                    DrawTextureRec(tile, {0, 0, dimensions.width * 1.1f, dimensions.height * 1.1f}, position, GREEN);
                }
            }
            else
            {
                DrawTextureRec(tile, dimensions, position, WHITE);
            }

            if (position.x == 0)
            {
                position.x += (tile_width + 2);
            }
            else
            {
                position.y += (tile_height + 2);
                position.x = 0;
            }

            index++;
        }
    }

    void display_workspace_grid()
    {
        auto& camera = workspace_grid_camera;
        Rectangle dimensions{0, 0, tile_width, tile_height};
        Vector2 position;

        for (std::size_t x_index = 0; x_index < grid_width; x_index++)
        {
            for (std::size_t y_index = 0; y_index < grid_height; y_index++)
            {
                position = {x_index * tile_width, y_index * tile_height};

                auto& tile = get_tile_texture(grid[x_index][y_index]);

                if (CheckCollisionPointRec(mouse_point, {camera.offset.x + (camera.zoom * position.x), camera.offset.y + (camera.zoom * position.y),
                                                         camera.zoom * dimensions.width, camera.zoom * dimensions.height}))
                {
                    if (IsMouseButtonDown(MouseButton::MOUSE_LEFT_BUTTON))
                    {
                        if (has_selected_tile())
                        {
                            grid[x_index][y_index] = static_cast<MapTileType>(selected_tile_index);
                            auto& selected_tile_texture = get_selected_tile_texture();
                            DrawTextureRec(selected_tile_texture, {0, 0, dimensions.width * 1.1f, dimensions.height * 1.1f}, position, BLUE);
                        }
                        else
                        {
                            DrawTextureRec(tile, {0, 0, dimensions.width * 1.1f, dimensions.height * 1.1f}, position, BLUE);
                        }
                    }
                    else
                    {
                        DrawTextureRec(tile, {0, 0, dimensions.width * 1.1f, dimensions.height * 1.1f}, position, GREEN);
                    }
                }
                else
                {
                    DrawTextureRec(tile, dimensions, position, WHITE);
                }
            }
        }
    }
}

int main()
{
    InitWindow(screenWidth, screenHeight, "SpelunkyPSP room editor");

    load_tiles("/home/dbeef/Desktop/spelunky-psp/"); // FIXME: Maybe pass via cmdline args? With fallback via relative paths?

    menu_camera.target = {};
    menu_camera.offset = (Vector2){ 2, 2 };
    menu_camera.rotation = 0.0f;
    menu_camera.zoom = 1.6f;

    workspace_grid_camera.target = {};
    workspace_grid_camera.offset = (Vector2){ screenWidth / 4, screenHeight / 4 };
    workspace_grid_camera.rotation = 0.0f;
    workspace_grid_camera.zoom = 3.0f;

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        mouse_point = GetMousePosition();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode2D(menu_camera);
        display_menu();
        EndMode2D();

        BeginMode2D(workspace_grid_camera);
        display_workspace_grid();
        EndMode2D();

        EndDrawing();
    }

    std::ofstream out("out.hpp", std::ofstream::out);
    serialize(out, grid);

    CloseWindow();
    return EXIT_SUCCESS;
}