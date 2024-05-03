#ifndef MODEL_3D_H
#define MODEL_3D_H
#pragma execution_character_set("utf-8") //允许中文
#include <QVector3D>
#include <QMatrix4x4>
#include <QPainter>
#include <QtMath>
#include <QWidget>
#include <QMouseEvent>

class model_3d : public QWidget
{
    Q_OBJECT
public:
    model_3d();
    explicit model_3d(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event)override;
    void changeRotateByRPY(float roll,float pitch, float yaw);
private:
    QPointF getPoint(const QVector3D &vt,int w) const;
    QVector<QVector3D> vertexArr;   //八个顶点
    QVector<QVector<int>> elementArr; //六个面
    QMatrix4x4 rotateMat;   //旋转矩阵
};

#endif // MODEL_3D_H
