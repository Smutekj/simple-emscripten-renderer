#include "GridNeighbourSearcher.h"

GridNeighbourSearcher::GridNeighbourSearcher(float max_dist, utils::Vector2f center_pos, utils::Vector2f box_size)
{
    const utils::Vector2i n_cells = {static_cast<int>(box_size.x / max_dist) + 1,
                                     static_cast<int>(box_size.y / max_dist) + 1};
    assert(n_cells.x >= 3);
    assert(n_cells.y >= 3); //! seems kind of pointless to use 2x2 boxes..

    m_grid = std::make_unique<utils::SearchGrid>(n_cells, utils::Vector2f{max_dist, max_dist});
    m_grid2entities.resize(n_cells.x * n_cells.y);

    m_center_pos = {0};
}

void GridNeighbourSearcher::moveEntity(utils::Vector2f pos, int entity)
{
    auto old_grid_ind = m_entity2grid_ind.at(entity);
    auto new_grid_ind = m_grid->coordToCell(pos);
    if (new_grid_ind != old_grid_ind)
    {
        removeEntity(entity);
        insertEntityAt(entity, new_grid_ind);
    }
}

void GridNeighbourSearcher::removeEntity(int entity_ind)
{
    auto grid_ind = m_entity2grid_ind.at(entity_ind);
    m_grid2entities.at(grid_ind).erase(entity_ind);
    m_entity2grid_ind.erase(entity_ind);
}

void GridNeighbourSearcher::insertEntity(utils::Vector2f pos, int entity)
{
    insertEntityAt(entity, m_grid->coordToCell(pos));
}

void GridNeighbourSearcher::insertEntityAt(int entity, int grid_ind)
{
    if(grid_ind == -1)
    {
        return;
    }
    //! there should be no entity of this id existing
    assert(m_entity2grid_ind.count(entity) == 0);
    assert(m_grid2entities.at(grid_ind).count(entity) == 0);

    m_grid2entities.at(grid_ind)[entity] = entity;
    m_entity2grid_ind[entity] = grid_ind;
}

void GridNeighbourSearcher::setCenterPos(const utils::Vector2f &new_pos)
{
    m_center_pos = new_pos;
}

int GridNeighbourSearcher::calcGridIndex(const utils::Vector2f &absolute_pos) const
{
    auto grid_pos = absolute_pos - (m_center_pos - m_grid->getSize() / 2.f);
    if (!m_grid->isInGrid(grid_pos))
    {
        return -1;
    }
    auto cell_index = m_grid->coordToCell(grid_pos);
    assert(cell_index >= 0 && cell_index < m_grid->getNCells());
    return cell_index;
}

std::vector<int> GridNeighbourSearcher::getNeighboursOfExcept(utils::Vector2f center,
                                                              const std::vector<utils::Vector2f> &positions,
                                                              float radius, int exception) const
{
    std::vector<int> nearest_neighbours;
    int grid_ind = m_grid->coordToCell(center);
    if (grid_ind == -1)
    {
        return {}; //! query is outside of the grid so we don't return anything
    }

    auto radius_sq = radius * radius;

    std::array<int, 9> nearest_cells;
    int n_nearest_cells;
    m_grid->calcNearestCells(grid_ind, nearest_cells, n_nearest_cells);
    nearest_cells[n_nearest_cells] = grid_ind;
    n_nearest_cells++;

    for (int i = 0; i < n_nearest_cells; ++i)
    {
        const auto &neighbours = m_grid2entities.at(nearest_cells.at(i));
        for (auto [neighbour_id, neighbour] : neighbours)
        {
            bool is_in_range = utils::dist(positions.at(neighbour), center) < radius;
            if (is_in_range && neighbour_id != exception)
            {
                nearest_neighbours.push_back(neighbour);
            }
        }
    }
    return nearest_neighbours;
}

std::vector<int> GridNeighbourSearcher::getNeighboursOf(utils::Vector2f center, const std::vector<utils::Vector2f> &positions, float radius) const
{
    return getNeighboursOfExcept(center, positions, radius, -1);
}
