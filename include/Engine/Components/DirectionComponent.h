#ifndef DIRECTIONCOMPONENT_H
#define DIRECTIONCOMPONENT_H

enum class Direction {
    Up,
    Down,
    Left,
    Right
};

struct DirectionComponent {
    Direction current = Direction::Down; // Set down as default
};

#endif