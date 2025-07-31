#ifndef PATHFINDINGSYSTEM_H
#define PATHFINDINGSYSTEM_H

#include <queue>
#include <unordered_map>
#include <limits>

#include "Engine/ComponentManager.h"
#include "Engine/System.h"
#include "Engine/Components/NavigationComponent.h"
#include "Engine/Components/PathComponent.h"
#include "Engine/Components/Transform.h"
#include "Engine/Core/UpdateContext.h"

struct Edge {
    TileCoord to;
    float cost;
    StepAction action;
};

struct Node {
    TileCoord pos;
    float g;
    float f;
    TileCoord parent;
};

static float heuristic(const TileCoord& a, const TileCoord& b) {
    float dx = float(a.x - b.x);
    float dy = float(a.y - b.y);
    return std::sqrt(dx*dx + dy*dy);
}


class PathfindingSystem : public System {
public:
    PathfindingSystem(
        const std::unordered_map<TileCoord, std::vector<Edge>>& graph)
            : graph_(graph) {}

    void update(const UpdateContext& ctxt) override {
        auto& components = *ctxt.component;
        for (auto entity : entities) {
            auto& nav = components.getComponent<NavigationComponent>(entity);
            auto& path = components.getComponent<PathComponent>(entity);
            if (!nav.needsRepath) continue;

            TileCoord start;
            TileCoord goal = nav.targetTile;

            std::priority_queue<
                std::pair<float, TileCoord>,
                std::vector<std::pair<float, TileCoord>>,
                std::greater<>> openSet;
            std::unordered_map<TileCoord, float> gScore;
            std::unordered_map<TileCoord, TileCoord> cameFrom;

            for (auto& kv : graph_) {
                gScore[kv.first] = std::numeric_limits<float>::infinity();
            }
            gScore[start] = 0;
            openSet.emplace(heuristic(start, goal), start);
            while (!openSet.empty()) {
                auto [f, current] = openSet.top();
                openSet.pop();
                if (current == goal) break;

                for (auto& edge : graph_.at(current)) {
                    float tentative = gScore[current] + edge.cost;
                    if (tentative < gScore[edge.to]) {
                        cameFrom[edge.to] = current;
                        gScore[edge.to] = tentative;
                        float fscore = tentative + heuristic(edge.to, goal);
                        openSet.emplace(fscore, edge.to);
                    }
                }
            }
            // Path reconstruction
            std::vector<PathStep> steps;
            TileCoord current = goal;
            while (current != start) {
                TileCoord previous = cameFrom[current];
                for (auto& edge : graph_.at(previous)) {
                    if (edge.to == current) {
                        steps.push_back({current, edge.action});
                        break;
                    }
                }
                current = previous;
            }
            std::reverse(steps.begin(), steps.end());

            path.steps = std::move(steps);
            path.currentIndex = 0;
            nav.needsRepath = false;
        }
    }

private:
    const std::unordered_map<TileCoord, std::vector<Edge>>& graph_;
};

#endif