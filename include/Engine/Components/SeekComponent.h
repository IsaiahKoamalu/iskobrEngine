#ifndef SEEKCOMPONENT_H
#define SEEKCOMPONENT_H

#include <cstdint>

struct SeekComponent {
    std::uint32_t targetEntityId;
    float speed = 100.f;
};

#endif