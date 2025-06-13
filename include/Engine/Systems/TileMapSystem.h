#ifndef TILEMAPSYSTEM_H
#define TILEMAPSYSTEM_H

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include "Engine/System.h"
#include "Engine/Components/TileComponent.h"
#include "Engine/Components/Position.h"
#include "Engine/EntityManager.h"
#include "Engine/ComponentManager.h"

class TileMapSystem : public System {
public:
     bool loadMap(const std::string& filename,
                 ComponentManager& components,
                 EntityManager& entityManager,
                 sf::Texture& tilesheet,
                 int tileSize,
                 int tilesPerRow,
                 RenderSystem& renderSystem) // Pass render system by reference
    {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open map file: " << filename << std::endl;
            return false;
        }

        std::string line;
        int y = 0;

        while (std::getline(file, line)) {
            for (int x = 0; x < static_cast<int>(line.size()); ++x) {
                char tileChar = line[x];

                if (tileChar < '0' || tileChar > '9')
                    continue;

                int tileID = tileChar - '0';

                Entity tile = entityManager.createEntity();

                // Create and set up the tile sprite
                sf::Sprite sprite;
                sprite.setTexture(tilesheet);

                int tx = tileID % tilesPerRow;
                int ty = tileID / tilesPerRow;

                sprite.setTextureRect(sf::IntRect(tx * tileSize, ty * tileSize, tileSize, tileSize));
                sprite.setScale(3,3);
                sprite.setOrigin(tileSize / 2.f, tileSize / 2.f);
                sprite.setPosition(x * tileSize + tileSize / 2.f, y * tileSize + tileSize / 2.f);

                // Create and assign tile component
                TileComponent tileComp;
                tileComp.tileID = tileID;
                tileComp.sprite = sprite;
                tileComp.isSolid = (tileID == 1); // 1 = wall

                components.addComponent<TileComponent>(tile, tileComp);

                // Position is center-aligned (to match sprite)
                components.addComponent<Position>(tile, {
                    sprite.getPosition().x,
                    sprite.getPosition().y
                });

                std::cout << "Spawning tile ID: " << tileID << " at (" << x << ", " << y << ")\n";


                // Register tile entity with render system
                renderSystem.entities.insert(tile);
            }
            y++;
        }

        std::cout << "Map loaded: " << filename << std::endl;
        return true;
    }
};

#endif