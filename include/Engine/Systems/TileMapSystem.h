#ifndef TILEMAPSYSTEM_H
#define TILEMAPSYSTEM_H

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <unordered_map>

#include "Engine/System.h"
#include "Engine/Core/UpdateContext.h"
#include "Engine/Components/TileComponent.h"
#include "Engine/Components/Position.h"
#include "Engine/EntityManager.h"
#include "Engine/ComponentManager.h"
#include "Engine/TilesetManager.h"
#include "external/json/json.hpp"

class TileMapSystem : public System {
public:
    int maxTileCount = 1000;

    static TileType getTileTypeFromID(int id, const std::string &tileset) {
        if (tileset == "grass") return TileType::Grass;
        if (tileset == "water") return TileType::Water;
        return TileType::Empty;
    }

    bool issSolidTile(TileType type) {
        switch (type) {
            case TileType::Water:
                return true;
            default:
                return false;
        }
    }

    bool loadMap(const std::string &filename,
                 ComponentManager &components,
                 EntityManager &entityManager,
                 TilesetManager &tilesetManager,
                 RenderSystem &renderSystem,
                 CollisionSystem &collisionSystem,
                 float tileScale = 3.0f)
    {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open map file: " << filename << std::endl;
            return false;
        }

        nlohmann::json j;
        file >> j;

        int width = j["width"];
        int height = j["height"];
        const auto &tileRows = j["tiles"];

        for (int y = 0; y < height; ++y) {
            const auto &row = tileRows[y];
            for (int x = 0; x < width; ++x) {
                const auto &cell = row[x];

                if (cell.is_null()) continue;

                std::string tilesetName = cell["sheet"];
                int tileID = cell["id"];

                if (!tilesetManager.hasTileset(tilesetName)) {
                    std::cerr << "Tileset not found: " << tilesetName << std::endl;
                    continue;
                }

                const Tileset &ts = tilesetManager.getTileset(tilesetName);

                Entity tile = entityManager.createEntity();
                sf::Sprite sprite;
                sprite.setTexture(ts.texture);

                int tx = tileID % ts.tilesPerRow;
                int ty = tileID / ts.tilesPerRow;

                sprite.setTextureRect(sf::IntRect(
                    tx * ts.tileWidth,
                    ty * ts.tileHeight,
                    ts.tileWidth,
                    ts.tileHeight
                ));
                sprite.setOrigin(ts.tileWidth / 2.f, ts.tileHeight / 2.f);
                sprite.setScale(tileScale, tileScale);
                sprite.setPosition(x * ts.tileWidth * tileScale + ts.tileWidth / 2.f,
                                   y * ts.tileHeight * tileScale + ts.tileHeight / 2.f);

                TileComponent tileComp;
                tileComp.type = getTileTypeFromID(tileID, tilesetName);
                tileComp.tileID = tileID;
                tileComp.sprite = sprite;
                tileComp.isSolid = (tilesetName == "grass"); // might try to pull from JSON if available later

                if (tileComp.isSolid) {
                    ColliderComponent collider;
                    collider.bounds = {
                        -ts.tileWidth * tileScale / 2.f,
                        (-ts.tileHeight * tileScale / 2.f) + 10,
                        ts.tileWidth * tileScale,
                        (ts.tileHeight * tileScale) - 10
                    };
                    collider.isStatic = true;
                    collider.tag = "Tile";

                    components.addComponent<ColliderComponent>(tile, collider);
                    collisionSystem.entities.insert(tile);
                }

                components.addComponent<TileComponent>(tile, tileComp);
                components.addComponent<Position>(tile, {sprite.getPosition().x, sprite.getPosition().y});
                renderSystem.entities.insert(tile);
            }
        }

        std::cout << "Map loaded from JSON: " << filename << std::endl;
        return true;
    }
    void update(const UpdateContext &ctxt) override{};
};

#endif
