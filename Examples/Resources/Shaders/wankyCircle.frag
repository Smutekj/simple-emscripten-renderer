#version 300 es

precision mediump float;                     

in vec2 v_tex_coord;
in vec4 v_color;                          

out vec4 FragColor;

uniform float u_random_factor = 0.05;
uniform float u_speed_factor= 0.05;
uniform vec4 u_color = vec4(0., 1., 0., 1.);
uniform float u_time = 0.;
uniform sampler2D u_texture;

float hash(float x)
{
    float xx =x*60239.16816818;
    return sin(fract(xx));
};

void main()                                  
{                
    vec2 uv = v_tex_coord;
    vec2 center = vec2(0.5);
    float radius = distance(center, uv);
    float circle_sdf =  radius - 0.4;
    float mask = smoothstep(-0.1, 0.1, circle_sdf + u_random_factor * hash(radius + u_speed_factor * u_time));

    FragColor =  u_color * mask;
}                                            