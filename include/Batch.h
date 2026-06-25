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
//

struct RenderContext
{
    View view;
    Shader *p_shader = nullptr;
    TextureArray tex_ids;
};

class BatchI
{

public:
    BatchI(VAOId layout);
    virtual ~BatchI();

    virtual void flush(View &view, Shader &shader, TextureArray textures) = 0;
    virtual void flush(std::size_t begin, std::size_t end, RenderContext context) = 0;
    virtual void flush(std::size_t begin, RenderContext context) = 0;
    GLuint initVertexArrayObject(VAOId layout);

    void addVertices(const void *vertex_data, std::size_t data_size);
    void addInstance(const void *instance_data, std::size_t data_size);

    float getDepth(std::size_t index) const
    {
        assert(m_buffer_with_depth != nullptr);
        return *reinterpret_cast<const float *>(m_buffer_with_depth +
                                                index * m_instance_size +
                                                m_depth_offset);
    }

    virtual void sortByDepth() = 0;
    //! finds first instance index higher than given depth
    std::size_t firstHigherThan(std::size_t begin, float query_depth) const
    {
        assert(m_buffer_with_depth != nullptr);
        for (std::size_t instance_id = begin; instance_id < m_instance_count; ++instance_id)
        {
            float depth = *reinterpret_cast<const float *>((m_buffer_with_depth +
                                                            instance_id * m_instance_size +
                                                            m_depth_offset));
            if (depth > query_depth)
            {
                return instance_id;
            }
        }
        return m_instance_count;
    }

private:
    // virtual GLuint initVertexArrayObjectImpl(VAOId layout) {};

public:
    std::size_t m_depth_offset;  //!< depth is offset this many bytes from start of the instance
    std::size_t m_instance_size; //!< size of one instance in bytes

    std::size_t m_instance_count = 0;
    std::size_t m_vertex_count = 0;

protected:
    GLuint m_instance_buffer;
    GLuint m_vertex_buffer;
    GLuint m_vao;

protected:
    alignas(16) std::vector<std::byte> m_vertex_data;
    alignas(16) std::vector<std::byte> m_instance_data;
    std::byte *m_buffer_with_depth = nullptr;

    VAOId m_layout;
};

class VertexBatch : public BatchI
{
public:
    VertexBatch(VAOId layout);

    void flush(View &view, Shader &shader, TextureArray textures) override;
    void flush(std::size_t begin, std::size_t end, RenderContext context) override;
    void flush(std::size_t begin, RenderContext context) override;

    void sortByDepth() override;
};

class InstancedBatch : public BatchI
{
public:
    InstancedBatch(std::vector<std::byte> vertex_data, VAOId layout);

    void flush(View &view, Shader &shader, TextureArray textures) final;
    void flush(std::size_t begin, std::size_t end, RenderContext context) final;
    void flush(std::size_t begin, RenderContext context) final;
    void sortByDepth() override
    {
        m_buffer_with_depth = nullptr;
    }
};

template <class InstanceT>
class DepthBatch : public InstancedBatch
{
public:
    DepthBatch(std::vector<std::byte> vertex_data, VAOId layout)
        : InstancedBatch(vertex_data, layout) {}

    void sortByDepth() override
    {
        m_buffer_with_depth = m_instance_data.data();
        InstanceT *p_instance_data = reinterpret_cast<InstanceT *>(m_instance_data.data());

        std::sort(p_instance_data,
                  p_instance_data + m_instance_count,
                  [](InstanceT &i1, InstanceT &i2)
                  { return i1.depth < i2.depth; });
    }

private:
};

VAOId makeSpriteVAO();
VAOId makeTextVAO();

std::unique_ptr<BatchI> makeSpriteBatch();
std::unique_ptr<BatchI> makeTextBatch();
std::unique_ptr<BatchI> makeVertexBatch();

class BatchRegistry
{
public:
    using BatchMaker = std::function<std::unique_ptr<BatchI>()>;

private:
    using BatchHolderFlat = std::vector<std::pair<BatchConfig, std::unique_ptr<BatchI>>>;
    using BatchConfigs = std::vector<BatchConfig>;

public:
    void renderAll(View &view);

