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
layout(location = 10) in float a_depth;

out highp vec2 v_tex_coord;
out vec4 v_edge_color;
out vec4 v_color;
out vec4 v_glow_color;
out float v_start_time;

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
    v_tex_coord = vec2(tex_coord.x + tex_dim.x * a_tex_coord.x, tex_coord.y + tex_dim.y * (1.-a_tex_coord.y));
    v_edge_color = a_edge_color;
    v_color = a_fill_color;
    v_glow_color = a_glow_color;
    v_start_time = a_start_time;
}
)V0G0N";

constexpr const char *vertex_sprite_code = R"V0G0N(#version 300 es
precision highp float;

layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_tex_pos;
layout(location = 2) in vec2 a_translation;
layout(location = 3) in vec2 a_scale;
layout(location = 4) in mediump float a_angle;
layout(location = 5) in vec2 a_tex_coord;
layout(location = 6) in vec2 a_tex_dim;
layout(location = 7) in mediump vec4 a_color;
layout(location = 8) in float a_depth;

out highp vec2 v_tex_coord;
out mediump vec4 v_color;
uniform sampler2D u_texture ;
uniform mat4 u_view_projection ;
void main()
{
   vec2 scaled_pos = a_scale * a_position;
   vec2 rotated_pos = vec2(
                            +cos(a_angle) * scaled_pos.x - sin(a_angle) * scaled_pos.y,
                            +sin(a_angle) * scaled_pos.x + cos(a_angle) * scaled_pos.y
                            );
   gl_Position = u_view_projection * vec4(rotated_pos + a_translation, 0., 1.0);
   v_tex_coord= vec2(a_tex_coord.x + a_tex_dim.x * a_tex_pos.x, a_tex_coord.y - a_tex_dim.y * (1. - a_tex_pos.y));
   v_color = a_color;
}
)V0G0N";

constexpr const char *vertex_sprite_code_direct = R"V0G0N(#version 300 es
precision highp float;

layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_tex_pos;

out mediump vec2 v_tex_coord;
out mediump vec4 v_color;

void main()
{
    gl_Position =  vec4(a_position.xy, 0., 1.);
    v_tex_coord= a_tex_pos;
    v_color = vec4(1.);
}
)V0G0N";

constexpr const char *vertex_vertexarray_code = R"V0G0N(#version 300 es
precision highp float;

in vec2 a_position;
in vec4 a_color;
in vec2 a_tex_coord;
in float a_depth;

out highp vec2 v_tex_coord;
out vec4 v_color;

uniform mat4 u_view_projection;

void main()
{
    gl_Position = u_view_projection*vec4(a_position.xy, 0., 1.0);
    gl_Position.z = a_color.a;
    v_color     = a_color;
    v_tex_coord = a_tex_coord;
}
)V0G0N";

constexpr const char *fragment_font_code = R"V0G0N(#version 300 es
precision highp float;
in highp vec2 v_tex_coord;
in vec4 v_color;
out vec4 FragColor;
uniform sampler2D u_texture;
void main()
{               
    float glyph_region = texture(u_texture, v_tex_coord).r;
    FragColor = vec4(glyph_region);
}
)V0G0N";

constexpr const char *fragment_fullpass_code = R"V0G0N(#version 300 es
precision mediump float;
in highp vec2 v_tex_coord;
in vec4 v_color;
out vec4 FragColor;
void main()
{
   FragColor = vec4(v_color.rgb * v_color.a, v_color.a);
}
)V0G0N";

constexpr const char *fragment_fullpass_texture_code = R"V0G0N(#version 300 es
precision mediump float;
uniform sampler2D u_texture;
in highp vec2 v_tex_coord;
in vec4 v_color;
out vec4 FragColor;
void main()
{
    ivec2 tex_sizei = textureSize(u_texture, 0);
    float texelX = 1.0 / float(tex_sizei.x);
    float texelY = 1.0 / float(tex_sizei.y);
    vec2 uv = v_tex_coord;// + 0.5*vec2(texelX, texelY);
    vec4 color = v_color *  texture(u_texture, uv);
    float alpha = color.a * v_color.a;
    FragColor = vec4(color.rgb * alpha, alpha);
}
)V0G0N";

