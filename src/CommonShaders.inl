#pragma once

constexpr const char *vertex_text_code = R"V0G0N(#version 300 es
precision highp float;

layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_tex_coord;
layout(location = 2) in vec2 a_translation;
layout(location = 3) in vec2 a_scale;
layout(location = 4) in float a_angle;
layout(location = 5) in vec4 a_edge_color;
layout(location = 6) in vec4 a_fill_color;
layout(location = 7) in vec4 a_glow_color;
layout(location = 8) in int a_charcode;
layout(location = 9) in float a_start_time;

out vec2 v_tex_coord;
out vec4 v_edge_color;
out vec4 v_fill_color;
out vec4 v_glow_color;
out float v_start_time;

uniform sampler2D u_texture;
uniform sampler2D u_charmap;
uniform mat4 u_view_projection;
void main()
{
    vec2 scaled_pos = a_scale * a_position;
    vec2 rotated_pos = vec2(cos(a_angle) * scaled_pos.x - sin(a_angle) * scaled_pos.y,
                            +sin(a_angle) * scaled_pos.x + cos(a_angle) * scaled_pos.y);
    gl_Position = u_view_projection * vec4(rotated_pos + a_translation, 0., 1.0);
    float char_count = float(textureSize(u_charmap, 0).x);
    vec4 glyph_tex_rect = texelFetch(u_charmap, ivec2(a_charcode, 0), 0);
    vec2 tex_coord = glyph_tex_rect.rg;
    vec2 tex_dim = glyph_tex_rect.ba;
    v_tex_coord = vec2(tex_coord.x + tex_dim.x * a_tex_coord.x, tex_coord.y - tex_dim.y * (1.-a_tex_coord.y));
    v_edge_color = a_edge_color;
    v_fill_color = a_fill_color;
    v_glow_color = a_glow_color;
    v_start_time = a_start_time;
}
)V0G0N";

constexpr const char *vertex_sprite_code_old = R"V0G0N(#version 300 es
                                               precision highp float;
                                               layout(location = 0) in vec2 a_position;
                                               layout(location = 1) in vec2 a_translation;
                                               layout(location = 2) in vec2 a_scale;
                                               layout(location = 3) in float a_angle;
                                               layout(location = 4) in vec2 a_tex_coord;
                                               layout(location = 5) in vec2 a_tex_dim;
                                               layout(location = 6) in vec4 a_color;
                                               out vec2 v_tex_coord;
                                               out vec4 v_color;
                                               uniform sampler2D u_texture ;
                                               uniform mat4 u_view_projection ;
                                               void main()
                                               {
                                                   vec2 scaled_pos = a_scale * a_position;
                                                   vec2 rotated_pos = vec2(cos(a_angle) * scaled_pos.x - sin(a_angle) * scaled_pos.y,
                                                                               +sin(a_angle) * scaled_pos.x + cos(a_angle) * scaled_pos.y);
                                                   gl_Position = u_view_projection * vec4(rotated_pos + a_translation, 0., 1.0);
                                                   float id_f = float(gl_VertexID);
                                                   float ix = float(gl_VertexID / 2);
                                                   float iy = 1.-mod(id_f, 2.);
                                                   ivec2 texel_i = textureSize(u_texture, 0);
                                                   vec2 texel = vec2(0.0 / float(texel_i.x), 0.0 / float(texel_i.y));
                                                   v_tex_coord= vec2(a_tex_coord.x + a_tex_dim.x * ix, a_tex_coord.y - a_tex_dim.y * iy);
                                                   v_color = a_color;
                                               }
)V0G0N";

constexpr const char *vertex_sprite_code = R"V0G0N(#version 300 es
precision highp float;
layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_tex_pos;
layout(location = 2) in vec2 a_translation;
layout(location = 3) in vec2 a_scale;
layout(location = 4) in float a_angle;
layout(location = 5) in vec2 a_tex_coord;
layout(location = 6) in vec2 a_tex_dim;
layout(location = 7) in vec4 a_color;
out vec2 v_tex_coord;
out vec4 v_color;
uniform sampler2D u_texture ;
uniform mat4 u_view_projection ;
void main()
{
   vec2 scaled_pos = a_scale * a_position;
   vec2 rotated_pos = vec2(cos(a_angle) * scaled_pos.x - sin(a_angle) * scaled_pos.y,
                               +sin(a_angle) * scaled_pos.x + cos(a_angle) * scaled_pos.y);
   gl_Position = u_view_projection * vec4(rotated_pos + a_translation, 0., 1.0);
   v_tex_coord= vec2(a_tex_coord.x + a_tex_dim.x * a_tex_pos.x, a_tex_coord.y - a_tex_dim.y * (1. - a_tex_pos.y));
   v_color = a_color;
}
)V0G0N";

