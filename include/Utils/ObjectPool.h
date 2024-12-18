
#pragma once

#include <array>
#include <vector>
#include <unordered_set>
#include <set>
#include <cassert>

template <typename DataType, int MAX_OBJECTS = 5000>
struct ObjectPool
{

    std::array<DataType, MAX_OBJECTS> objects;

    std::array<int, MAX_OBJECTS> entity2ind;

    std::vector<int> active_inds;

    std::set<int> free_inds;

    ObjectPool()
    {
        for (int i = 0; i < MAX_OBJECTS; ++i)
        {
            free_inds.insert(i);
        }
        entity2ind.fill(-1);
    }

    int addObject(DataType &obj)
    {
        auto new_entity_ind = *free_inds.begin();

        assert(entity2ind.at(new_entity_ind) == -1);
        assert(new_entity_ind < MAX_OBJECTS);
        free_inds.erase(free_inds.begin());

        objects.at(new_entity_ind) = std::move(obj);

        entity2ind.at(new_entity_ind) = active_inds.size();
        active_inds.push_back(new_entity_ind);
        return new_entity_ind;
    }
    int addObject(DataType &&obj)
    {
        auto new_entity_ind = *free_inds.begin();

        assert(entity2ind.at(new_entity_ind) == -1);
        assert(new_entity_ind < MAX_OBJECTS);
        free_inds.erase(free_inds.begin());

        objects.at(new_entity_ind) = std::move(obj);

        entity2ind.at(new_entity_ind) = active_inds.size();
        active_inds.push_back(new_entity_ind);
        return new_entity_ind;
    }

    void remove(int entity_ind)
    {

        free_inds.insert(entity_ind);
        assert(entity2ind.at(entity_ind) != -1);

        auto vec_ind = entity2ind.at(entity_ind);
        active_inds.at(vec_ind) = active_inds.back();
        entity2ind.at(active_inds.back()) = vec_ind;
        active_inds.pop_back();
        entity2ind.at(entity_ind) = -1;
    }

    DataType &at(int entity_ind)
    {
        assert(entity_ind < MAX_OBJECTS && entity2ind.at(entity_ind) != -1);
        return objects.at(entity_ind);
    }
};

template <typename DataType, int MAX_OBJECTS>
struct DynamicObjectPool
{

public:
    DynamicObjectPool()
    {
        for (int i = 0; i < MAX_OBJECTS; ++i)
        {
            free_inds.insert(i);
        }
        entity2ind.fill(-1);
    }

    template <class T>
    int addObject(T &&obj)
    {
        assert(!free_inds.empty()); //! there is at least one object!
        auto new_entity_ind = *free_inds.begin();
        free_inds.erase(free_inds.begin());

        assert(entity2ind.at(new_entity_ind) == -1);
        assert(new_entity_ind < MAX_OBJECTS);

        objects.push_back(std::forward<T>(obj));

        object2entity.push_back(new_entity_ind);
        entity2ind.at(new_entity_ind) = objects.size() - 1;
        return new_entity_ind;
    }

    void remove(int entity_ind)
    {
        free_inds.insert(entity_ind);
        assert(entity2ind.at(entity_ind) != -1);

        auto vec_ind = entity2ind.at(entity_ind);
        object2entity.at(vec_ind) = object2entity.back();
        entity2ind.at(object2entity.back()) = vec_ind;
        object2entity.pop_back();

        objects.at(vec_ind) = objects.back();
        objects.pop_back();

        entity2ind.at(entity_ind) = -1;
    }

    DataType &at(int entity_ind)
    {
        assert(entity_ind < MAX_OBJECTS && entity2ind.at(entity_ind) != -1);
        return objects.at(entity2ind.at(entity_ind));
    }

    std::vector<DataType> &getObjects()
    {
        return objects;
    }
    std::vector<int> &getEntityIds()
    {
        return object2entity;
    }
    void clear()
    {
        for (auto &ind : object2entity)
        {
            free_inds.insert(ind);
            entity2ind.at(ind) = -1;
        }

        objects.clear();
        object2entity.clear();
    }

private:
    std::vector<int> object2entity;
    std::vector<DataType> objects;
    std::array<int, MAX_OBJECTS> entity2ind;
    std::set<int> free_inds;
};

