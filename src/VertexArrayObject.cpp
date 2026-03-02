#include "VertexArrayObject.h"



VAOId makeVertexArrayVAO()
{
    VAOId layout;

    Vertex i;
    layout.vertex_attirbutes = {
        makeAttribute(i.pos),
        makeAttribute(i.color),
        makeAttribute(i.tex_coord)};

    layout.vertices_size = sizeof(Vertex);
    layout.instance_size = 0;
    layout.max_vertex_buffer_count = 60000; //! vertices are just a square
    layout.max_instance_count = 1;

    return layout;
}

VAOId makeSpriteVAO()
{
    VAOId layout;

    SpriteInstance i;
    layout.instanced_attributes = {
        makeAttribute(i.trans),
        makeAttribute(i.scale),
        makeAttribute(i.angle),
        makeAttribute(i.tex_coords),
        makeAttribute(i.tex_size),
        makeAttribute(i.color)};

    layout.vertex_attirbutes = {
        makeAttribute(utils::Vector2f{}),
        makeAttribute(utils::Vector2f{})};

    layout.vertices_size = 2 * sizeof(utils::Vector2f);
    layout.instance_size = sizeof(SpriteInstance);
    layout.max_vertex_buffer_count = 6; //! vertices are just a square
    layout.max_instance_count = 40000;

    return layout;
}

VAOId makeTextVAO()
{
    VAOId layout;

    TextInstance i;
    layout.instanced_attributes = {
        makeAttribute(i.pos),
        makeAttribute(i.scale),
        makeAttribute(i.angle),
        makeAttribute(i.edge_color),
        makeAttribute(i.fill_color),
        makeAttribute(i.glow_color),
        makeAttribute(i.char_code),
        makeAttribute(i.start_time)};

    layout.vertex_attirbutes = {
        makeAttribute(utils::Vector2f{}),
        makeAttribute(utils::Vector2f{})};

    layout.vertices_size = 2 * sizeof(utils::Vector2f);
    layout.instance_size = sizeof(TextInstance);
    layout.max_vertex_buffer_count = 6; //! vertices are just a square
    layout.max_instance_count = 40000;

    return layout;
}