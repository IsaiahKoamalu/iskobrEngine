#ifndef TILEMAPSYSTEM_H
#define TILEMAPSYSTEM_H

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <unordered_map>
#include "Engine/System.h"
#include "Engine/Components/TileComponent.h"
#include "Engine/Components/Position.h"
#include "Engine/EntityManager.h"
#include "Engine/ComponentManager.h"
#include "Engine/TilesetManager.h"

class TileMapSystem : public System {
public:
    int maxTileCount = 1000;

    static TileType getTileTypeFromID(int id) {
        switch (id) {
            case 0: return TileType::Grass;
            case 1: return TileType::Water;
            case 2: return TileType::Wall;

            default: return TileType::Empty;
        }
    }

    bool issSolidTile(TileType type) {
        switch (type) {
            case TileType::Water:
                return true;
            default:
                return false;
        }
    }

     bool loadMap(const std::string& filename,
                 ComponentManager& components,
                 EntityManager& entityManager,
                 TilesetManager& tilesetManager,
                 RenderSystem& renderSystem, // Pass render system by reference
                 CollisionSystem& collisionSystem,
                 float tileScale = 3.0f
    )
    {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open map file: " << filename << std::endl;
            return false;
        }

         std::string line;
int y = 0;

while (std::getline(file, line)) {
    std::istringstream rowStream(line);
    std::string token;
    int x = 0;

    while (std::getline(rowStream, token, ' ')) {
        if (token.empty() || token == "-" || token == "-1") {
            x++;
            continue;
        }

        try {
            size_t delim = token.find(':');
            if (delim == std::string::npos) {
                std::cerr << "Invalid token format: " << token << std::endl;
                x++;
                continue;
            }

            std::string tilesetName = token.substr(0, delim);
            int tileID = std::stoi(token.substr(delim + 1));

            if (!tilesetManager.hasTileset(tilesetName)) {
                std::cerr << "Tileset not found: " << tilesetName << std::endl;
                x++;
                continue;
            }

            const Tileset& ts = tilesetManager.getTileset(tilesetName);

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
            tileComp.type = getTileTypeFromID(tileID);
            tileComp.tileID = tileID;
            tileComp.sprite = sprite;
            tileComp.isSolid = (tilesetName == "water");

            if (tileComp.isSolid) {
                ColliderComponent collider;
                collider.bounds = {
                    -ts.tileWidth * tileScale / 2.f,
                    -ts.tileHeight * tileScale / 2.f,
                    ts.tileWidth * tileScale,
                    ts.tileHeight * tileScale
                };
                collider.isStatic = true;
                collider.tag = "Tile";

                components.addComponent<ColliderComponent>(tile, collider);
                collisionSystem.entities.insert(tile);
            }

            components.addComponent<TileComponent>(tile, tileComp);
            components.addComponent<Position>(tile, { sprite.getPosition().x, sprite.getPosition().y });
            renderSystem.entities.insert(tile);
        } catch (const std::exception& e) {
            std::cerr << "Invalid token: " << token << " (" << e.what() << ")\n";
        }

        x++;
    }

    y++;
}
        std::cout << "Map loaded: " << filename << std::endl;
        return true;
    }
};

#endif