    template <class T>
    void pushInstance(const T &instance, const BatchConfig &config)
    {
        std::size_t batch_type_id = m_type2batch_id.at(typeid(T));
        BatchHolderFlat &batch_holder = m_batches.at(batch_type_id);
        BatchConfigs &configs = m_batch_configs.at(batch_type_id);

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
        std::size_t batch_type_id = m_type2batch_id.at(typeid(T));
        BatchHolderFlat &batch_holder = m_batches.at(batch_type_id);
        BatchConfigs &configs = m_batch_configs.at(batch_type_id);

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

private:
    std::unordered_map<std::type_index, std::size_t> m_type2batch_id;

    std::vector<BatchHolderFlat> m_batches;
    std::vector<BatchConfigs> m_batch_configs;
    std::vector<BatchMaker> m_batch_makers;
};

// #define PARENS ()

// /*---------------------------------------------------------------------------------*/
// #define EXPAND(...) EXPAND4(EXPAND4(EXPAND4(EXPAND4(__VA_ARGS__))))
// #define EXPAND4(...) EXPAND3(EXPAND3(EXPAND3(EXPAND3(__VA_ARGS__))))
// #define EXPAND3(...) __VA_ARGS__
// /*---------------------------------------------------------------------------------*/
// #define FOR_EACH(macro, ...) \
//     __VA_OPT__(EXPAND(FOR_EACH_HELPER(macro, __VA_ARGS__)))
// #define FOR_EACH_HELPER(macro, a1, ...) \
//     macro(a1)                           \
//         __VA_OPT__(, FOR_EACH_AGAIN PARENS(macro, __VA_ARGS__))
// #define FOR_EACH_AGAIN() FOR_EACH_HELPER
// /*---------------------------------------------------------------------------------*/
// #define FOR_EACH_P(macro, first, ...) \
//     __VA_OPT__(EXPAND(FOR_EACH_HELPER_P(macro, first, __VA_ARGS__)))
// #define FOR_EACH_HELPER_P(macro, first, a1, ...) \
//     macro(first, a1)                             \
//         __VA_OPT__(, FOR_EACH_AGAIN_P PARENS(macro, first, __VA_ARGS__))
// #define FOR_EACH_AGAIN_P() FOR_EACH_HELPER_P
// /*---------------------------------------------------------------------------------*/
// #define STRINGIFY(arg) #arg
// #define NUMARGS(...) (sizeof((const char *[]){FOR_EACH(STRINGIFY, __VA_ARGS__)}) / sizeof(const char *))
// /*---------------------------------------------------------------------------------*/
// #define SIZEOF_MEMBER(Base, member) \
//     sizeof(Base::member)
// /*---------------------------------------------------------------------------------*/
// #define DESCRIBE_ATTRIBUTE(AttributeType, ...)                                                              \
//     namespace describe                                                                                      \
//     {                                                                                                       \
//         class AttributeType##_DESCRIPTOR                                                                    \
//         {                                                                                                   \
//         public:                                                                                             \
//             inline static int ATTRIB_COUNT = NUMARGS(__VA_ARGS__);                                          \
//             inline static std::vector<int> OFFSETS = {FOR_EACH_P(offsetof, AttributeType, __VA_ARGS__)};    \
//             inline static std::vector<int> SIZES = {FOR_EACH_P(SIZEOF_MEMBER, AttributeType, __VA_ARGS__)}; \
//         };                                                                                                  \
//                                                                                                             \
//         template <>                                                                                         \
//         std::vector<int> getOffsets<AttributeType>()                                                        \
//         {                                                                                                   \
//             return AttributeType##_DESCRIPTOR::OFFSETS;                                                     \
//         }                                                                                                   \
//         template <>                                                                                         \
//         std::vector<int> getSizes<AttributeType>()                                                          \
//         {                                                                                                   \
//             return AttributeType##_DESCRIPTOR::SIZES;                                                       \
//         }                                                                                                   \
//     }

// /*---------------------------------------------------------------------------------*/

// namespace describe
// {
//     template <class DescribedType>
//     std::vector<int> getOffsets() { return {}; }
//     template <class DescribedType>
//     std::vector<int> getSizes() { return {}; }
// };

// struct Instance
// {
//     char x;
//     double y;
// };
// DESCRIBE_ATTRIBUTE(Instance, x, y);
// DESCRIBE_ATTRIBUTE(Vertex, pos, color, tex_coord);

// template <class VertexData, class InstanceData>
// VAOId getVAOId()
// {
//     VAOId id;
//     // id.instance_size = si
//     auto offsets = describe::getOffsets<InstanceData>();
//     id.instanced_attributes;

//     return id;
// }