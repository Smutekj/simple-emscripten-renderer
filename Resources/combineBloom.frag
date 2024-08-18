#version 300 es 

precision mediump float;    

uniform sampler2D u_source;
uniform sampler2D u_bloom;

uniform float exposure = 2.0;

in vec2 v_tex_coord;                          
in vec4 v_color;       

out vec4 FragColor;

void main(void)
{

    const float gamma = 2.2;

    vec3 hdrColor = texture(u_source, v_tex_coord).rgb;      
    float source_alpha = texture(u_source, v_tex_coord).a;   

    vec3 bloomColor = texture(u_bloom, v_tex_coord).rgb;
    hdrColor += bloomColor; // additive blending

    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);

    // gamma correction    
    result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);

}
