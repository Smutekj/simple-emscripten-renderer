#include <gtest/gtest.h>

#include <Shader.h>
#include "../CommonShaders.inl"

TEST(TestShaders, ShaderCompilation)
{
    Shader basic_instanced_shader(vertex_font_code, fragment_fullpass_code);
    
}