constexpr const char *vertex_sprite_code_direct = R"V0G0N(#version 300 es
precision highp float;
layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_tex_pos;
out vec2 v_tex_coord;
out vec4 v_color;
uniform mat4 u_view_projection ;
uniform mat4 u_transform;
void main()
{
    gl_Position = u_view_projection * u_transform * vec4(a_position, 0., 1.);
    v_tex_coord= a_tex_pos;
    v_color = vec4(1.);
}
)V0G0N";

constexpr const char *vertex_vertexarray_code = R"V0G0N(#version 300 es
precision highp float;
in vec2 a_position;
in vec4 a_color;
in vec2 a_tex_coord;
out vec2 v_tex_coord;
out vec4 v_color;
uniform mat4 u_view_projection;
void main()
{
    gl_Position = u_view_projection*vec4(a_position.xy, 0.f, 1.0);
    gl_Position.z = a_color.a;
    v_color     = a_color;
    v_tex_coord = a_tex_coord;
}
)V0G0N";

constexpr const char *fragment_font_code = R"V0G0N(#version 300 es
precision highp float;
in vec2 v_tex_coord;
in vec4 v_color;
out vec4 FragColor;
uniform sampler2D u_texture;
void main()
{               
    float glyph_region = texture(u_texture, v_tex_coord).r;
    float x = smoothstep(0.1, 0.3, glyph_region);
    FragColor = vec4(vec3(glyph_region), glyph_region);
}
)V0G0N";

constexpr const char *fragment_fullpass_code = R"V0G0N(#version 300 es
precision highp float;
in vec2 v_tex_coord;
in vec4 v_color;
out vec4 FragColor;
void main()
{
   FragColor =   vec4(v_color.rgb * v_color.a, v_color.a);
}
)V0G0N";

constexpr const char *fragment_fullpass_texture_code = R"V0G0N(#version 300 es
precision highp float;
uniform sampler2D u_texture;
in vec2 v_tex_coord;
in vec4 v_color;
out vec4 FragColor;
void main()
{
    ivec2 tex_sizei = textureSize(u_texture, 0);
    float texelX = 0.0 / float(tex_sizei.x);
    float texelY = 0.0 / float(tex_sizei.y);
    vec2 uv = v_tex_coord + vec2(texelX, texelY);
    vec3 tex_color = texture(u_texture, uv).rgb;
    float tex_alpha = texture(u_texture, uv).a;
    FragColor =   v_color *  vec4(tex_color*tex_alpha, tex_alpha);
}
)V0G0N";
constexpr const char *fragment_fullpass_texture_code_no_alpha = R"V0G0N(#version 300 es
                                                                precision highp float;
                                                                uniform sampler2D u_texture;
                                                                in vec2 v_tex_coord;
                                                                in vec4 v_color;
                                                                out vec4 FragColor;
                                                                void main()
                                                                {
                                                                    vec3 tex_color = texture(u_texture, v_tex_coord).rgb;
                                                                    float tex_alpha = texture(u_texture, v_tex_coord).a;
                                                                    FragColor =   v_color *  vec4(tex_color, tex_alpha);
                                                                }
                                                            )V0G0N";


