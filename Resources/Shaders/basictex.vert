#version 300 es

precision mediump float;   

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