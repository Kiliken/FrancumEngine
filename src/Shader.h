#pragma once

#include <string>
#include <unordered_map>


struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FramgentSource;
};


class Shader
{
private:
    std::string m_FilePath;
    unsigned int m_RendererID;
    std::unordered_map<std::string, int> m_UniformLocationCache;
public:
    Shader(const std::string& filepath);
    ~Shader();

    void Bind() const;
    void Unbind() const;

    void SetUnisform4f(const std::string& name, float v0, float v1, float v2, float v3);
    ShaderProgramSource ParseShaders();
    unsigned int CompileShader(unsigned int type, const std::string& source);
    unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

private:
    int GetUniformLocation(const std::string& name);
};