constexpr const char *fragment_text2_code = R"V0G0N(#version 300 es
precision highp float;
in vec2 v_tex_coord;
in vec4 v_edge_color;
in vec4 v_fill_color;
in vec4 v_glow_color;
uniform float u_smooth_min = 0.465;
uniform float u_smooth_max = 0.495;
uniform float u_glow_min = 0.25;
uniform float u_glow_max = 0.45;;
uniform int u_outline = 1;
uniform int u_smooth_edges = 1;
uniform int u_outer_glow = 1;
uniform vec2 u_glow_uv_offset = vec2(0.0, 0.0);
uniform float u_outline_min0 = 0.45;
uniform float u_outline_min1 = 0.48;
uniform float u_outline_max0 = 0.495;
uniform float u_outline_max1 = 0.53;
uniform float u_thick = 0.485;
out vec4 FragColor;
uniform sampler2D u_texture;
uniform sampler2D u_charmap;
void main()
{
    float dist_mask =  texture(u_texture, v_tex_coord).a;
    vec4 color_res = v_fill_color;
    if(u_outline == 1 && dist_mask > u_outline_min0 && dist_mask < u_outline_max1){ 
        float outline_factor = 1.0;
        if(dist_mask < u_outline_min1)
        {
            outline_factor = smoothstep(u_outline_min0, u_outline_min1, dist_mask);
        }else{
            outline_factor = smoothstep(u_outline_max1, u_outline_max0, dist_mask);
        }
        color_res = mix(v_fill_color, v_edge_color, outline_factor);
   }
    float glyph_mask = 0.;
    if(u_smooth_edges == 1)
    {
        glyph_mask = color_res.a * smoothstep(u_smooth_min, u_smooth_max, dist_mask);
    }
    else{
        glyph_mask = float(dist_mask > u_thick);
    }
    color_res.a = glyph_mask;
    if(u_outer_glow == 1)
    {
        float glow_texel = texture(u_texture, v_tex_coord + u_glow_uv_offset).a; 
        vec4 glow_color = v_glow_color * smoothstep(u_glow_min, u_glow_max, glow_texel);
        color_res = mix(glow_color, color_res, glyph_mask);
    }
   FragColor = vec4(color_res.rgb * color_res.a, color_res.a);
}
)V0G0N";

constexpr const char *fragment_text_code = "#version 300 es\n"
                                           "precision highp float;\n"
                                           "in vec2 v_tex_coord;\n"
                                           "in vec4 v_color;\n"
                                           "uniform vec4 u_edge_color = vec4(0., 0.,0., 1.);\n"
                                           "uniform float u_smoothness = 0.01;\n"
                                           "uniform float u_outline_min0 = 0.38;\n"
                                           "uniform float u_outline_min1 = 0.41;\n"
                                           "uniform float u_outline_max0 = 0.50;\n"
                                           "uniform float u_outline_max1 = 0.52;\n"
                                           "uniform float u_thick = 0.49;\n"
                                           "out vec4 FragColor;\n"
                                           "uniform sampler2D u_texture;\n"
                                           "void main()\n"
                                           "{\n"
                                           "    float dist_mask = texture(u_texture, v_tex_coord).a;\n"
                                           "    vec4 color_res = vec4(0.);\n"
                                           "    float outline_factor = 1.0;\n"
                                           "    if(dist_mask > u_outline_min0 && dist_mask < u_outline_max1){\n"
                                           "        if(dist_mask < u_outline_min1)\n"
                                           "        {\n"
                                           "            outline_factor = smoothstep(u_outline_min0, u_outline_min1, dist_mask);\n"
                                           "        }else{\n"
                                           "            outline_factor = smoothstep(u_outline_max0, u_outline_max1, dist_mask);\n"
                                           "        }\n"
                                           "    }\n"
                                           "    color_res = mix(u_edge_color, v_color, outline_factor);\n"
                                           "    float glyph_factor = smoothstep(u_thick - u_smoothness, u_thick + u_smoothness, dist_mask);\n"
                                           "    FragColor = color_res*glyph_factor;\n"
                                           "}";

constexpr const char *fragment_brightness_code = "#version 300 es\n"
                                                 "precision highp float;\n"
                                                 "in vec2 v_tex_coord;\n"
                                                 "in vec4 v_color;\n"
                                                 "out vec4 FragColor;\n"
                                                 "uniform float u_brightness_threshold = 1.0;\n"
                                                 "uniform sampler2D u_input;\n"
                                                 "void main()\n"
                                                 "{\n"
                                                 "    vec3 input_color = texture(u_input, v_tex_coord).rgb;\n"
                                                 "    float input_alpha = texture(u_input, v_tex_coord).a;\n"
                                                 "    // check whether fragment output is higher than threshold, if so output as brightness color\n"
                                                 "    float brightness = dot(input_color, vec3(0.2126, 0.7152, 0.0722));\n"
                                                 "    float bright_alpha = smoothstep(u_brightness_threshold-0.02, u_brightness_threshold+0.07, brightness);\n"
                                                 "    FragColor = bright_alpha * vec4(input_color, input_alpha);\n"
                                                 "}\n";