constexpr const char *fragment_fullpass_texture_code_no_alpha = R"V0G0N(#version 300 es
precision highp float;
uniform sampler2D u_texture;
in highp vec2 v_tex_coord;
in vec4 v_color;
out vec4 FragColor;
void main()
{
    vec4 color = texture(u_texture, v_tex_coord);
    FragColor =   v_color *  vec4(color.rgb, color.a);
}
)V0G0N";

constexpr const char *fragment_text_sprite_code = R"V0G0N(#version 300 es
precision highp float;
in highp vec2 v_tex_coord;
in vec4 v_color;

float u_smooth_min = 0.465;
float u_smooth_max = 0.59;
int u_outline= 1;

uniform sampler2D u_texture;
uniform sampler2D u_charmap;

out vec4 FragColor;
void main()
{
    highp float glyph =  texture(u_texture, v_tex_coord).r;
    vec4 color_res = v_color; 
   FragColor =  vec4(v_color.rgb *v_color.a * glyph, v_color.a * glyph);
}
)V0G0N";
constexpr const char *fragment_text_sdf_code = R"V0G0N(#version 300 es
precision highp float;
in highp vec2 v_tex_coord;
in vec4 v_edge_color;
in vec4 v_color;
in vec4 v_glow_color;

uniform float u_smooth_min = 0.465;
uniform float u_smooth_max = 0.59;
uniform float u_glow_min = -0.1;
uniform float u_glow_max = 0.6;
uniform int u_outline = 0;
uniform int u_smooth_edges = 1;
uniform int u_outer_glow = 1;
uniform vec2 u_glow_uv_offset = vec2(0.0, 0.0);
uniform float u_outline_min0 = 0.45;
uniform float u_outline_min1 = 0.48;
uniform float u_outline_max0 = 0.495;
uniform float u_outline_max1 = 0.53;
uniform float u_thick = 0.485;

uniform sampler2D u_texture;
uniform sampler2D u_charmap;

out vec4 FragColor;
void main()
{
    highp float dist_mask =  texture(u_texture, vec2(v_tex_coord.x, v_tex_coord.y)).r;
    vec4 color_res = vec4(0);
    if(u_outline == 1 && dist_mask > u_outline_min0 && dist_mask < u_outline_max1){ 
        float outline_factor = 1.0;
        if(dist_mask < u_outline_min1)
        {
            outline_factor = smoothstep(u_outline_min0, u_outline_min1, dist_mask);
        }else{
            outline_factor = smoothstep(u_outline_max1, u_outline_max0, dist_mask);
        }
        color_res = mix(color_res, v_edge_color, outline_factor);
   }
    float glyph_mask = 0.;
    if(u_smooth_edges == 1)
    {
        glyph_mask = smoothstep(u_smooth_min, u_smooth_max, dist_mask);
    }
    else{
        glyph_mask = float(dist_mask > u_thick);
    }
    color_res = mix(color_res, v_color, glyph_mask);
    
    if(u_outer_glow == 1)
    {
        float glow_texel = texture(u_texture, v_tex_coord + u_glow_uv_offset).r; 
        vec4 glow_color = v_glow_color * smoothstep(u_glow_min, u_glow_max, glow_texel);
        color_res = mix(glow_color, color_res, glyph_mask);
    }
   FragColor =  vec4(color_res.rgb * color_res.a, color_res.a);
}
)V0G0N";

