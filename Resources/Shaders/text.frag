#version 300 es

precision mediump float;                     
in vec2 v_tex_coord;
in vec4 v_color;                          

out vec4 FragColor;

uniform sampler2D u_texture;

void main()                                  
{               
    float glyph_region = texture(u_texture, v_tex_coord).a; 
    float x = smoothstep(0., 0.9, glyph_region); 
    FragColor = vec4(vec3(1.-x), glyph_region); //vec4(vec3(1. - glyph_region), 1.); 
}                                            