constexpr const char *fragment_downsample_code = "#version 300 es\n"
                                                 "precision highp float;\n"
                                                 "uniform sampler2D u_image;\n"
                                                 "in vec2 v_tex_coord;\n"
                                                 "in vec4 v_color;\n"
                                                 "out vec4 FragColor;\n"
                                                 "void main(void)\n"
                                                 "{\n"
                                                 "	ivec2 tex_sizei = textureSize(u_image, 0);\n"
                                                 "	vec2 tex_size = vec2(tex_sizei.x, tex_sizei.y);\n"
                                                 "  vec2 srcPos = v_tex_coord * vec2(tex_size);\n"
                                                 "  ivec2 base = ivec2(floor(srcPos));\n"
                                                 "  vec4 c00 = texelFetch(u_image, base, 0);\n"
                                                 "  vec4 c10 = texelFetch(u_image, base + ivec2(1,0), 0);\n"
                                                 "  vec4 c01 = texelFetch(u_image, base + ivec2(0,1), 0);\n"
                                                 "  vec4 c11 = texelFetch(u_image, base + ivec2(1,1), 0);\n"
                                                 "	FragColor = (c00 + c01 + c10 + c11) * 0.25;\n"
                                                 "}\n";

constexpr const char *fragment_gauss_vert_code = "#version 300 es\n"
                                                 "precision highp float;\n"
                                                 "uniform sampler2D u_image;\n"
                                                 "in vec2 v_tex_coord;\n"
                                                 "in vec4 v_color;\n"
                                                 "out vec4 FragColor;\n"
                                                 "void main(void)\n"
                                                 "{\n"
                                                 "	float offset[5] = float[]( 0.0, 1.0, 2.0, 3.0, 4.0 );\n"
                                                 "	float weight[5] = float[]( 0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162 );\n"
                                                 "	vec4 vertexColor = v_color;\n"
                                                 "	ivec2 tex_sizei = textureSize(u_image, 0);\n"
                                                 "	vec2 tex_size = vec2(tex_sizei.x, tex_sizei.y);\n"
                                                 "	vec2 uv = v_tex_coord + 0.0*vec2(1.0/tex_size.x, 1.0/tex_size.y);\n"
                                                 "	vec4 result = texture( u_image, vec2(uv)) * weight[0];\n"
                                                 "	for (int i=1; i<5; i++)\n"
                                                 "	{\n"
                                                 "		result += texture( u_image, ( uv+vec2(0.0, offset[i]*1.0/tex_size.y) ) ) * weight[i];\n"
                                                 " 		result += texture( u_image, ( uv-vec2(0.0, offset[i]*1.0/tex_size.y) ) ) * weight[i];\n"
                                                 "	}\n"
                                                 "	FragColor = result;\n"
                                                 "}\n";
constexpr const char *fragment_gauss3x3_code = "#version 300 es\n"
                                               "precision highp float;\n"
                                               "uniform sampler2D u_image;\n"
                                               "in vec2 v_tex_coord;\n"
                                               "in vec4 v_color;\n"
                                               "out vec4 FragColor;\n"
                                               "void main(void)\n"
                                               "{\n"
                                               "	float offset[5] = float[]( 0.0, 1.0, 2.0, 3.0, 4.0 );\n"
                                               "	float weight[5] = float[]( 0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162 );\n"
                                               "	vec4 vertexColor = v_color;\n"
                                               "	ivec2 tex_sizei = textureSize(u_image, 0);\n"
                                               "	vec2 tex_size = vec2(tex_sizei.x, tex_sizei.y);\n"
                                               "	vec2 uv = v_tex_coord + 0.0*vec2(1.0/tex_size.x, 1.0/tex_size.y);\n"
                                               "	vec4 result = texture( u_image, vec2(uv)) * weight[0];\n"
                                               "	for (int i=1; i<5; i++)\n"
                                               "	{\n"
                                               "		result += texture( u_image, ( uv+vec2(0.0, offset[i]*1.0/tex_size.y) ) ) * weight[i];\n"
                                               " 		result += texture( u_image, ( uv-vec2(0.0, offset[i]*1.0/tex_size.y) ) ) * weight[i];\n"
                                               "	}\n"
                                               "	FragColor = result;\n"
                                               "}\n";