template <typename Type>
class VectorMap
{

public:
    VectorMap() = default;
    explicit VectorMap(int n_max_entities);

    void removeByDataInd(int data_ind);
    void removeByEntityInd(int entity_ind);
    size_t insert(Type datum);
    size_t size();
    Type &getEntity(int index);
    void clear();
    void setMaxCount(int n_max_count);
    std::vector<Type> &getData();
    size_t getEntityInd(int data_ind) const
    {
        return m_data2entity_ind.at(data_ind);
    }
    size_t capacity();

private:
    size_t n_max_entities = 0;
    std::vector<int> m_entity2data_ind;
    std::vector<int> m_data2entity_ind;
    std::vector<Type> m_data;
    std::set<int> free_inds;
    size_t n_active = 0;
};

template <class T>
size_t VectorMap<T>::capacity()
{
    return n_max_entities;
}

template <class T>
std::vector<T> &VectorMap<T>::getData()
{
    return m_data;
}

template <class T>
VectorMap<T>::VectorMap(int n_max_entities)
    : n_max_entities(n_max_entities),
      m_entity2data_ind(n_max_entities, -1),
      m_data2entity_ind(n_max_entities, -1),
      m_data(n_max_entities)
{
    for (int i = 0; i < n_max_entities; ++i)
    {
        free_inds.insert(i);
    }
}

template <class T>
void VectorMap<T>::setMaxCount(int n_max_count)
{
    n_max_entities = n_max_count;
}

template <class T>
void VectorMap<T>::removeByDataInd(int data_ind)
{
    assert(n_active > 0);
    auto entity_ind = m_data2entity_ind.at(data_ind);

    assert(entity_ind != -1);
    assert(data_ind != -1);

    free_inds.insert(entity_ind);

    m_entity2data_ind[entity_ind] = -1;
    m_entity2data_ind.at(m_data2entity_ind.at(n_active - 1)) = data_ind;

    m_data[data_ind] = m_data[n_active - 1];
    m_data2entity_ind[n_active - 1] = -1;

    n_active--;
}

template <class T>
void VectorMap<T>::removeByEntityInd(int entity_ind)
{
    assert(free_inds.count(entity_ind) == 0);
    assert(n_active > 0);

    free_inds.insert(entity_ind);

    assert(entity_ind != -1);
    auto data_ind = m_entity2data_ind.at(entity_ind);
    assert(data_ind != -1);

    auto moved_entity_ind = m_data2entity_ind.at(n_active - 1);
    if (data_ind == -1)
    {
        throw std::runtime_error("Trying to delete non-existent object!");
    }
    m_entity2data_ind.at(moved_entity_ind) = data_ind;
    m_entity2data_ind.at(entity_ind) = -1;

    m_data.at(data_ind) = m_data.at(n_active - 1);
    m_data2entity_ind.at(data_ind) = moved_entity_ind;
    m_data2entity_ind.at(n_active - 1) = -1;

    n_active--;
}

template <class T>
size_t VectorMap<T>::insert(T datum)
{
    if (free_inds.empty())
    {
        return -1;
    }
    size_t new_ind = *free_inds.begin();
    free_inds.erase(free_inds.begin());

    m_data.at(n_active) = datum;
    m_data2entity_ind.at(n_active) = new_ind;
    m_entity2data_ind.at(new_ind) = n_active;

    n_active++;

    return new_ind;
}

template <class T>
size_t VectorMap<T>::size()
{
    return n_active;
}
template <class T>
T &VectorMap<T>::getEntity(int index)
{
    assert(index >= 0 && index <= n_max_entities);
    return m_data.at(m_entity2data_ind.at(index));
}

template <class T>
void VectorMap<T>::clear()
{
    for (size_t i = 0; i < n_max_entities; ++i)
    {
        free_inds.insert(i);
        m_entity2data_ind[i] = -1;
        m_data2entity_ind[i] = -1;
    }
    n_active = 0; 
}