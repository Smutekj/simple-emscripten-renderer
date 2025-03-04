#pragma once

constexpr const char *vertex_sprite_code = "#version 300 es\n"
                                           "precision mediump float;\n"
                                           "layout(location = 0) in vec2 a_position;\n"
                                           "layout(location = 1) in vec2 a_translation;\n"
                                           "layout(location = 2) in vec2 a_scale;\n"
                                           "layout(location = 3) in float a_angle;\n"
                                           "layout(location = 4) in vec2 a_tex_coord;\n"
                                           "layout(location = 5) in vec2 a_tex_dim;\n"
                                           "layout(location = 6) in vec4 a_color;\n"
                                           "out vec2 v_tex_coord;\n"
                                           "out vec4 v_color;\n"
                                           "uniform mat4 u_view_projection ; \n"
                                           "void main()\n"
                                           "{\n"
                                           "    vec2 transformed_pos = vec2(cos(a_angle) * a_position.x - sin(a_angle) * a_position.y,\n"
                                           "                                +sin(a_angle) * a_position.x + cos(a_angle) * a_position.y);\n"
                                           "    gl_Position = u_view_projection * vec4(a_scale * transformed_pos + a_translation, 0., 1.0);\n"
                                           "    float id_f = float(gl_VertexID);\n"
                                           "    float ix = float(gl_VertexID / 2);\n"
                                           "    float iy = mod(id_f, 2.);\n"
                                           "    v_tex_coord = vec2(a_tex_coord.x + a_tex_dim.x * ix, a_tex_coord.y + a_tex_dim.y * iy);\n"
                                           "    v_color = a_color;\n"
                                           "}\n";

constexpr const char *vertex_vertexarray_code = "#version 300 es\n"
                                                "precision mediump float;\n"
                                                "in vec2 a_position;\n"
                                                "in vec4 a_color;\n"
                                                "in vec2 a_tex_coord;\n"
                                                "out vec2 v_tex_coord;\n"
                                                "out vec4 v_color;\n"
                                                "uniform mat4 u_view_projection;\n"
                                                "void main()\n"
                                                "{\n"
                                                "    gl_Position = u_view_projection*vec4(a_position.xy, 0.f, 1.0);\n"
                                                "    gl_Position.z = a_color.a;\n"
                                                "    v_color     = a_color;\n"
                                                "    v_tex_coord = a_tex_coord;\n"
                                                "}";

constexpr const char *fragment_font_code = "#version 300 es\n"
                                           "precision mediump float;\n"
                                           "in vec2 v_tex_coord;\n"
                                           "in vec4 v_color; \n"
                                           "out vec4 FragColor;\n"
                                           "uniform sampler2D u_texture;\n"
                                           "void main()\n"
                                           "{               "
                                           "    float glyph_region = texture(u_texture, v_tex_coord).r;\n"
                                           "    float x = smoothstep(0., 0.9, glyph_region);\n"
                                           "    FragColor = vec4(vec3(1.-x), glyph_region);\n"
                                           "}";

constexpr const char *fragment_fullpass_code = "#version 300 es\n"
                                               "precision mediump float;\n"
                                               "in vec2 v_tex_coord;\n"
                                               "in vec4 v_color;\n"
                                               "out vec4 FragColor;\n"
                                               "void main()\n"
                                               "{\n"
                                               "    FragColor =   v_color;\n"
                                               "}";
constexpr const char *fragment_fullpass_texture_code = "#version 300 es\n"
                                                       "precision mediump float;\n"
                                                       "uniform sampler2D u_texture;\n"
                                                       "in vec2 v_tex_coord;\n"
                                                       "in vec4 v_color;\n"
                                                       "out vec4 FragColor;\n"
                                                       "void main()\n"
                                                       "{\n"
                                                       "    FragColor =   v_color *  texture(u_texture, v_tex_coord);\n"
                                                       "}";

constexpr const char *fragment_text_code = "#version 300 es\n"
                                           "precision mediump float;\n"
                                           "in vec2 v_tex_coord;\n"
                                           "in vec4 v_color;\n"
                                           "uniform vec4 u_edge_color = vec4(0., 0.,0.,1.);\n"
                                           "uniform float u_time = 0.;\n"
                                           "uniform float u_time_factor = 10.;\n"
                                           "out vec4 FragColor;\n"
                                           "uniform sampler2D u_texture;\n"
                                           "void main()\n"
                                           "{\n"
                                           "    float glyph_region = texture(u_texture, v_tex_coord).a;\n"
                                           "    float x = smoothstep(0.44, 0.45, glyph_region);\n"
                                           "    float edge = smoothstep(0.31, 0.36, glyph_region)  - smoothstep(0.46, 0.5, glyph_region);\n"
                                           "    float edge_inside_alpha = smoothstep(0.5, 0.9, edge);\n"
                                           "    vec3 color_res = edge*edge_inside_alpha * u_edge_color.rgb + (1. - edge_inside_alpha) * x * v_color.rgb;\n"
                                           "    FragColor = vec4(color_res, max(edge, x));\n"
                                           "}";