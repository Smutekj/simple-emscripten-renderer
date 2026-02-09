#pragma once

#include "Vertex.h"
#include "Shader.h"
#include "View.h"
#include "VertexArray.h"
#include "GLTypeDefs.h"
#include "BatchConfig.h"
#include "VertexArrayObject.h"

#include <typeindex>
#include <unordered_map>
#include <vector>
#include <memory>

constexpr static int BATCH_VERTEX_CAPACITY = 65000; //! maximum number of vertices per batch


class BatchI
{

public:
    BatchI(VAOId layout);
    virtual ~BatchI();

    virtual void flush(View &view, Shader &shader, TextureArray textures) = 0;

public:
    void addVertices(void *vertex_data, std::size_t data_size);
    void addInstance(void *instance_data, std::size_t data_size);

    GLuint initVertexArrayObject(VAOId layout);

protected:
    GLuint m_instance_buffer;
    GLuint m_vertex_buffer;
    GLuint m_vao;

protected:
    std::vector<std::byte> m_vertex_data;
    std::vector<std::byte> m_instance_data;

    std::size_t m_instance_count = 0;
    std::size_t m_vertex_count = 0;

    VAOId m_layout;
};

class VertexBatch : public BatchI
{
public:
    VertexBatch(VAOId layout);

    virtual void flush(View &view, Shader &shader, TextureArray textures) override;
    void addVertices(void *data, std::size_t data_size);
};
class InstancedBatch : public BatchI
{
public:
    InstancedBatch(std::vector<std::byte> vertex_data, VAOId layout);

    virtual void flush(View &view, Shader &shader, TextureArray textures) override;
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
    void pushInstance(T instance, BatchConfig config)
    {

        auto batch_type_id = m_type2batch_id.at(typeid(T));
        if (!configExists(config, typeid(T)))
        {
            m_batches.at(batch_type_id)[config] = m_batch_makers.at(batch_type_id)();
        }

        m_batches.at(batch_type_id).at(config)->addInstance(&instance, sizeof(T));
    }

    template <class T>
    void pushVertex(T vertex, BatchConfig config)
    {
        auto batch_type_id = m_type2batch_id.at(typeid(T));
        if (!configExists(config, typeid(T)))
        {
            m_batches.at(batch_type_id)[config] = m_batch_makers.at(batch_type_id)();
        }

        m_batches.at(batch_type_id).at(config)->addVertices(&vertex, sizeof(T));
    }
    template <class T>
    void pushVertices(std::vector<T> vertex, BatchConfig config)
    {
        auto batch_type_id = m_type2batch_id.at(typeid(T));
        if (!configExists(config, typeid(T)))
        {
            m_batches.at(batch_type_id)[config] = m_batch_makers.at(batch_type_id)();
        }

        m_batches.at(batch_type_id).at(config)->addVertices(vertex.data(), vertex.size() * sizeof(T));
    }

    bool configExists(BatchConfig config, std::type_index type_id)
    {
        auto batch_type_id = m_type2batch_id.at(type_id);
        return m_batches.at(batch_type_id).contains(config);
    }

    template <class VertexT, class InstanceT>
    void registerBatch(BatchMaker maker)
    {
        m_type2batch_id[typeid(VertexT)] = m_batches.size();
        m_type2batch_id[typeid(InstanceT)] = m_batches.size();

        m_batches.push_back({});
        m_batch_makers.push_back(maker);
    }

    
    std::unordered_map<std::type_index, std::size_t> m_type2batch_id;

    std::vector<BatchHolder> m_batches;
    std::vector<BatchMaker> m_batch_makers;
};