constexpr const char *fragment_gauss_horiz_code = "#version 300 es\n"
                                                  "precision highp float;\n"
                                                  "uniform sampler2D u_image;\n"
                                                  "in vec2 v_tex_coord;\n"
                                                  "in vec4 v_color;\n"
                                                  "out vec4 FragColor;\n"
                                                  "void main(void)\n"
                                                  "{\n"
                                                  "	float offset[5] = float[]( 0.0, 1.0, 2.0, 3.0, 4.0 );\n"
                                                  "	float weight[5] = float[]( 0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162 );\n"
                                                  "	vec4 vertexColor = v_color;\n"
                                                  "	ivec2 tex_sizei = textureSize(u_image, 0);\n"
                                                  "	vec2 tex_size = vec2(tex_sizei.x, tex_sizei.y);\n"
                                                  "	vec2 uv = v_tex_coord + 0.0*vec2(1.0/tex_size.x, 1.0/tex_size.y);\n"
                                                  "	vec4 result = texture( u_image, vec2(uv)) * weight[0];\n"
                                                  "	for (int i=1; i<5; i++)\n"
                                                  "	{\n"
                                                  "		result += texture( u_image, ( uv+vec2(offset[i]*1.0/tex_size.x, 0.0) ) ) * weight[i];\n"
                                                  " 	result += texture( u_image, ( uv-vec2(offset[i]*1.0/tex_size.x, 0.0) ) ) * weight[i];\n"
                                                  "	}\n"
                                                  "	FragColor =  result;\n"
                                                  "}\n";

// Stolen/ from Here
// Remember to add bilinear minification filter for this texture!
// Remember to use a floating-point texture format (for HDR)!
// Remember to use edge clamping for this texture!
// Takes 13 samples around current texel:
// a - b - c
// - j - k -
// d - e - f
// - l - m -
// g - h - i
// === ('e' is the current texel) ===
constexpr const char *fragment_downsample13_code = "#version 300 es\n"
                                                   "precision highp float;\n"
                                                   "uniform sampler2D u_texture;\n"
                                                   "uniform vec2 u_src_resolution;\n"
                                                   "in vec2 v_tex_coord;\n"
                                                   "in vec4 v_color;\n"
                                                   "out vec4 FragColor;\n"
                                                   "void main(){\n"
                                                   "   vec2 srcTexelSize = vec2(1.0 / u_src_resolution.x, 1.0 / u_src_resolution.y);\n"
                                                   "   float x = srcTexelSize.x;\n"
                                                   "   float y = srcTexelSize.y;\n"

                                                   "   vec4 a = texture(u_texture, vec2(v_tex_coord.x - 2.*x, v_tex_coord.y + 2.*y));\n"
                                                   "   vec4 b = texture(u_texture, vec2(v_tex_coord.x,       v_tex_coord.y + 2.*y));\n"
                                                   "   vec4 c = texture(u_texture, vec2(v_tex_coord.x + 2.*x, v_tex_coord.y + 2.*y));\n"

                                                   "   vec4 d = texture(u_texture, vec2(v_tex_coord.x - 2.*x, v_tex_coord.y));\n"
                                                   "   vec4 e = texture(u_texture, vec2(v_tex_coord.x,       v_tex_coord.y));\n"
                                                   "   vec4 f = texture(u_texture, vec2(v_tex_coord.x + 2.*x, v_tex_coord.y));\n"

                                                   "   vec4 g = texture(u_texture, vec2(v_tex_coord.x - 2.*x, v_tex_coord.y - 2.*y));\n"
                                                   "   vec4 h = texture(u_texture, vec2(v_tex_coord.x,       v_tex_coord.y - 2.*y));\n"
                                                   "   vec4 i = texture(u_texture, vec2(v_tex_coord.x + 2.*x, v_tex_coord.y - 2.*y));\n"

                                                   "   vec4 j = texture(u_texture, vec2(v_tex_coord.x - x, v_tex_coord.y + y));\n"
                                                   "   vec4 k = texture(u_texture, vec2(v_tex_coord.x + x, v_tex_coord.y + y));\n"
                                                   "   vec4 l = texture(u_texture, vec2(v_tex_coord.x - x, v_tex_coord.y - y));\n"
                                                   "   vec4 m = texture(u_texture, vec2(v_tex_coord.x + x, v_tex_coord.y - y));\n"

                                                   "   vec4 downsample = e*0.125;\n"
                                                   "   downsample += (a+c+g+i)*0.03125;\n"
                                                   "   downsample += (b+d+f+h)*0.0625;\n"
                                                   "   downsample += (j+k+l+m)*0.125;\n"
                                                   "   FragColor = downsample;\n"
                                                   "}";

