#include "openglwidget.h"

#include <stdlib.h>


OpenGLWidget::OpenGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    playerPosX = 0;
    playerPosY = -2.5;
    playerPosXOffset = 0;
    playerPosYOffset = 0;
    playerSize = 0.2f;

    targetSize = 0.1f;

    gasTankSize = 0.2f;
    gasAvailable = 100;
    throwGasTank = -400;
    gasTankPosY = 8.0f;
    gasTankPosX = 0.0f;
    gasTankRotation = QVector3D(0, 0, 0);

    lose = 0;
    score = 0;
    finalScore = 0;

}

OpenGLWidget::~OpenGLWidget()
{

}


void OpenGLWidget::applyLightParams(std::shared_ptr<Model> model)
{
    int shaderProgramID = model->shaderProgram;

    QVector4D ambientProduct = light.ambient * model->material.ambient;
    QVector4D diffuseProduct = light.diffuse * model->material.diffuse;
    QVector4D specularProduct = light.specular * model->material.specular;

    GLuint locProjection = glGetUniformLocation(shaderProgramID, "projection");
    GLuint locView = glGetUniformLocation(shaderProgramID, "view");

    GLuint locLightPosition = glGetUniformLocation(shaderProgramID, "lightPosition");
    GLuint locAmbientProduct = glGetUniformLocation(shaderProgramID, "ambientProduct");
    GLuint locDiffuseProduct = glGetUniformLocation(shaderProgramID, "diffuseProduct");
    GLuint locSpecularProduct = glGetUniformLocation(shaderProgramID, "specularProduct");
    GLuint locShininess = glGetUniformLocation(shaderProgramID, "shininess");

    glUniformMatrix4fv(locProjection, 1, GL_FALSE, camera.projectionMatrix.data());
    glUniformMatrix4fv(locView, 1, GL_FALSE, camera.viewMatrix.data());

    glUniform4fv(locLightPosition, 1, &(light.position[0]));
    glUniform4fv(locAmbientProduct, 1, &(ambientProduct[0]));
    glUniform4fv(locDiffuseProduct, 1, &(diffuseProduct[0]));
    glUniform4fv(locSpecularProduct, 1, &(specularProduct[0]));
    glUniform1f(locShininess, playerModel->material.shininess);
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    qDebug("OpenGL version: %s", glGetString(GL_VERSION));
    qDebug("GLSL %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    glEnable(GL_DEPTH_TEST);

    playerModel = std::make_shared<Model>(this);
    playerModel->readOFFFile(":/models/car.off");

    targetModel = std::make_shared<Model>(this);
    targetModel->readOFFFile(":/models/barriere.off");

    roadModel = std::make_shared<Model>(this);
    roadModel->readOFFFile(":/models/road.off");

    roadstripModel = std::make_shared<Model>(this);
    roadstripModel->readOFFFile(":/models/roadstrip.off");

    grassModel = std::make_shared<Model>(this);
    grassModel->readOFFFile(":/models/grass.off");

    gasTankModel = std::make_shared<Model>(this);
    gasTankModel->readOFFFile(":/models/gastank.off");

    connect(&timer, SIGNAL(timeout()), this, SLOT(animate()));
    timer.start(0);

    roadPosY = std::make_unique<float[]>(2);

    roadPosY[0] = 0.0f;
    roadPosY[1] = 4.0f;
    roadPosY[2] = 8.0f;

    srand((unsigned int)time.currentTime().msec());

    targetsPosX = std::make_unique<float[]>(NUM_TARGETS);
    targetsPosY = std::make_unique<float[]>(NUM_TARGETS);

    for (int i = 0; i < NUM_TARGETS; i++) {
        double r = ((double) rand()/(RAND_MAX)) + 1;
        targetsPosY[i] = 4.0f + 4.0f*r;
        if (i) r *= -1;
        targetsPosX[i] = 0.0f + 1.0f*r;
    }

    roadstripsPosY = std::make_unique<float[]>(NUM_STRIPS);
    roadstripsPosY[0] = 0.0f;
    roadstripsPosY[1] = 2.0f;
    roadstripsPosY[2] = -2.0f;
    roadstripsPosY[3] = 4.0f;
    roadstripsPosY[4] = -4.0f;
    roadstripsPosY[5] = 6.0f;
    roadstripsPosY[6] = -6.0f;
    roadstripsPosY[7] = 8.0f;

    grassPosY = std::make_unique<float[]>(NUM_GRASS);
    grassPosY[0] = 0.0f;
    grassPosY[1] = 0.5f;
    grassPosY[2] = 4.0f;
    grassPosY[3] = 4.5f;

    time.start();
}

void OpenGLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    camera.resizeViewport(width, height);

    update();
}

void OpenGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.3, 0.33, 0.33, 1);

    if (playerModel)
    {
        applyLightParams(playerModel);
        playerModel->drawModel(playerPosX, playerPosY, 0.23f, playerSize, QVector3D(0,0,0));
    }

    if (targetModel)
    {
        applyLightParams(targetModel);
        for (int i = 0; i < NUM_TARGETS; i++)
            targetModel->drawModel(targetsPosX[i], targetsPosY[i], 0.45f, targetSize, QVector3D(0,0,0));
    }

    if (roadModel)
    {
        applyLightParams(roadModel);
        for (int i = 0; i < 3; i++)
            roadModel->drawModel(-0.4, roadPosY[i], 0.0, 2.0f, QVector3D(0,0,0));
    }

    if (roadstripModel)
    {
        applyLightParams(roadstripModel);
        for (int i = 0; i < NUM_STRIPS; i++)
            roadstripModel->drawModel(0.0, roadstripsPosY[i], 0.06f, 0.15f, QVector3D(0, 0, 0));
    }

    if (grassModel)
    {
        applyLightParams(grassModel);
        for(int i = 0; i < NUM_GRASS; i++){
            if(i % 2 == 0) {
                grassModel->drawModel(-3.1f, grassPosY[i], 0.06f, 0.3f, QVector3D(0, 0, 0));
            } else {
                grassModel->drawModel(3.1f, grassPosY[i], 0.06f, 0.3f, QVector3D(0, 0, 0));
            }
        }
    }


    if (gasTankModel)
    {
        applyLightParams(gasTankModel);
        gasTankModel->drawModel(gasTankPosX, gasTankPosY, 0.4f, gasTankSize, gasTankRotation);
    }

    if (lose) {
        disconnect(&timer, SIGNAL(timeout()), this, SLOT(animate()));
    }

}

void OpenGLWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Left)
    {
        playerPosXOffset = -2.0f*0.48;;
    }

    if (event->key() == Qt::Key_Right)
    {
        playerPosXOffset = 2.0f*0.48;;
    }

    if (event->key() == Qt::Key_Escape)
    {
        QApplication::quit();
    }

}

void OpenGLWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Left)
        playerPosXOffset = 0;

    if (event->key() == Qt::Key_Right)
        playerPosXOffset = 0;
}

void OpenGLWidget::animate()
{
    float elapsedTime = time.restart() / 300.0f;
    elapsedTime += elapsedTime * score/500;
    // Change player X position
    playerPosX += playerPosXOffset * elapsedTime;

    // Check bounds
    // player
    if (playerPosX < -2.0f)
        playerPosX = -2.0f;
    if (playerPosX > 2.0f)
        playerPosX = 2.0f;

    // road
    for (int i = 0; i < 3; i++) {
        if (roadPosY[i] < -2.5f){
            roadPosY[i] = 4.0f;
        } else {
            roadPosY[i] += targetPosYOffset * elapsedTime;
        }
    }
    // roadstrips
    for (int i = 0; i < 8; i++) {
        if (roadstripsPosY[i] < -4.0f){
            roadstripsPosY[i] = 8.0f;
        } else {
            roadstripsPosY[i] += targetPosYOffset * elapsedTime;
        }
    }
    // grass
    for (int i = 0; i < NUM_GRASS; i++) {
        if (grassPosY[i] < -3.0f) {
            grassPosY[i] = 7.8f;
        } else {
            grassPosY[i] += targetPosYOffset * elapsedTime;
        }
    }

    // gastank throw logic
    if (gasTankPosY < -4.0f) {
        if(throwGasTank > -1) {
            srand((unsigned int)time.currentTime().msec());
            int r = rand() % 100 + 1;
            if (r % 2 == 0) r *= -1;
            gasTankPosY = 8.0f + 1.0f*r;
            gasTankPosX = 0.0f + 0.3f*r;
            throwGasTank = -5;
        } else {
            if (throwGasTank < 0) {
                throwGasTank++;
            }
        }
    } else {
        gasTankPosY += targetPosYOffset * elapsedTime * 1.1f;
    }
    if (gasTankPosX < -2.0f)
        gasTankPosX = -2.0f;
    if (gasTankPosX > 2.0f)
        gasTankPosX = 2.0f;


    //targets
    srand((unsigned int) time.currentTime().msec());
    for (int i = 0; i < NUM_TARGETS; i++)
    {
        if (targetsPosY[i] < -8.0f) {
            double r = ((double) rand()/(RAND_MAX)) + 1;
            targetsPosY[i] = 8.0f + 4.0f*r;
            if (i) r *= -1;
            targetsPosX[i] = 0.0f + 0.8f*r;
        } else if (targetsPosY[i] == 0.0f) {
            double r = ((double) rand()/(RAND_MAX)) + 1;
            targetsPosY[i] = 4.0f + 1.0f*r;
            if (i) r *= -1;
            targetsPosX[i] = 0.0f + 0.8f*r;
        } else {
            targetsPosY[i] += targetPosYOffset * elapsedTime;
        }
    }

    // check player contact
    //gastank
    float distance;
    distance = calculateDistance(playerPosX, playerPosY, gasTankPosX, gasTankPosY);
    if (distance < 0.3f) {
        gasTankPosY = -3.0f;
        gasAvailable += 25;
        totalTime = 0;
    }
    // barrier
    for (int i = 0; i < NUM_TARGETS; i++) {
        distance = calculateDistance(playerPosX, playerPosY, targetsPosX[i], targetsPosY[i]);
        if (distance < 0.4f)
            lose = 1;
    }

    // Use fuel
    totalTime += elapsedTime;
    // Increase the size of targe if nothing is done
    if ( (int) totalTime % 4 == 0 && totalTime > 4) {
        gasAvailable -= 0.01;
        totalTime /= 4;
    }
    if (gasAvailable == 0)
        lose = 1;

    score += 0.1;
    int scoreLabel = score;
    if (lose) {
        finalScore = scoreLabel;
        emit updateScoreLabel(QString("You Lose! Distance: %1").arg(finalScore));
    } else {
        emit updateScoreLabel(QString("Distance:%1 \t Fuel:%2").arg(scoreLabel).arg(gasAvailable));
        if (NUM_TARGETS < 30)
            NUM_TARGETS = score/300;
        update();
    }
}

float OpenGLWidget::calculateDistance(float x1, float y1, float x2, float y2)
{
    return sqrt(pow(x1-x2, 2) + pow(y1-y2, 2));
}
