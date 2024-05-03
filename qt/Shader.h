#pragma once
#include <QOpenGLFunctions_3_3_Compatibility>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace std;
using namespace glm;

enum Error_Type
{
    SHADER_VERTEX_COMPILE,
    SHADER_FRAGMENT_COMPILE,
    PROGRAM_LINK
};

class Shader : public QOpenGLFunctions_3_3_Compatibility
{
public:
    Shader(const char *VertexShaderPath, const char *FragmentShaderPath);//构造函数
    unsigned int ID;//着色器程序的ID
    void ErrorCheck(unsigned int ShaderID, const Error_Type ErrorType);//错误信息输出
    void use();//激活着色器程序
    void setFloat(const string &uniformname, float value);//设置着色器程序中单个float的值
    void setInt(const string &uniformname, int value);//设置着色器程序中单个int的值
    void SetMat4(const string &uniformname, mat4& Mat4);//设置着色器程序中单个mat4的值
    void SeVec3(const string &uniformname, vec3 Vec3);//设置着色器程序中单个vec3的值
    void SeVec4(const string &uniformname, vec4 Vec4);//设置着色器程序中单个vec4的值
private:

protected:

};

