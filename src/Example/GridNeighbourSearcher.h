#pragma once

#include "Grid.h"

#include <vector>
#include <unordered_map>
#include <memory>

#include <Utils/Vector2.h>

class GridNeighbourSearcher
{

public:
    GridNeighbourSearcher(float max_dist = 30,
                          utils::Vector2f center_pos = {500, 500},
                          utils::Vector2f box_size = {1000, 1000});

    void moveEntity(utils::Vector2f pos, int entity_ind);
    void removeEntity(int entity_ind);
    void insertEntity(utils::Vector2f pos, int entity_ind);
    void insertEntityAt(int entity_ind, int grid_ind);
    void setCenterPos(const utils::Vector2f &new_pos);
    int calcGridIndex(const utils::Vector2f &pos) const;

    void clear()
    {
        m_entity2grid_ind.clear();
        std::for_each(m_grid2entities.begin(), m_grid2entities.end(), [](auto &ents)
                      { ents.clear(); });
    }

    std::vector<int> getNeighboursOfExcept(utils::Vector2f center,
                                           const std::vector<utils::Vector2f> &positions,
                                           float radius, int exception) const;
    std::vector<int> getNeighboursOf(utils::Vector2f center,
                                     const std::vector<utils::Vector2f> &positions, float radius) const;

private:
    std::unique_ptr<utils::SearchGrid> m_grid;

    utils::Vector2f m_center_pos;

    std::vector<std::unordered_map<int, int>> m_grid2entities;
    std::unordered_map<int, int> m_entity2grid_ind;
};
