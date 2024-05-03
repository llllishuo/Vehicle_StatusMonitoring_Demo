#include "Scene.h"

Scene::Scene()
{
    initializeOpenGLFunctions();//初始化Opengl
}

void Scene::PointsSet()
{
    //配置VAO
    glGenVertexArrays(1, &VAO_Pnt_Cloud);
    glBindVertexArray(VAO_Pnt_Cloud);

    //将内存中的顶点数据放入GPU内存中
    glGenBuffers(1, &VBO_Pnt_Cloud);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Pnt_Cloud);
    glBufferData(GL_ARRAY_BUFFER, 6*sizeof(float)*10000, NULL, GL_DYNAMIC_DRAW);
    //设置GPU如何读取被绑定的VBO
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);



    //配置VAO
    glGenVertexArrays(1, &VAO_Coord);
    glBindVertexArray(VAO_Coord);
    float Vertices[] = {//局部空间(Local Space，或者称为物体空间(Object Space))的坐标
                        // 位置              // 颜色
                             0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
                             1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
                             0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
                             0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
                             0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
                             0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
    };
    // 将内存中的顶点数据放入GPU内存中
    glGenBuffers(1, &VBO_Coord);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Coord);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    //设置GPU如何读取被绑定的VBO
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}
