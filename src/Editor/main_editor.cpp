#include <SFML/Graphics.hpp>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include "external/json/json.hpp"

using json = nlohmann::json;

std::unordered_map<std::string, int> nameToID;
std::unordered_map<int, std::string> idToName;

void loadTilesetData(const std::string& path) {
    std::ifstream file(path);
    std::cout << "Trying to open: " << std::filesystem::absolute(path) << "\n";

    if (!file.is_open()) {
        std::cerr << "Failed to open: " << path << "\n";
        return;
    }
    json tilesetData;
    file >> tilesetData;

    for (const auto& [name, data] : tilesetData.items()) {
        int id = data["id"];
        nameToID[name] = id;
        idToName[id] = name;
    }
}

const int TILE_SIZE = 16;
const int SCALE = 3;
const int SCALED_TILE_SIZE = TILE_SIZE * SCALE;
const int MAP_WIDTH = 20;
const int MAP_HEIGHT = 15;

struct Tile {
    int sheetID = -1;
    int tileID = -1;
};

void saveMapToFile(const std::vector<std::vector<Tile>>& map, const std::string& filename, std::string tileType) {
    std::ofstream outFile(filename);
    for (const auto& row : map) {
        for (size_t i = 0; i < row.size(); ++i) {
            if (row[i].tileID >= 0)
                outFile << idToName[row[i].sheetID] << ":" << row[i].tileID;
            else
                outFile << " ";
            if (i < row.size() - 1) {outFile << ' ';}
        }
        outFile << "\n";
    }
    outFile.close();
    std::cout << "Map saved to: " << filename << std::endl;
}

int main() {

    loadTilesetData("assets/tilesets.json");
    sf::RenderWindow window(sf::VideoMode(1600, 800), "Tilemap Editor");
    std::unordered_map<int, sf::Texture> tileSheets;
    sf::Texture grassSheet;
    sf::Texture waterSheet;
    if (!grassSheet.loadFromFile("assets/Grass.png")) {
        std::cerr << "Failed to load tileset.\n";
        return -1;
    }
    if (!waterSheet.loadFromFile("assets/Water.png")) {
        std::cerr << "Failed to load: assets/Water.png\n";
    }
    tileSheets[0] = grassSheet;
    tileSheets[1] = waterSheet;

    // Calculate columns and rows from the tilesheet
    int tileCols = grassSheet.getSize().x / TILE_SIZE;
    int tileRows = grassSheet.getSize().y / TILE_SIZE;

    int selectedTileSheetID = 0;
    int selectedTileID = 0;
    std::string selectedType = "grass";
    std::vector<std::vector<Tile>> tilemap(MAP_HEIGHT, std::vector<Tile>(MAP_WIDTH));

    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::E) {
                selectedTileSheetID ++;
                std::cout << selectedTileID << std::endl;
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Q) {
                selectedTileSheetID --;
                std::cout << selectedTileID << std::endl;
            }

            // Handle tile selection (click on the side panel)
            if (event.type == sf::Event::MouseButtonPressed) {
                int mouseX = event.mouseButton.x;
                int mouseY = event.mouseButton.y;

                // Click on map area
                if (mouseX < MAP_WIDTH * SCALED_TILE_SIZE) {
                    int gridX = mouseX / SCALED_TILE_SIZE;
                    int gridY = mouseY / SCALED_TILE_SIZE;

                    if (gridX >= 0 && gridX < MAP_WIDTH && gridY >= 0 && gridY < MAP_HEIGHT) {
                        tilemap[gridY][gridX] = { selectedTileSheetID, selectedTileID };
                    }
                } else {
                    // Click on tile selector (right panel)
                    int tileX = (mouseX - MAP_WIDTH * SCALED_TILE_SIZE) / SCALED_TILE_SIZE;
                    int tileY = mouseY / SCALED_TILE_SIZE;

                    if (tileX >= 0 && tileX < tileCols && tileY >= 0 && tileY < tileRows) {
                        selectedTileID = tileY * tileCols + tileX;
                    }
                }
            }

            // Press S to save map
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S) {
                saveMapToFile(tilemap, "assets/maps/level1.txt", selectedType);
            }
        }

        window.clear();

        // Draw tilemap
        for (int y = 0; y < MAP_HEIGHT; ++y) {
            for (int x = 0; x < MAP_WIDTH; ++x) {
                Tile tileData = tilemap[y][x];
                if (tileData.sheetID >= 0 && tileData.tileID >= 0) {
                    int tu = tileData.tileID % tileCols;
                    int tv = tileData.tileID / tileCols;

                    sf::Sprite tile(tileSheets[tileData.sheetID]);
                    tile.setTextureRect(sf::IntRect(tu * TILE_SIZE, tv * TILE_SIZE, TILE_SIZE, TILE_SIZE));
                    tile.setScale(SCALE, SCALE);
                    tile.setPosition(x * SCALED_TILE_SIZE, y * SCALED_TILE_SIZE);
                    window.draw(tile);
                }
            }
        }

        // Draw tile selector on the right
        for (int i = 0; i < tileCols * tileRows; ++i) {
            int tu = i % tileCols;
            int tv = i / tileCols;

            sf::Sprite tile(tileSheets[selectedTileSheetID]);
            tile.setTextureRect(sf::IntRect(tu * TILE_SIZE, tv * TILE_SIZE, TILE_SIZE, TILE_SIZE));
            tile.setScale(SCALE, SCALE);
            tile.setPosition(MAP_WIDTH * SCALED_TILE_SIZE + tu * SCALED_TILE_SIZE,
                             tv * SCALED_TILE_SIZE);
            window.draw(tile);
        }

        // Draw selected tile preview
        int previewX = selectedTileID % tileCols;
        int previewY = selectedTileID / tileCols;

        sf::RectangleShape outline(sf::Vector2f(SCALED_TILE_SIZE, SCALED_TILE_SIZE));
        outline.setFillColor(sf::Color::Transparent);
        outline.setOutlineColor(sf::Color::Red);
        outline.setOutlineThickness(2.f);
        outline.setPosition(MAP_WIDTH * SCALED_TILE_SIZE + previewX * SCALED_TILE_SIZE,
                            previewY * SCALED_TILE_SIZE);
        window.draw(outline);

        window.display();
    }

    return 0;
}
