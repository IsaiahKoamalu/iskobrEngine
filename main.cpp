#include <Engine/Engine.h>

int main() {
    bool debugMode = false;
    char input;
    std::cout << "RUN WITH VISIBLE COLLISION?(y/n):";
    std::cin >> input;
    if (input == 'y') debugMode = true;
    Engine engine;
    engine.run(debugMode);
    return 0;
}