constexpr const char *fragment_combine_bloom_code = "#version 300 es\n"
                                                    "precision highp float;\n"
                                                    "uniform sampler2D u_source;\n"
                                                    "uniform sampler2D u_bloom;\n"
                                                    "uniform float exposure = 1.5;\n"
                                                    "in vec2 v_tex_coord;\n"
                                                    "in vec4 v_color;\n"
                                                    "out vec4 FragColor;\n"
                                                    "void main(void)\n"
                                                    "{\n"
                                                    "    const float gamma = 2.2;\n"
                                                    "    vec2 tex_size_s = vec2(textureSize(u_source, 0));\n"
                                                    "    vec2 tex_size_b = vec2(textureSize(u_bloom, 0));\n"
                                                    "    vec2 uv_s = v_tex_coord + vec2(0.5/tex_size_s.x, 0.5/tex_size_s.y);\n"
                                                    "    vec2 uv_b = v_tex_coord + vec2(0.5/tex_size_b.x, 0.5/tex_size_b.y);\n"
                                                    "    vec3 source_color = texture(u_source, v_tex_coord).rgb;\n"
                                                    "    vec3 bloom_color = texture(u_bloom, v_tex_coord).rgb;\n"
                                                    "    float source_alpha = texture(u_source, v_tex_coord).a;\n"
                                                    "    float bloom_alpha = texture(u_bloom, v_tex_coord).a;\n"
                                                    "    vec3 hdr_color = source_color + bloom_color;             // additive blending\n"
                                                    "    vec3 result = min(hdr_color, vec3(1.));             // additive blending\n"
                                                    "    //vec3 result = 1.0 - exp(-hdr_color * exposure);    // tone mapping\n"
                                                    "    float result_alpha = 1.0 - exp(-(bloom_alpha + source_alpha) *exposure);    // tone mapping\n"
                                                    "    result = pow(result, vec3(1.0 / gamma));                // gamma correction\n"
                                                    "    float alpha = min(source_alpha+bloom_alpha, 1.);\n"
                                                    "    FragColor = vec4(result, alpha);\n"
                                                    "}\n";

// constexpr const char *fragment_combine_bloom_code = "#version 300 es\n"
//                                                     "precision highp float;\n"
//                                                     "uniform sampler2D u_source;\n"
//                                                     "uniform sampler2D u_bloom;\n"
//                                                     "uniform float exposure = 1.5;\n"
//                                                     "in vec2 v_tex_coord;\n"
//                                                     "in vec4 v_color;\n"
//                                                     "out vec4 FragColor;\n"
//                                                     "void main(void)\n"
//                                                     "{\n"
//                                                     "    const float gamma = 2.2;\n"
//                                                     "    vec3 source_color = texture(u_source, v_tex_coord).rgb;\n"
//                                                     "    vec3 bloom_color = texture(u_bloom, v_tex_coord).rgb;\n"
//                                                     "    float source_alpha = texture(u_source, v_tex_coord).a;\n"
//                                                     "    float bloom_alpha = texture(u_bloom, v_tex_coord).a;\n"
//                                                     "    vec3 hdr_color = source_color + bloom_color;             // additive blending\n"
//                                                     "    vec3 result = vec3(1.0) - exp(-hdr_color * exposure);    // tone mapping\n"
//                                                     "    result = pow(result, vec3(1.0 / gamma));                // gamma correction\n"
//                                                     "    // result = clamp(bloom_color*5. + source_color, 0., 1.);\n"
//                                                     "    float alpha = min(source_alpha + bloom_alpha, 1.);\n"
//                                                     "    FragColor = vec4(result, alpha);\n"
//                                                     "}\n";

