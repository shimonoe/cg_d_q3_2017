#include "openglwidget.h"

OpenGLWidget::OpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    playerPosXOffset = 0;
    playerPosYOffset = 0;
    playerPosX = -0.8f;
    playerPosY = 0;
    playerSize = 0.1;

    targetPosYOffset = 2.0f;
    targetPosX = 0.8f;
    targetPosY = 0;

    shooting = false;
    shootingDirection = 0;
    projectilePosX = 0;
    projectilePosY = 0;

    targetSize = 0.1;
    targetOffset = 0.125f;
    numHits = 0;
    totalTime = 0;
}

OpenGLWidget::~OpenGLWidget()
{
    destroyVBOs();
    destroyShaders();
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, 1);

    qDebug("OpenGL version: %s", glGetString(GL_VERSION));
    qDebug("GLSL %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    createShaders();
    createVBOs();

    connect(&timer, SIGNAL(timeout()), this, SLOT(animate()));
    timer.start(0);

    time.start();
}

void OpenGLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
}

void OpenGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    GLuint locScaling = glGetUniformLocation(shaderProgram, "scaling");
    GLuint locTranslation = glGetUniformLocation(shaderProgram, "translation");

    glUseProgram(shaderProgram);

    glBindVertexArray(vao);

    // Player
    glUniform4f(locTranslation, playerPosX, playerPosY, 0, 0);
    glUniform1f(locScaling, playerSize);
    glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, 0);

    // Target
    glUniform4f(locTranslation, targetPosX, targetPosY, 0, 0);
    glUniform1f(locScaling, targetSize);
    glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, 0);

    // Projectile
    if (shooting)
    {
        glUniform4f(locTranslation, projectilePosX, projectilePosY, 0, 0);
        glUniform1f(locScaling, 0.05);
        glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, 0);
    }
}

void OpenGLWidget::createShaders()
{
    // makeCurrent();
    destroyShaders();

    QFile vs(":/shaders/vshader1.glsl");
    QFile fs(":/shaders/fshader1.glsl");

    vs.open(QFile::ReadOnly | QFile::Text);
    fs.open(QFile::ReadOnly | QFile::Text);

    QTextStream streamVs(&vs), streamFs(&fs);

    QString qtStringVs = streamVs.readAll();
    QString qtStringFs = streamFs.readAll();

    std::string stdStringVs = qtStringVs.toStdString();
    std::string stdStringFs = qtStringFs.toStdString();

    // Create an empty vertex shader handle
    GLuint vertexShader = 0;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // Send the vertex shader source code to GL
    const GLchar *source = stdStringVs.c_str();

    glShaderSource(vertexShader, 1, &source, 0);

    // Compile the vertex shader
    glCompileShader(vertexShader);

    GLint isCompiled = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);
        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);
        qDebug("%s", &infoLog[0]);

        glDeleteShader(vertexShader);
        return;
    }

    // Create an empty fragment shader handle
    GLuint fragmentShader = 0;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Send the fragment shader source code to GL
    // Note that std::string's .c_str is NULL character terminated.
    source = stdStringFs.c_str();
    glShaderSource(fragmentShader, 1, &source, 0);

    // Compile the fragment shader
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);
        qDebug("%s", &infoLog[0]);

        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader);
        return;
    }

    // Vertex and fragment shaders are successfully compiled.
    // Now time to link them together into a program.
    // Get a program object.
    shaderProgram = glCreateProgram();

    // Attach our shaders to our program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    // Link our program
    glLinkProgram(shaderProgram);

    // Note the different functions here: glGetProgram* instead of glGetShader*.
    GLint isLinked = 0;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, (int *)&isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, &infoLog[0]);
        qDebug("%s", &infoLog[0]);

        glDeleteProgram(shaderProgram);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return;
    }

    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    vs.close();
    fs.close();
}

void OpenGLWidget::destroyShaders()
{
    makeCurrent();
    glDeleteProgram(shaderProgram);
}

void OpenGLWidget::createVBOs()
{
    makeCurrent();
    destroyVBOs();

    vertices = std::make_unique<QVector4D[]>(4);
    colors = std::make_unique<QVector4D[]>(4);
    indices = std::make_unique<unsigned int[]>(2 * 3);

    // create  four  vertices  to  define a square
    vertices[0] = QVector4D(-0.5,  -0.5, 0, 1);
    vertices[1] = QVector4D( 0.5,  -0.5, 0, 1);
    vertices[2] = QVector4D( 0.5,   0.5, 0, 1);
    vertices[3] = QVector4D(-0.5,   0.5, 0, 1);
    // create  colors  for  the  vertices
    colors[0] = QVector4D(1, 1, 1, 1);     // red
    colors[1] = QVector4D(1, 1, 1, 1);     // green
    colors[2] = QVector4D(1, 1, 1, 1);     // blue
    colors[3] = QVector4D(1, 1, 1, 1);     // yellow
    // topology  of the  mesh ( square )
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 2;
    indices[4] = 3;
    indices[5] = 0;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(QVector4D), vertices.get(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vboColors);
    glBindBuffer(GL_ARRAY_BUFFER, vboColors);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(QVector4D), colors.get(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &vboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * 3 * sizeof(unsigned int), indices.get(), GL_DYNAMIC_DRAW);
}

