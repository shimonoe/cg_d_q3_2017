#ifndef MODEL_H
#define MODEL_H

#include <QtOpenGL>
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QTextStream>
#include <QFile>

#include <fstream>
#include <limits>
#include <iostream>
#include <memory>

#include "material.h"
#include "util.h"

class Model : public QOpenGLExtraFunctions
{
public:
    Model(QOpenGLWidget *_glWidget);
    ~Model();

    QOpenGLWidget *glWidget;

    std::unique_ptr<QVector4D[]> vertices;
    std::unique_ptr<unsigned int[]> indices;
    std::unique_ptr<QVector3D[]> normals;
    std::unique_ptr<QVector2D []> texCoords;

    unsigned int numVertices;
    unsigned int numFaces;

    GLuint vao = 0;

    GLuint vboVertices = 0;
    GLuint vboIndices = 0;
    GLuint vboNormals = 0;
    GLuint vboTexCoords = 0;
    GLuint textureID = 0;

    GLuint shaderProgram;

    QMatrix4x4 modelMatrix;
    QVector3D midPoint;
    double invDiag;

    Material material;

    void createVBOs();
    void createShaders(QString vertexShaderFile, QString fragmentShaderFile);
    void createNormals();

    void destroyVBOs();
    void destroyShaders();

    void readOFFFile(const QString &fileName);

    void drawModel(float posX, float posY, float posZ, float scale, QVector3D rotation);

    void createTexCoords();
    void loadTexture(const QString imagepath);
};

#endif // MODEL_H
