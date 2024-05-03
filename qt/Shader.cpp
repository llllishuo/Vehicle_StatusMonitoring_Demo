#include "Shader.h"

Shader::Shader(const char *VertexShaderPath, const char *FragmentShaderPath)
{//构造函数
    initializeOpenGLFunctions();//初始化Opengl
    string vShaderString, fShaderString;
    ifstream VertexShaderFile, FragmentShaderFile;
    VertexShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
    FragmentShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
    try
    {
        //利用文件流将文件从硬盘读到缓冲中
        VertexShaderFile.open(VertexShaderPath);
        FragmentShaderFile.open(FragmentShaderPath);
        //读取缓冲区的内容到数据流中
        stringstream VertexShaderStream, FragmentShaderStream;
        VertexShaderStream << VertexShaderFile.rdbuf();
        FragmentShaderStream << FragmentShaderFile.rdbuf();
        //关闭文件处理器
        VertexShaderFile.close();
        FragmentShaderFile.close();
        //转换数据格式到string
        vShaderString = VertexShaderStream.str();
        fShaderString = FragmentShaderStream.str();
    }
    catch (ifstream::failure e)
    {
        cout << "ERROR::" << "SHADER::" << "failed to read the shader files";
    }
    //再转换成char*格式
    const char* vShaderCode;
    const char* fShaderCode;
    vShaderCode = vShaderString.c_str();
    fShaderCode = fShaderString.c_str();
    unsigned int vshader, fshader;
    vshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshader, 1, &vShaderCode, NULL);
    glCompileShader(vshader);
    ErrorCheck(vshader, SHADER_VERTEX_COMPILE);
    fshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshader, 1, &fShaderCode, NULL);
    glCompileShader(fshader);
    ErrorCheck(fshader, SHADER_FRAGMENT_COMPILE);
    //连接并链接着色器为着色器程序
    ID = glCreateProgram();
    glAttachShader(ID, vshader);
    glAttachShader(ID, fshader);
    glLinkProgram(ID);
    ErrorCheck(ID, PROGRAM_LINK);
    glDeleteShader(vshader);
    glDeleteShader(fshader);
}

void Shader::ErrorCheck(unsigned int ShaderID, const Error_Type ErrorType)
{
    int success;
    char infoLog[512];
    if (ErrorType == SHADER_VERTEX_COMPILE)
    {
        glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(ShaderID, 512, NULL, infoLog);
            cout << "ERROR:: SHADER:: VERTEX" << infoLog << endl;
        }
    }
    else if (ErrorType == SHADER_FRAGMENT_COMPILE)
    {
        glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(ShaderID, 512, NULL, infoLog);
            cout << "ERROR:: SHADER:: FRAGMENT::" << infoLog << endl;
        }
    }
    else if (ErrorType == PROGRAM_LINK)
    {
        glGetProgramiv(ShaderID, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(ShaderID, 512, NULL, infoLog);
            cout << "ERROR:: SHADER:: PROGRAM::" << infoLog << endl;
        }
    }
}

void Shader::use()
{
    //激活着色器程序
    glUseProgram(ID);
}

void Shader::setFloat(const string &uniformname, float value)
{
    //设置单个unifrom float的值
    unsigned int UniformLocation = glGetUniformLocation(ID, uniformname.c_str());
    glUniform1f(UniformLocation, value);
}

void Shader::setInt(const string &uniformname, int value)
{
    //设置单个unifrom int的值
    unsigned int UniformLocation = glGetUniformLocation(ID, uniformname.c_str());
    glUniform1i(UniformLocation, value);
}

void Shader::SetMat4(const string &uniformname, mat4& Mat4)
{
    //设置单个unifrom mat4的值
    unsigned int UniformLocation = glGetUniformLocation(ID, uniformname.c_str());
    glUniformMatrix4fv(UniformLocation, 1, GL_FALSE, &Mat4[0][0]);
}

void Shader::SeVec3(const string &uniformname, vec3 Vec3)
{
    //设置单个unifrom vec3的值
    unsigned int UniformLocation = glGetUniformLocation(ID, uniformname.c_str());
    glUniform3fv(UniformLocation, 1, &Vec3[0]);
}

void Shader::SeVec4(const string &uniformname, vec4 Vec4)
{
    //设置单个unifrom vec3的值
    unsigned int UniformLocation = glGetUniformLocation(ID, uniformname.c_str());
    glUniform4fv(UniformLocation, 1, &Vec4[0]);
}
