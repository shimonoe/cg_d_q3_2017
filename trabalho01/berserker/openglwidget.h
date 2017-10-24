#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QtOpenGL>
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>

#include <memory>

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
    Q_OBJECT

    GLuint vboVertices = 0;
    GLuint vboColors = 0;
    GLuint vboIndices = 0;

    GLuint vao = 0;

    std::unique_ptr<QVector4D []> vertices = nullptr;
    std::unique_ptr<QVector4D []> colors = nullptr;
    std::unique_ptr<unsigned int[]> indices = nullptr;

    GLuint shaderProgram;

    float playerPosXOffset; // Player displacement along X axis
    float playerPosYOffset; // Player displacement along Y axis
    float playerPosX; // Current player X position
    float playerPosY; // Current player Y position
    float playerSize; // Fixed player size

    float targetPosYOffset; // Target displacement along Y axis
    float targetPosX; // Current target X position
    float targetPosY; // Current target Y position
    float targetSize; // Current size of a target
    float targetOffset; // Offset based on target size

    bool shooting; // Whether the projectile is being animated
    int shootingDirection; // Indicate shooting directions (0:up, 1:right, 2:down, 3:left)
    float projectilePosX; // Projectile X position
    float projectilePosY; // Projectile Y position

    int numHits; // Number of hits
    float totalTime;

    QTimer timer;
    QTime time;

public:
    explicit OpenGLWidget (QWidget *parent = 0);
    ~OpenGLWidget();

    void createVBOs();
    void createShaders();

    void destroyVBOs();
    void destroyShaders();

protected :
    void initializeGL();
    void resizeGL (int width, int height);
    void paintGL();

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void setShootingParameters();

signals:
    void updateHitsLabel(QString);

public slots:
    void animate();
};
#endif
