#ifndef TILESETMANAGER_H
#define TILESETMANAGER_H

#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <utility>

struct Tileset {
    std::string name;
    sf::Texture texture;
    int tileWidth{};
    int tileHeight{};
    int tilesPerRow{};

    Tileset() = default;

    Tileset(std::string name, const std::string &filepath, int tileW, int tileH)
        : name(std::move(name)), tileWidth(tileW), tileHeight(tileH) {
        if (!texture.loadFromFile(filepath)) {
            throw std::runtime_error("Failed to load tileset: " + filepath);
        }
        tilesPerRow = static_cast<int>(texture.getSize().x / tileWidth);
    }
};

class TilesetManager {
public:
    void addTileset(const std::string &name, const std::string &filepath, int tileW, int tileH) {
        tilesets.emplace(name, Tileset(name, filepath, tileW, tileH));
    }

    const Tileset &getTileset(const std::string &name) const {
        auto it = tilesets.find(name);
        if (it == tilesets.end()) {
            throw std::runtime_error("Tileset not found: " + name);
        }
        return it->second;
    }

    bool hasTileset(const std::string &name) const {
        return tilesets.find(name) != tilesets.end();
    }

private:
    std::unordered_map<std::string, Tileset> tilesets;
};

#endif
