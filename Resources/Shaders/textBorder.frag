#version 300 es

precision mediump float;                     
in vec2 v_tex_coord;
in vec4 v_color;                          

uniform vec4 u_edge_color = vec4(1., 0.,0.,1.);
uniform float u_time = 0.;
uniform float u_time_factor = 10.;

out vec4 FragColor;

uniform sampler2D u_texture;

void main()                                  
{        

    float r_t = (1. + cos(u_time_factor * u_time))/2.;
    float g_t = (1. + sin(u_time_factor * u_time + v_tex_coord.x*5.))/2.;
    float b_t = (1. + sin(0.5f*u_time_factor * u_time + v_tex_coord.x))/2.;

    vec3 edge_color = vec3(r_t, g_t, b_t);

    float glyph_region = texture(u_texture, v_tex_coord).a;         
    float x = smoothstep(0.40, 0.5, glyph_region);  
    float edge = smoothstep(0.45, 0.49, glyph_region)  - smoothstep(0.51, 0.55, glyph_region);  

    FragColor = vec4(v_color.rgb*x*(1.-edge) + edge*edge_color.rgb, x); //vec4(vec3(1. - glyph_region), 1.); 
}                                            