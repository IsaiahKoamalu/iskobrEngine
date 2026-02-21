#include <iostream>
#include <map>

//Playing with different ideas of level instance generation.
class mapGenerator {
public:
    std::map<int, int> mapValues;
    mapGenerator();
    void generateMap(int width, int height)
    {
        for (int x = 0; x < width; x++)
        {
            for (int y = 0; y < height; y++)
            {
                if (x % 2 == 0 && y % 2 == 0)
                {
                    mapValues.insert(std::pair<int, int>(x, y));
                }
            }
        }
    };
};