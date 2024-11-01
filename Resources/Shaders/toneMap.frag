#version 300 es 

precision mediump float;    

uniform sampler2D u_source;

uniform float exposure = 0.5;

in vec2 v_tex_coord;                          
in vec4 v_color;       

out vec4 FragColor;

void main(void)
{

    const float gamma = 2.2;
    
    vec3 source_color = texture(u_source, v_tex_coord).rgb;      
    float source_alpha = texture(u_source, v_tex_coord).a;

    vec3 result = vec3(1.0) - exp(-source_color *source_alpha* exposure);    // tone mapping
    result = pow(result, vec3(1.0 / gamma));                // gamma correction      

    FragColor = vec4(result, 1.0);
}
