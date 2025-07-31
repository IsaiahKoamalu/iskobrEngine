#ifndef TILECOORD_H
#define TILECOORD_H

#include <cstdint>
#include <functional>

struct TileCoord {
    int x = 0;
    int y = 0;

    TileCoord() = default;
    TileCoord(int x_, int y_) : x(x_), y(y_) {}

    bool operator==(TileCoord const& o) const noexcept {
        return x == o.x && y == o.y;
    }

    bool operator!=(TileCoord const& o) const noexcept {
        return !(*this == o);
    }
};

namespace std {
    template<>
    struct hash<TileCoord> {
        size_t operator()(TileCoord const& t) const noexcept {
            return (static_cast<uint32_t>(t.x) << 16) ^ static_cast<uint32_t>(t.y);
        }
    };
}

#endif