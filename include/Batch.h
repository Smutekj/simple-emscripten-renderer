#pragma once

#include "Vertex.h"
#include "View.h"
#include "VertexArray.h"
#include "GLTypeDefs.h"
#include "BatchConfig.h"
#include "VertexArrayObject.h"

#include <typeindex>
#include <unordered_map>
#include <vector>
#include <memory>
#include <list>
#include <array>
#include <memory>
#include <functional>

constexpr static std::size_t BATCH_VERTEX_CAPACITY = 65000; //! maximum number of vertices per batch

class Shader;

// template <std::size_t chunk_size>
// class ListOfArrays
// {

//     struct Chunk
//     {
//         std::array<std::byte, chunk_size> data;
//         std::size_t count = 0;
//     };

// public:
//     void pushBack(void *data, std::size_t data_size, std::size_t align)
//     {
//         assert(data_size <= chunk_size);
//         std::size_t pos = m_chunks.back().count  + m_chunks.back().count % align;

//         if(pos + data_size >= chunk_size){
//            m_data.emplace_back() ;
//         }

//         auto& chunk = m_data.back();
//         std::memcpy(chunk.data.get() + count, data, data_size)
//     }

// private:
//     std::list<Chunk> m_chunks;
// };

class BatchI
{

public:
    BatchI(VAOId layout);
    virtual ~BatchI();

    virtual void flush(View &view, Shader &shader, TextureArray textures) = 0;

public:
    void addVertices(const void *vertex_data, std::size_t data_size);
    void addInstance(const void *instance_data, std::size_t data_size);

    GLuint initVertexArrayObject(VAOId layout);

protected:
    GLuint m_instance_buffer;
    GLuint m_vertex_buffer;
    GLuint m_vao;

protected:
    alignas(16) std::vector<std::byte> m_vertex_data;
    alignas(16) std::vector<std::byte> m_instance_data;

    std::size_t m_instance_count = 0;
    std::size_t m_vertex_count = 0;

    VAOId m_layout;
};

class VertexBatch : public BatchI
{
public:
    VertexBatch(VAOId layout);

    void flush(View &view, Shader &shader, TextureArray textures) override;
    void addVertices(void *data, std::size_t data_size);
};
class InstancedBatch : public BatchI
{
public:
    InstancedBatch(std::vector<std::byte> vertex_data, VAOId layout);

    void flush(View &view, Shader &shader, TextureArray textures) override;
};

VAOId makeSpriteVAO();
VAOId makeTextVAO();

std::shared_ptr<BatchI> makeSpriteBatch();
std::shared_ptr<BatchI> makeTextBatch();
std::shared_ptr<BatchI> makeVertexBatch();

struct BatchRegistry
{
    using BatchMaker = std::function<std::shared_ptr<BatchI>()>;
    using BatchHolder = std::unordered_map<BatchConfig, std::shared_ptr<BatchI>>;
    using BatchHolderFlat = std::vector<std::pair<BatchConfig, std::shared_ptr<BatchI>>>;
    using BatchConfigs = std::vector<BatchConfig>;

    void renderAll(View &view)
    {

        for (auto &batch_holder : m_batches)
        {
            for (auto &[config, batch] : batch_holder)
            {
                batch->flush(view, *config.p_shader, config.texture_ids);
            }
        }
    }

    template <class T>
    void pushInstance(const T &instance, const BatchConfig &config)
    {
        // auto batch_type_id = m_type2batch_id.at(typeid(T));
        // if (!configExists(config, batch_type_id))
        // {
        //     m_batches.at(batch_type_id)[config] = m_batch_makers.at(batch_type_id)();
        // }
        // m_batches.at(batch_type_id).at(config)->addInstance(&instance, sizeof(T));

        std::size_t batch_type_id = m_type2batch_id.at(typeid(T));
        BatchHolderFlat &batch_holder = m_batches.at(batch_type_id);
        BatchConfigs& configs = m_batch_configs.at(batch_type_id);

        std::size_t batch_holder_id = std::find(begin(configs), end(configs), config) - begin(configs);
        if (batch_holder_id == batch_holder.size()) [[unlikely]]
        {
            batch_holder.emplace_back(config, m_batch_makers.at(batch_type_id)());
            configs.push_back(config);
        }
        
        batch_holder.at(batch_holder_id).second->addInstance(&instance, sizeof(T));
    }

    template <class T>
    void pushVertex(const T &vertex, const BatchConfig &config)
    {
        pushVertices(&vertex, 1, config);
    }
    template <class T>
    void pushVertices(const std::vector<T> &vertex, const BatchConfig &config)
    {
        pushVertices(vertex.data(), vertex.size(), config);
    }
    template <class T>
    void pushVertices(const T *vertices, std::size_t vertex_count, const BatchConfig &config)
    {
        // auto batch_type_id = m_type2batch_id.at(typeid(T));
        // if (!configExists(config, batch_type_id))
        // {
        //     m_batches.at(batch_type_id)[config] = m_batch_makers.at(batch_type_id)();
        // }
        // m_batches.at(batch_type_id).at(config)->addVertices(vertices, vertex_count * sizeof(T));

        std::size_t batch_type_id = m_type2batch_id.at(typeid(T));
        BatchHolderFlat &batch_holder = m_batches.at(batch_type_id);
        BatchConfigs& configs = m_batch_configs.at(batch_type_id);

        std::size_t batch_holder_id = std::find(begin(configs), end(configs), config) - begin(configs);
        if (batch_holder_id == batch_holder.size()) [[unlikely]]
        {
            batch_holder.emplace_back(config, m_batch_makers.at(batch_type_id)());
            configs.push_back(config);
        }

        m_batches.at(batch_type_id).at(batch_holder_id).second->addVertices(vertices, vertex_count * sizeof(T));
    }

    template <class VertexT, class InstanceT>
    void registerBatch(BatchMaker maker)
    {
        m_type2batch_id[typeid(VertexT)] = m_batches.size();
        m_type2batch_id[typeid(InstanceT)] = m_batches.size();

        m_batches.push_back({});
        m_batch_configs.push_back({});
        m_batch_makers.push_back(maker);
    }

    std::unordered_map<std::type_index, std::size_t> m_type2batch_id;

    // std::vector<BatchHolder> m_batches;
    std::vector<BatchHolderFlat> m_batches;
    std::vector<BatchConfigs> m_batch_configs;

    std::vector<BatchMaker> m_batch_makers;
};
