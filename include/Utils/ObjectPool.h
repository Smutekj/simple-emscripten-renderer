
#pragma once

#include <vector>
#include <stdexcept>

namespace utils
{

    template <typename Type>
    class VectorMap
    {

    public:
        VectorMap() = default;
        explicit VectorMap(int n_max_entities);

        void removeByDataInd(int data_ind);
        void removeByEntityInd(int entity_ind);

        size_t insert(auto &&datum);

        void clear();
        size_t size();
        size_t capacity();
        void setMaxCount(int n_max_count);


        Type &getEntity(int index);
        size_t getEntityInd(int data_ind) const;

        std::vector<Type> &getData();

    private:
        size_t n_max_entities = 0;
        std::vector<int> m_entity2data_ind;
        std::vector<int> m_data2entity_ind;
        std::vector<Type> m_data;

        std::size_t m_freelist_head = -1;
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
    size_t VectorMap<T>::getEntityInd(int data_ind) const
    {
        return m_data2entity_ind.at(data_ind);
    }

    template <class T>
    VectorMap<T>::VectorMap(int n_max_entities)
        : n_max_entities(n_max_entities),
          m_entity2data_ind(n_max_entities, -1),
          m_data2entity_ind(n_max_entities, -1)
    {
        m_data.reserve(n_max_entities);
    }

    template <class T>
    void VectorMap<T>::setMaxCount(int n_max_count)
    {
        n_max_entities = n_max_count;
    }

    template <class T>
    void VectorMap<T>::removeByDataInd(int data_ind)
    {
        auto entity_ind = m_data2entity_ind.at(data_ind);

        m_entity2data_ind.at(m_data2entity_ind.at(n_active - 1)) = data_ind;

        m_data[data_ind] = m_data.back();
        m_data.pop_back();

        m_data2entity_ind[n_active - 1] = -1;

        //! update freelist
        m_entity2data_ind[entity_ind] = m_freelist_head;
        m_freelist_head = entity_ind; //! deleted becomes new freelist_head
        n_active--;
    }

    template <class T>
    void VectorMap<T>::removeByEntityInd(int entity_ind)
    {
        auto data_ind = m_entity2data_ind.at(entity_ind);
        if (data_ind == -1)
        {
            throw std::runtime_error("Trying to delete non-existent object!");
        }

        auto moved_entity_ind = m_data2entity_ind.at(n_active - 1);
        m_entity2data_ind.at(moved_entity_ind) = data_ind;

        m_data.at(data_ind) = m_data.back();
        m_data.pop_back();
        m_data2entity_ind.at(data_ind) = moved_entity_ind;
        m_data2entity_ind.at(n_active - 1) = -1;

        m_entity2data_ind[entity_ind] = m_freelist_head;
        m_freelist_head = entity_ind; //! deleted becomes new freelist_head

        n_active--;
    }

    template <class T>
    size_t VectorMap<T>::insert(auto &&datum)
    {
        if (n_active >= n_max_entities)
        {
            return -1;
        }
        size_t new_ind = n_active;
        if (m_freelist_head != -1)
        {
            new_ind = m_freelist_head;
            m_freelist_head = m_entity2data_ind[m_freelist_head];
        }

        m_data.emplace_back(datum);
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

        return m_data.at(m_entity2data_ind.at(index));
    }

    template <class T>
    void VectorMap<T>::clear()
    {
        for (size_t i = 0; i < n_max_entities; ++i)
        {
            m_entity2data_ind[i] = -1;
            m_data2entity_ind[i] = -1;
        }
        n_active = 0;
    }

}