void OpenGLWidget::destroyVBOs()
{
    //makeCurrent();

    glDeleteBuffers(1, &vboVertices);
    glDeleteBuffers(1, &vboColors);
    glDeleteBuffers(1, &vboIndices);
    glDeleteVertexArrays(1, &vao);

    vboVertices = 0;
    vboIndices = 0;
    vboColors = 0;
    vao = 0;
}

void OpenGLWidget::animate()
{
    float elapsedTime = time.restart() / 1000.0f;
    // Change player X and Y position
    playerPosX += playerPosXOffset * elapsedTime;
    playerPosY += playerPosYOffset * elapsedTime;

    // Check player bounds
    if (playerPosX < -0.8f)
        playerPosX = -0.8f;
    if (playerPosX > 0.8f)
        playerPosX = 0.8f;
    if (playerPosY < -0.8f)
        playerPosY = -0.8f;
    if (playerPosY > 0.8f)
        playerPosY = 0.8f;

    // Check target bounds
    if (targetPosX < -0.8f)
        targetPosX = -0.8f;
    if (targetPosX > 0.8f)
        targetPosX = 0.8f;
    if (targetPosY < -0.8f)
        targetPosY = -0.8f;
    if (targetPosY > 0.8f)
        targetPosY = 0.8f;

    // Update target
    targetPosX += playerPosX * elapsedTime * numHits/15;
    targetPosY += playerPosY * elapsedTime * numHits/15;
    //targetPosY += targetPosYOffset * elapsedTime*2/3;

    if (targetPosYOffset > 0)
    {
        if (targetPosY > 0.8f)
        {
            targetPosY = 0.8f;
            targetPosYOffset = -targetPosYOffset;
        }
    }
    else if (targetPosYOffset < 0)
    {
        if (targetPosY < -0.8f)
        {
            targetPosY = -0.8f;
            targetPosYOffset = -targetPosYOffset;
        }
    }

    // Update projectile
    if (shooting)
    {
        totalTime = 0;
        // Move projectile based on shooting direction
        switch(shootingDirection) {
        case 0:
            projectilePosY += 3.0f * elapsedTime;
            break;
        case 1:
            projectilePosX += 3.0f * elapsedTime;
            break;
        case 2:
            projectilePosY -= 3.0f * elapsedTime;
            break;
        case 3:
            projectilePosX -= 3.0f * elapsedTime;
            break;
        }

        // Check whether the target was hit
        if (projectilePosX > 0.8f || projectilePosX < -0.8f || projectilePosY > 0.8f || projectilePosY < -0.8f)
        {
            if (std::fabs(projectilePosY - targetPosY) < targetOffset || std::fabs(projectilePosX - targetPosX) < targetOffset)
            {
                numHits++;
                if(targetSize > 0.1)
                    targetSize -= 0.01;
                if(targetOffset > 0.125f)
                    targetOffset -= 0.05f;
                emit updateHitsLabel(QString("Score: %1").arg(numHits));
                shooting = false;
            }
        }

        // Check whether the projectile missed the target
        if (projectilePosX > 1.0f || projectilePosX < -1.0f || projectilePosY > 1.0f || projectilePosY < -1.0f)
        {
            shooting = false;
            targetSize += 0.05;
            if(targetOffset < 0.2f)
                targetOffset += 0.05f;
        }

    }
    totalTime += elapsedTime;
    // Increase the size of targe if nothing is done
    if ( (int) totalTime % 3 == 0 && totalTime > 3) {
        targetSize += 0.1;
        totalTime /= 3;
        if(targetOffset < 0.2f)
            targetOffset += 0.05f;
    }

    // Check whether target hits player
    if (std::fabs(targetPosY - playerPosY) + std::fabs(targetPosX - playerPosX) < targetOffset) {
        playerSize = 0;
        targetSize = 0;
        QOpenGLWidget::setEnabled(false);
        emit updateHitsLabel(QString("GAME OVER :( Score: %1").arg(numHits));
        QOpenGLWidget::setEnabled(true);
        QOpenGLWidget::setEnabled(false);
    }

    update();
}

// Strong focus is required
void OpenGLWidget::keyPressEvent(QKeyEvent *event)
{
    // Player directions update
    if (event->key() == Qt::Key_W)
        playerPosYOffset = 2.0f;

    if (event->key() == Qt::Key_S)
        playerPosYOffset = -2.0f;

    if (event->key() == Qt::Key_A)
        playerPosXOffset = -2.0f;

    if (event->key() == Qt::Key_D)
        playerPosXOffset = 2.0f;

    // Shooting direction update
    if (event->key() == Qt::Key_Up) {
        shootingDirection = 0;
        setShootingParameters();
    }
    if (event->key() == Qt::Key_Right) {
        shootingDirection = 1;
        setShootingParameters();
    }
    if (event->key() == Qt::Key_Down) {
        shootingDirection = 2;
        setShootingParameters();
    }
    if (event->key() == Qt::Key_Left) {
        shootingDirection = 3;
        setShootingParameters();
    }

    if (event->key() == Qt::Key_Escape)
    {
        QApplication::quit();
    }
}


void OpenGLWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_W)
        playerPosYOffset = 0;

    if (event->key() == Qt::Key_S)
        playerPosYOffset = 0;

    if (event->key() == Qt::Key_A)
        playerPosXOffset = 0;

    if (event->key() == Qt::Key_D)
        playerPosXOffset = 0;
}


void OpenGLWidget::setShootingParameters()
{
    if (!shooting) {
        shooting = true;
        projectilePosX = playerPosX;
        projectilePosY = playerPosY;
    }
}