constexpr const char *fragment_text_code = R"V0G0N(#version 300 es
precision highp float;
in highp vec2 v_tex_coord;
in vec4 v_color;
uniform vec4 u_edge_color = vec4(0., 0.,0., 1.);
uniform float u_smoothness = 0.01;
uniform float u_outline_min0 = 0.38;
uniform float u_outline_min1 = 0.41;
uniform float u_outline_max0 = 0.50;
uniform float u_outline_max1 = 0.52;
uniform float u_thick = 0.49;
out vec4 FragColor;
uniform sampler2D u_texture;
void main()
{
   float dist_mask = texture(u_texture, v_tex_coord).r;
   vec4 color_res = vec4(0.);
   float outline_factor = 1.0;
   if(dist_mask > u_outline_min0 && dist_mask < u_outline_max1){
       if(dist_mask < u_outline_min1)
       {
           outline_factor = smoothstep(u_outline_min0, u_outline_min1, dist_mask);
       }else{
           outline_factor = smoothstep(u_outline_max0, u_outline_max1, dist_mask);
       }
   }
   color_res = mix(u_edge_color, v_color, outline_factor);
   float glyph_factor = smoothstep(u_thick - u_smoothness, u_thick + u_smoothness, dist_mask);
   FragColor = color_res*glyph_factor;
})V0G0N";

constexpr const char *fragment_brightness_code = R"V0G0N(#version 300 es
precision highp float;

uniform float u_brightness_threshold = 1.0;
uniform sampler2D u_input;

in highp vec2 v_tex_coord;
in vec4 v_color;
out vec4 FragColor;
void main()
{
    vec4 input_pixel = texture(u_input, v_tex_coord);
    vec3 input_color = input_pixel.rgb ;
    float input_alpha = input_pixel.a;
    // check whether fragment output is higher than threshold, if so output as brightness color
    float brightness = dot(input_color, vec3(0.2126, 0.7152, 0.0722));
    float bright_alpha = smoothstep(u_brightness_threshold, u_brightness_threshold+0.27, brightness);
    FragColor = bright_alpha * vec4(input_color, input_alpha);
})V0G0N";

constexpr const char *fragment_downsample_code = R"V0G0N(#version 300 es
precision highp float;
uniform sampler2D u_image;
in highp vec2 v_tex_coord;
in vec4 v_color;
out vec4 FragColor;
void main(void)
{
    ivec2 tex_sizei = textureSize(u_image, 0);
    vec2 tex_size = vec2(tex_sizei.x, tex_sizei.y);
    vec2 srcPos = v_tex_coord * vec2(tex_size);
    ivec2 base = ivec2(floor(srcPos));
    vec4 c00 = texelFetch(u_image, base, 0);
    vec4 c10 = texelFetch(u_image, base + ivec2(1,0), 0);
    vec4 c01 = texelFetch(u_image, base + ivec2(0,1), 0);
    vec4 c11 = texelFetch(u_image, base + ivec2(1,1), 0);
    FragColor = (c00 + c01 + c10 + c11) * 0.25;
})V0G0N";

constexpr const char *fragment_downsample_kawase_code = R"V0G0N(#version 300 es
precision highp float;
uniform sampler2D u_image;
in highp vec2 v_tex_coord;
in vec4 v_color;
out vec4 FragColor;
void main(void)
{
    ivec2 tex_sizei = textureSize(u_image, 0);
    vec2 tex_size = vec2(tex_sizei.x, tex_sizei.y);
    vec2 half_pixel = vec2(0.5 / tex_size.x, 0.5 / tex_size.y); 
    vec4 sum = texture(u_image, v_tex_coord);
    sum += texture(u_image, v_tex_coord + vec2(half_pixel.x, half_pixel.y));
    sum += texture(u_image, v_tex_coord + vec2(-half_pixel.x, half_pixel.y));
    sum += texture(u_image, v_tex_coord + vec2(-half_pixel.x, -half_pixel.y));
    sum += texture(u_image, v_tex_coord + vec2(half_pixel.x, -half_pixel.y));
    FragColor = sum/8.;
})V0G0N";