constexpr const char *fragment_edge_detect_code = "#version 300 es\n"
                                                  "precision highp float;\n"
                                                  "in vec2 v_tex_coord;\n"
                                                  "out vec4 FragColor;\n"
                                                  "uniform sampler2D u_input;\n"
                                                  "void main() {\n"
                                                  "    vec2 texel_size = 1.0 / vec2(textureSize(u_input, 0));\n"
                                                  "    float kernelX[9] = float[9](\n"
                                                  "         1.0,  0.0, -1.0,\n"
                                                  "         2.0,  0.0, -2.0,\n"
                                                  "         1.0,  0.0, -1.0\n"
                                                  "    );\n"
                                                  "    float kernelY[9] = float[9](\n"
                                                  "         1.0,  2.0,  1.0,\n"
                                                  "         0.0,  0.0,  0.0,\n"
                                                  "        -1.0, -2.0, -1.0\n"
                                                  "    );\n"
                                                  "    float edgeX = 0.0;\n"
                                                  "    float edgeY = 0.0;\n"
                                                  "    int idx = 0;\n"
                                                  "    for (int y = -1; y <= 1; ++y) {\n"
                                                  "        for (int x = -1; x <= 1; ++x) {\n"
                                                  "            vec2 offset = vec2(float(x), float(y)) * texel_size;\n"
                                                  "            vec3 input_color = texture(u_input, v_tex_coord + offset).rgb;\n"
                                                  "            float input_alpha = texture(u_input, v_tex_coord + offset).a;\n"
                                                  "            float lum = dot(input_color.rgb, vec3(0.299, 0.587, 0.114));\n"
                                                  "            edgeX += kernelX[idx] * input_alpha;\n"
                                                  "            edgeY += kernelY[idx] * input_alpha;\n"
                                                  "            idx++;\n"
                                                  "        }\n"
                                                  "    }\n"
                                                  "    float magnitude = length(vec2(edgeX, edgeY));\n"
                                                  "    FragColor = vec4(vec3(magnitude), min(magnitude, 1.0));\n"
                                                  "}\n";
constexpr const char *fragment_edge_combine_code = "#version 300 es\n"
                                                   "precision highp float;    \n"
                                                   "uniform sampler2D u_source;\n"
                                                   "uniform sampler2D u_edge;\n"
                                                   "uniform vec3 edge_color = vec3(0., 0.5, 5.);\n"
                                                   "in vec2 v_tex_coord;                          \n"
                                                   "in vec4 v_color;       \n"
                                                   "out vec4 FragColor;\n"
                                                   "void main(void)\n"
                                                   "{\n"
                                                   "    vec3 source_color = texture(u_source, v_tex_coord).rgb;      \n"
                                                   "    float source_alpha = texture(u_source, v_tex_coord).a;\n"
                                                   "    float edge_alpha = texture(u_edge, v_tex_coord).a;\n"
                                                   "    FragColor = vec4(edge_color * edge_alpha + source_color*(1.-edge_alpha), source_alpha + edge_alpha);\n"
                                                   "}\n";

// This shader performs upsampling on a texture,
// as taken from Call Of Duty method, presented at ACM Siggraph 2014.

// Remember to add bilinear minification filter for this texture!
// Remember to use a floating-point texture format (for HDR)!
// Remember to use edge clamping for this texture!
constexpr const char *fragment_upsample_blur_code = R"V0G0N(#version 300 es
precision highp float;    
uniform sampler2D u_texture;
uniform float u_filter_radius;
uniform float u_aspect_ratio;
in vec2 v_tex_coord;
in vec4 v_color;
out vec4 FragColor;

