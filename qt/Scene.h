#ifndef SCENE_H
#define SCENE_H

#include <QOpenGLFunctions_3_3_Compatibility>

class Scene:protected QOpenGLFunctions_3_3_Compatibility
{
public:
    Scene();
    ~Scene()=default;
    void PointsSet();
    unsigned int VAO_Pnt_Cloud,VBO_Pnt_Cloud, VAO_Coord, VBO_Coord;
};

#endif // SCENE_H
