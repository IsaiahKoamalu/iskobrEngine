#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

const int TILE_SIZE = 16;
const int MAP_WIDTH = 40;
const int MAP_HEIGHT = 40;

int main() {
    sf::RenderWindow window(sf::VideoMode(MAP_WIDTH * TILE_SIZE, MAP_HEIGHT * TILE_SIZE), "Tilemap Editor");

    sf::Texture tileset;
    if (!tileset.loadFromFile("assets/Grass.png")) {
        return -1;
    }

    // Empty map initialization
    std::vector<std::vector<int>> map(MAP_HEIGHT, std::vector<int>(MAP_WIDTH, -1));
    std::string selectedType = "grass";
    int selectedTileID = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                int col = event.mouseButton.x / (TILE_SIZE* 3);
                int row = event.mouseButton.y / (TILE_SIZE * 3);
                if (row < MAP_HEIGHT && col < MAP_WIDTH) {
                    map[row][col] = selectedTileID;
                }
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::D) {
                    selectedTileID++;
                    std::cout << selectedTileID << std::endl;
                }
                if (event.key.code == sf::Keyboard::A) {
                    selectedTileID--;
                    std::cout << selectedTileID << std::endl;
                }
                if (event.key.code == sf::Keyboard::Enter) {
                    std::ofstream outFile("assets/maps/level1.txt");
                    for (const auto& row : map) {
                        for (size_t i = 0; i < row.size(); ++i) {
                            if (row[i] >= 0)
                                outFile << selectedType << ":" << row[i];
                            else
                                outFile << " ";
                            if (i < row.size() - 1) {outFile << ' ';}
                        }
                        outFile << '\n';
                    }
                    outFile.close();
                    std::cout << "Map saved to assets/maps/level1.txt\n";
                }
            }
        }

        window.clear();
        for (int y = 0; y < MAP_HEIGHT; ++y) {
            for (int x = 0; x < MAP_WIDTH; ++x) {
                int id = map[y][x];
                if (id >= 0) {
                    sf::Sprite tile(tileset);
                    tile.setTextureRect(sf::IntRect(id * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
                    tile.setPosition(x * TILE_SIZE * 3, y * TILE_SIZE * 3);
                    tile.setScale(3.0f, 3.0f);
                    window.draw(tile);
                }
            }
        }
        sf::Sprite previewTile(tileset);
        previewTile.setTextureRect(sf::IntRect(selectedTileID * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
        previewTile.setPosition(10, 10);
        previewTile.setScale(2.0f, 2.0f);
        window.draw(previewTile);
        window.display();
    }

    return 0;
};