constexpr const char *fragment_upsample_kawase_code = R"V0G0N(#version 300 es
precision highp float;
uniform sampler2D u_image;
in highp vec2 v_tex_coord;
in vec4 v_color;
out vec4 FragColor;
void main(void)
{
    ivec2 tex_sizei = textureSize(u_image, 0);
    vec2 tex_size = vec2(tex_sizei.x, tex_sizei.y);
    vec2 half_pixel = vec2(0.5 / tex_size.x, 0.5 / tex_size.y); 
    vec4 sum = texture(u_image, v_tex_coord + vec2(0.0, 2.0*half_pixel.y));
    sum += texture(u_image, v_tex_coord + vec2(half_pixel.x, half_pixel.y))*2.0;
    sum += texture(u_image, v_tex_coord + vec2(2.0*half_pixel.x, 0.0));
    sum += texture(u_image, v_tex_coord + vec2(half_pixel.x, -half_pixel.y))*2.0;
    sum += texture(u_image, v_tex_coord + vec2(0.0, -2.0*half_pixel.y));
    sum += texture(u_image, v_tex_coord + vec2(-half_pixel.x, -half_pixel.y))*2.0;
    sum += texture(u_image, v_tex_coord + vec2(-2.0*half_pixel.x, 0.0));
    sum += texture(u_image, v_tex_coord + vec2(-half_pixel.x, half_pixel.y))*2.0;
    FragColor = sum / 12.;
})V0G0N";

constexpr const char *fragment_gauss_vert_code = R"V0G0N(#version 300 es
precision highp float;
uniform sampler2D u_image;
in highp vec2 v_tex_coord;
in vec4 v_color;
out vec4 FragColor;
void main(void)
{
    float offset[5] = float[]( 0.0, 1.0, 2.0, 3.0, 4.0 );
    float weight[5] = float[]( 0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162 );
    vec4 vertexColor = v_color;
    ivec2 tex_sizei = textureSize(u_image, 0);
    vec2 tex_size = vec2(tex_sizei.x, tex_sizei.y);
    vec2 uv = v_tex_coord + 0.0*vec2(1.0/tex_size.x, 1.0/tex_size.y);
    vec4 result = texture( u_image, vec2(uv)) * weight[0];
    for (int i=1; i<5; i++)
    {
        result += texture( u_image, ( uv+vec2(0.0, offset[i]*1.0/tex_size.y) ) ) * weight[i];
        result += texture( u_image, ( uv-vec2(0.0, offset[i]*1.0/tex_size.y) ) ) * weight[i];
    }
    FragColor = result;
})V0G0N";

constexpr const char *fragment_gauss3x3_code = R"V0G0N(#version 300 es
precision highp float;
uniform sampler2D u_image;
in highp vec2 v_tex_coord;
in vec4 v_color;
out vec4 FragColor;
void main(void)
{
    float offset[5] = float[]( 0.0, 1.0, 2.0, 3.0, 4.0 );
    float weight[5] = float[]( 0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162 );
    vec4 vertexColor = v_color;
    ivec2 tex_sizei = textureSize(u_image, 0);
    vec2 tex_size = vec2(tex_sizei.x, tex_sizei.y);
    vec2 uv = v_tex_coord + 0.0*vec2(1.0/tex_size.x, 1.0/tex_size.y);
    vec4 result = texture( u_image, vec2(uv)) * weight[0];
    for (int i=1; i<5; i++)
    {
        result += texture( u_image, ( uv+vec2(0.0, offset[i]*1.0/tex_size.y) ) ) * weight[i];
        result += texture( u_image, ( uv-vec2(0.0, offset[i]*1.0/tex_size.y) ) ) * weight[i];
    }
    FragColor = result;
})V0G0N";