void main(){
    // The filter kernel is applied with a radius, specified in texture
    // coordinates, so that the radius will vary across mip resolutions.
    float x = u_filter_radius;
    float y = u_filter_radius * u_aspect_ratio;

    // Take 9 samples around current texel:
    // a - b - c
    // d - e - f
    // g - h - i
    // === ('e' is the current texel) ===
     vec4 a = texture(u_texture, vec2(v_tex_coord.x - x, v_tex_coord.y + y));
     vec4 b = texture(u_texture, vec2(v_tex_coord.x,     v_tex_coord.y + y));
     vec4 c = texture(u_texture, vec2(v_tex_coord.x + x, v_tex_coord.y + y));

     vec4 d = texture(u_texture, vec2(v_tex_coord.x - x, v_tex_coord.y));
     vec4 e = texture(u_texture, vec2(v_tex_coord.x,     v_tex_coord.y));
     vec4 f = texture(u_texture, vec2(v_tex_coord.x + x, v_tex_coord.y));

     vec4 g = texture(u_texture, vec2(v_tex_coord.x - x, v_tex_coord.y - y));
     vec4 h = texture(u_texture, vec2(v_tex_coord.x,     v_tex_coord.y - y));
     vec4 i = texture(u_texture, vec2(v_tex_coord.x + x, v_tex_coord.y - y));

    // Apply weighted distribution, by using a 3x3 tent filter:
    //  1   | 1 2 1 |
    // -- * | 2 4 2 |
    // 16   | 1 2 1 |
    vec4 upsample = e*4.0;
    upsample += (b+d+f+h)*2.0;
    upsample += (a+c+g+i);
    upsample *= 1.0 / 16.0;
    FragColor = upsample;
}
)V0G0N";

constexpr const char *fragment_upsample_mix_code = R"V0G0N(#version 300 es
precision highp float;    
uniform sampler2D u_source;
uniform sampler2D u_texture;
uniform float u_filter_radius;
in vec2 v_tex_coord;
in vec4 v_color;
out vec4 FragColor;
void main(){
  float x = u_filter_radius;
  float y = u_filter_radius;

// Take 9 samples around current texel:
// a - b - c
// d - e - f
// g - h - i
// === ('e' is the current texel) ===
 vec3 source_color = texture(u_source, vec2(v_tex_coord.x, v_tex_coord.y));
 vec4 a = texture(u_texture, vec2(v_tex_coord.x - x, v_tex_coord.y + y));
 vec4 b = texture(u_texture, vec2(v_tex_coord.x,     v_tex_coord.y + y));
 vec4 c = texture(u_texture, vec2(v_tex_coord.x + x, v_tex_coord.y + y));

 vec4 d = texture(u_texture, vec2(v_tex_coord.x - x, v_tex_coord.y));
 vec4 e = texture(u_texture, vec2(v_tex_coord.x,     v_tex_coord.y));
 vec4 f = texture(u_texture, vec2(v_tex_coord.x + x, v_tex_coord.y));

 vec4 g = texture(u_texture, vec2(v_tex_coord.x - x, v_tex_coord.y - y));
 vec4 h = texture(u_texture, vec2(v_tex_coord.x,     v_tex_coord.y - y));
 vec4 i = texture(u_texture, vec2(v_tex_coord.x + x, v_tex_coord.y - y));

// Apply weighted distribution, by using a 3x3 tent filter:
//  1   | 1 2 1 |
// -- * | 2 4 2 |
// 16   | 1 2 1 |
vec4 upsample = e*4.0;
     upsample += (b+d+f+h)*2.0;
     upsample += (a+c+g+i);
     upsample *= 1.0 / 16.0;
     vec4 tone_mapped = vec4(1.0) - exp(-(upsample + source_color)*1.5);
     FragColor =  tone_mapped;
 }
)V0G0N";

constexpr const char *fragment_combine_light = R"V0G0N(#version 300 es 
precision highp float;    

uniform sampler2D u_source;
uniform float u_exposure = 2.0;
uniform float radius = 0.2;

in vec2 v_tex_coord;                          
in vec4 v_color;       

out vec4 FragColor;

void main(void)
{
    vec3 source_color = texture(u_source, v_tex_coord).rgb;      
    float source_alpha = texture(u_source, v_tex_coord).a;

    vec3 result = vec3(1.) - exp(-source_color*u_exposure);
    FragColor = vec4(result, source_alpha);
}
)V0G0N";