#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QtOpenGL>
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QDebug>

#include <memory>
#include <model.h>

#include "camera.h"
#include "light.h"

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
    Q_OBJECT

    int NUM_TARGETS = 3;
    int NUM_STRIPS = 8;
    int NUM_GRASS = 4;

    std::shared_ptr<Model> playerModel = nullptr;
    std::shared_ptr<Model> targetModel = nullptr;
    std::shared_ptr<Model> roadModel = nullptr;
    std::shared_ptr<Model> roadstripModel = nullptr;
    std::shared_ptr<Model> grassModel = nullptr;
    std::shared_ptr<Model> gasTankModel = nullptr;

    float playerPosXOffset; // Player displacement along Y axis
    float playerPosYOffset;
    float playerPosX; // Current player X position
    float playerPosY;
    float playerSize;

    std::unique_ptr<float[]> roadPosY;
    std::unique_ptr<float[]> roadstripsPosY;

    std::unique_ptr<float[]> grassPosY;

    float targetPosYOffset = -0.7;
    float targetSize;
    std::unique_ptr<float[]> targetsPosY;
    std::unique_ptr<float[]> targetsPosX;

    float gasTankPosY;
    float gasTankPosX;
    float gasTankSize;
    QVector3D gasTankRotation;
    int gasAvailable;
    int throwGasTank;

    QTimer timer;
    QTime time;
    float totalTime;

    float score;
    int finalScore;
    int lose;

public:
    explicit OpenGLWidget(QWidget *parent = nullptr);
    ~OpenGLWidget();

    void applyLightParams(std::shared_ptr<Model> model);

    float calculateDistance(float x1, float y1, float x2, float y2);

    Camera camera;
    Light light;

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
signals:
    void updateScoreLabel(QString);
public slots:
    void animate();
};

#endif // OPENGLWIDGET_H