constexpr const char *fragment_gauss_horiz_code = R"V0G0N(#version 300 es
precision highp float;
uniform sampler2D u_image;
in highp vec2 v_tex_coord;
in vec4 v_color;
out vec4 FragColor;
void main(void)
{
    float offset[5] = float[]( 0.0, 1.0, 2.0, 3.0, 4.0 );
    float weight[5] = float[]( 0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162 );
    vec4 vertexColor = v_color;
    ivec2 tex_sizei = textureSize(u_image, 0);
    vec2 tex_size = vec2(tex_sizei.x, tex_sizei.y);
    vec2 uv = v_tex_coord + 0.0*vec2(1.0/tex_size.x, 1.0/tex_size.y);
    vec4 result = texture( u_image, vec2(uv)) * weight[0];
    for (int i=1; i<5; i++)
    {
        result += texture( u_image, ( uv+vec2(offset[i]*1.0/tex_size.x, 0.0) ) ) * weight[i];
        result += texture( u_image, ( uv-vec2(offset[i]*1.0/tex_size.x, 0.0) ) ) * weight[i];
    }
    FragColor =  result;
})V0G0N";

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
constexpr const char *fragment_downsample13_code = R"V0G0N(#version 300 es
precision highp float;
uniform sampler2D u_texture;
uniform vec2 u_src_resolution;
in highp vec2 v_tex_coord;
in vec4 v_color;
out vec4 FragColor;
void main(){
  vec2 srcTexelSize = vec2(1.0 / u_src_resolution.x, 1.0 / u_src_resolution.y);
  float x = srcTexelSize.x;
  float y = srcTexelSize.y;

  vec4 a = texture(u_texture, vec2(v_tex_coord.x - 2.*x, v_tex_coord.y + 2.*y));
  vec4 b = texture(u_texture, vec2(v_tex_coord.x,       v_tex_coord.y + 2.*y));
  vec4 c = texture(u_texture, vec2(v_tex_coord.x + 2.*x, v_tex_coord.y + 2.*y));

  vec4 d = texture(u_texture, vec2(v_tex_coord.x - 2.*x, v_tex_coord.y));
  vec4 e = texture(u_texture, vec2(v_tex_coord.x,       v_tex_coord.y));
  vec4 f = texture(u_texture, vec2(v_tex_coord.x + 2.*x, v_tex_coord.y));

  vec4 g = texture(u_texture, vec2(v_tex_coord.x - 2.*x, v_tex_coord.y - 2.*y));
  vec4 h = texture(u_texture, vec2(v_tex_coord.x,       v_tex_coord.y - 2.*y));
  vec4 i = texture(u_texture, vec2(v_tex_coord.x + 2.*x, v_tex_coord.y - 2.*y));

  vec4 j = texture(u_texture, vec2(v_tex_coord.x - x, v_tex_coord.y + y));
  vec4 k = texture(u_texture, vec2(v_tex_coord.x + x, v_tex_coord.y + y));
  vec4 l = texture(u_texture, vec2(v_tex_coord.x - x, v_tex_coord.y - y));
  vec4 m = texture(u_texture, vec2(v_tex_coord.x + x, v_tex_coord.y - y));

  vec4 downsample = e*0.125;
  downsample += (a+c+g+i)*0.03125;
  downsample += (b+d+f+h)*0.0625;
  downsample += (j+k+l+m)*0.125;
  FragColor = downsample;
})V0G0N";

constexpr const char *fragment_combine_bloom_code = R"V0G0N(#version 300 es
precision highp float;
uniform sampler2D u_source;
uniform sampler2D u_bloom;
uniform float exposure = 1.5;
in highp vec2 v_tex_coord;
in vec4 v_color;
out vec4 FragColor;
void main(void)
{
    const float gamma = 2.2;
    vec2 tex_size_s = vec2(textureSize(u_source, 0));
    vec2 tex_size_b = vec2(textureSize(u_bloom, 0));
    vec2 uv_s = v_tex_coord + vec2(0.5/tex_size_s.x, 0.5/tex_size_s.y);
    vec2 uv_b = v_tex_coord + vec2(0.5/tex_size_b.x, 0.5/tex_size_b.y);
    vec3 source_color = texture(u_source, v_tex_coord).rgb;
    vec3 bloom_color = texture(u_bloom, v_tex_coord).rgb;
    float source_alpha = texture(u_source, v_tex_coord).a;
    float bloom_alpha = texture(u_bloom, v_tex_coord).a;
    vec3 hdr_color = source_color + bloom_color;             // additive blending
    vec3 result = min(hdr_color, vec3(1.));             // additive blending
    //vec3 result = 1.0 - exp(-hdr_color * exposure);    // tone mapping
    float result_alpha = 1.0 - exp(-(bloom_alpha + source_alpha) *exposure);    // tone mapping
    result = pow(result, vec3(1.0 / gamma));                // gamma correction
    float alpha = min(source_alpha+bloom_alpha, 1.);
    FragColor = vec4(result*alpha, alpha);
})V0G0N";

