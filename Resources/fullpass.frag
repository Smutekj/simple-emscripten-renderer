#version 300 es 

precision mediump float;    
                 
in vec2 v_tex_coord;                          
in vec4 v_color;       

out vec4 FragColor;

//uniform sampler2D u_texture;

void main()                                  
{            
    FragColor =   v_color; //   vec4(vec3(gl_FragCoord.z), 1.); //  
}                                            