constexpr const char *fragment_edge_detect_code = R"V0G0N(#version 300 es
precision highp float;
in highp vec2 v_tex_coord;
out vec4 FragColor;
uniform sampler2D u_input;
void main() {
  vec2 texel_size = 1.0 / vec2(textureSize(u_input, 0));
  float kernelX[9] = float[9](
       1.0,  0.0, -1.0,
       2.0,  0.0, -2.0,
       1.0,  0.0, -1.0
  );
  float kernelY[9] = float[9](
       1.0,  2.0,  1.0,
       0.0,  0.0,  0.0,
      -1.0, -2.0, -1.0
  );
  float edgeX = 0.0;
  float edgeY = 0.0;
  int idx = 0;
  for (int y = -1; y <= 1; ++y) {
      for (int x = -1; x <= 1; ++x) {
          vec2 offset = vec2(float(x), float(y)) * texel_size;
          vec3 input_color = texture(u_input, v_tex_coord + offset).rgb;
          float input_alpha = texture(u_input, v_tex_coord + offset).a;
          float lum = dot(input_color.rgb, vec3(0.299, 0.587, 0.114));
          edgeX += kernelX[idx] * input_alpha;
          edgeY += kernelY[idx] * input_alpha;
          idx++;
      }
  }
  float magnitude = length(vec2(edgeX, edgeY));
  FragColor = vec4(vec3(magnitude), min(magnitude, 1.0));
})V0G0N";
constexpr const char *fragment_edge_combine_code = R"V0G0N(#version 300 es
precision highp float;    
uniform sampler2D u_source;
uniform sampler2D u_edge;
uniform vec3 edge_color = vec3(0., 0.5, 5.);
in highp vec2 v_tex_coord;                          
in vec4 v_color;       
out vec4 FragColor;
void main(void)
{
   vec3 source_color = texture(u_source, v_tex_coord).rgb;      
   float source_alpha = texture(u_source, v_tex_coord).a;
   float edge_alpha = texture(u_edge, v_tex_coord).a;
   FragColor = vec4(edge_color * edge_alpha + source_color*(1.-edge_alpha), source_alpha + edge_alpha);
})V0G0N";

constexpr const char *fragment_downsample_cheap_code = R"V0G0N(#version 300 es
precision mediump float;

uniform sampler2D u_texture;
uniform highp vec2 u_src_resolution;
uniform float u_threshold = 0.;

in mediump vec2 v_tex_coord;

out vec4 FragColor;

void main(){
    
// sample at 4 points offset by half a texel
// hardware bilinear does a 2x2 average for free at each tap
// so this is effectively a 4x4 = 16 pixel average with only 4 taps
//  mediump vec2 halfTexel = vec2(0.5 / u_src_resolution.x, 0.5 / u_src_resolution.y);
//  vec4 a = texture(u_texture, v_tex_coord + vec2(-halfTexel.x,  halfTexel.y));
//  vec4 b =texture(u_texture, v_tex_coord + vec2( halfTexel.x,  halfTexel.y));
//  vec4 c = texture(u_texture, v_tex_coord + vec2(-halfTexel.x, -halfTexel.y));
//  vec4 d =texture(u_texture, v_tex_coord + vec2( halfTexel.x, -halfTexel.y));

//    vec4 avg = (a + b + c + d) / 4.;
    vec4 center = texture(u_texture, v_tex_coord);
    vec4 avg = center;

    // brightness threshold
    float brightness = dot(avg.rgb, vec3(0.2126, 0.7152, 0.0722));
    float soft = brightness - u_threshold;
    soft = clamp(soft, 0.0, 1.0);
    FragColor = vec4(avg.rgb , avg.a);
}
)V0G0N";

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
in highp vec2 v_tex_coord;
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
uniform float u_source_weight = 1.;
uniform float u_aspect_ratio = 1.;

in highp vec2 v_tex_coord;
in vec4 v_color;
out vec4 FragColor;
void main(){
  float x = u_filter_radius;
  float y = u_filter_radius * u_aspect_ratio;

  vec4 source_color = texture(u_source, vec2(v_tex_coord.x, v_tex_coord.y));

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
     vec4 tone_mapped = vec4(1.0) - exp(-(upsample + u_source_weight * source_color)*1.5);
     float alpha = min(max(tone_mapped.a, source_color.a), 1.);
     FragColor =  vec4(tone_mapped.rgb, alpha);
 }
)V0G0N";

constexpr const char *fragment_combine_light = R"V0G0N(#version 300 es 
precision highp float;

uniform sampler2D u_source;
uniform float u_exposure = 2.0;

in highp vec2 v_tex_coord;                          
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

constexpr const char *fragment_map_from_hdr = R"V0G0N(#version 300 es 
precision highp float;

uniform sampler2D u_source;
uniform float u_exposure = 2.0;

in highp vec2 v_tex_coord;                          
in vec4 v_color;       

out vec4 FragColor;

void main(void)
{
    vec3 source_color = texture(u_source, v_tex_coord).rgb;      
    float source_alpha = texture(u_source, v_tex_coord).a;

    vec3 result = vec3(1.) - exp(-source_color*u_exposure);
    FragColor = vec4(result*source_alpha, source_alpha);
}
)V0G0N";

constexpr const char *fragment_upsample_mix_kawase_code = R"V0G0N(#version 300 es
precision highp float;

uniform sampler2D u_source;
uniform sampler2D u_texture;

uniform float u_filter_radius;
uniform float u_source_weight = 1.;

in highp vec2 v_tex_coord;
in vec4 v_color;
out vec4 FragColor;
void main(){

    vec4 source_color = texture(u_source, vec2(v_tex_coord.x, v_tex_coord.y));
    
    ivec2 tex_sizei = textureSize(u_texture, 0);
    vec2 tex_size = vec2(tex_sizei.x, tex_sizei.y);
    vec2 half_pixel = vec2(0.5 / tex_size.x, 0.5 / tex_size.y); 
    
    vec4 sum = texture(u_texture, v_tex_coord + vec2(0.0, 2.0*half_pixel.y));
    sum += texture(u_texture, v_tex_coord + vec2(half_pixel.x, half_pixel.y))*2.0;
    sum += texture(u_texture, v_tex_coord + vec2(2.0*half_pixel.x, 0.0));
    sum += texture(u_texture, v_tex_coord + vec2(half_pixel.x, -half_pixel.y))*2.0;
    sum += texture(u_texture, v_tex_coord + vec2(0.0, -2.0*half_pixel.y));
    sum += texture(u_texture, v_tex_coord + vec2(-half_pixel.x, -half_pixel.y))*2.0;
    sum += texture(u_texture, v_tex_coord + vec2(-2.0*half_pixel.x, 0.0));
    sum += texture(u_texture, v_tex_coord + vec2(-half_pixel.x, half_pixel.y))*2.0;

     vec4 tone_mapped = vec4(1.0) - exp(-(sum + u_source_weight * source_color)*1.5);
     float alpha = min(max(tone_mapped.a, source_color.a), 1.);
//     FragColor =  vec4(tone_mapped.rgb, alpha);
     FragColor =  vec4(tone_mapped.rgb, alpha);
 }
)V0G0N";
