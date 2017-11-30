#include "model.h"

Model::Model(QOpenGLWidget *_glWidget)
{
    glWidget = _glWidget;
    glWidget->makeCurrent();

    initializeOpenGLFunctions();
}

Model::~Model()
{
    destroyVBOs();
    destroyShaders();
}

void Model::createShaders(QString vertexShaderFile, QString fragmentShaderFile)
{

    destroyShaders();

    QFile vs(vertexShaderFile);
    QFile fs(fragmentShaderFile);

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
    // Send the fragment shader source to GL
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

    shaderProgram = glCreateProgram();
    // Attach our shader to our program
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
    GL_CHECK(glFlush());
}

void Model::createNormals()
{
    normals = std::make_unique<QVector3D[]>(numVertices);

    for (unsigned int i = 0; i < numFaces; ++i)
    {
        QVector3D a = QVector3D(vertices[indices[i * 3 + 0]]);
        QVector3D b = QVector3D(vertices[indices[i * 3 + 1]]);
        QVector3D c = QVector3D(vertices[indices[i * 3 + 2]]);
        QVector3D faceNormal = QVector3D::crossProduct((b - a), (c - b));

        normals[indices[i * 3 + 0]] += faceNormal;
        normals[indices[i * 3 + 1]] += faceNormal;
        normals[indices[i * 3 + 2]] += faceNormal;
    }

    for (unsigned int i = 0; i < numVertices; ++i)
    {
        normals[i].normalize();
    }
}

void Model::destroyVBOs()
{
    GL_CHECK(glDeleteBuffers(1, &vboVertices));
    GL_CHECK(glDeleteBuffers(1, &vboIndices));
    GL_CHECK(glDeleteBuffers(1, &vboNormals));
    GL_CHECK(glDeleteBuffers(1, &vboTexCoords));

    GL_CHECK(glDeleteVertexArrays(1, &vao));

    vboVertices = 0;
    vboIndices = 0;
    vboNormals = 0;
    vboTexCoords = 0;
    vao = 0;
    GL_CHECK(glFlush());
}

void Model::destroyShaders()
{
    GL_CHECK(glDeleteProgram(shaderProgram));
    GL_CHECK(glFlush());
}

void Model::readOFFFile(const QString &fileName)
{
    QFile s(fileName);

    s.open(QFile::ReadOnly | QFile::Text);

    QTextStream stream(&s);

    QStringList line = stream.readLine().split(' ');
    line = stream.readLine().split(' ');
    numVertices = line[0].toFloat();
    numFaces = line[1].toFloat();

    vertices = std::make_unique<QVector4D[]>(numVertices);
    indices = std::make_unique<unsigned int[]>(numFaces * 3);

    if (numVertices > 0)
    {
        float minLim = std::numeric_limits<float>::lowest();
        float maxLim = std::numeric_limits<float>::max();
        QVector4D max(minLim, minLim, minLim, 1.0);
        QVector4D min(maxLim, maxLim, maxLim, 1.0);
        for (unsigned int i = 0; i < numVertices; ++i)
        {
            float x, y, z;

            line = stream.readLine().split(' ');
            x = line[0].toFloat();
            y = line[1].toFloat();
            z = line[2].toFloat();

            max.setX(std::max(max.x(), x));
            max.setY(std::max(max.y(), y));
            max.setZ(std::max(max.z(), z));
            min.setX(std::min(min.x(), x));
            min.setY(std::min(min.y(), y));
            min.setZ(std::min(min.z(), z));
            vertices[i] = QVector4D(x, y, z, 1.0);
        }

        this->midPoint = QVector3D((min + max) * 0.5);
        this->invDiag  = 2.0 / (max - min).length();

        for (unsigned int i = 0; i < numFaces; ++i)
        {
            unsigned int a, b, c;
            line = stream.readLine().split(' ');

            a = line[1].toInt();
            b = line[2].toInt();
            c = line[3].toInt();

            indices[i * 3 + 0] = a;
            indices[i * 3 + 1] = b;
            indices[i * 3 + 2] = c;
        }

        s.close();

        //qDebug("Vertices: %d, Faces: %d", numVertices, numFaces);
        createNormals();
        createTexCoords();

        // point de right shader for model
        QString fshader, modelName;
        modelName = fileName;
        fshader = modelName.replace(":/models/", ":/shaders/f").replace(".off", ".glsl");
        createShaders(":/shaders/vphong.glsl", fshader);

        createVBOs();
    }
}

void Model::drawModel(float posX, float posY, float posZ, float scale, QVector3D rotation)
{

    glBindVertexArray(vao);
    glUseProgram(shaderProgram);

    modelMatrix.setToIdentity();

    // Model rotation
    modelMatrix.translate(0.0, 0.0, 0.0);
    QVector3D quat;
    quat = QVector3D(0, 0, 0);
    for(int i=0; i<3; i++){
        if(rotation[i])
            quat[i] = rotation[i]/rotation[i] * sin(rotation[i]/2);
    }
    QQuaternion quaternion;
    quaternion.setVector(quat);
    modelMatrix.rotate(quaternion.normalized());

    // Translation of model
    modelMatrix.translate(posX, posY, posZ);
    // Scale of model
    modelMatrix.scale(scale, scale, scale);

    GLuint locModel = 0;
    GLuint locNormalMatrix = 0;
    GLuint locShininess = 0;

    locModel = glGetUniformLocation(shaderProgram, "model");
    locNormalMatrix = glGetUniformLocation(shaderProgram, "normalMatrix");
    locShininess = glGetUniformLocation(shaderProgram, "shininess");

    glUniformMatrix4fv(locModel, 1, GL_FALSE, modelMatrix.data());
    glUniformMatrix3fv(locNormalMatrix, 1, GL_FALSE, modelMatrix.normalMatrix().data());
    glUniform1f(locShininess, static_cast<GLfloat>(material.shininess));

    glDrawElements(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_INT, 0);

    if (textureID)
    {
        GLuint locColorTexture = 0;
        GL_CHECK(locColorTexture = glGetUniformLocation(shaderProgram, "colorTexture"));
        GL_CHECK(glUniform1i(locColorTexture, 0));

        GL_CHECK(glActiveTexture(GL_TEXTURE0));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, textureID));
    }
    GL_CHECK(glFlush());
}

void Model::createVBOs()
{
    glWidget->makeCurrent();

    destroyVBOs();

    GL_CHECK(glGenVertexArrays(1, &vao));
    GL_CHECK(glBindVertexArray(vao));

    GL_CHECK(glGenBuffers(1, &vboVertices));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vboVertices));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(QVector4D), vertices.get(), GL_STATIC_DRAW));
    GL_CHECK(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr));
    GL_CHECK(glEnableVertexAttribArray(0));
    vertices.reset();

    GL_CHECK(glGenBuffers(1, &vboNormals));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vboNormals));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(QVector3D), normals.get(), GL_STATIC_DRAW));
    GL_CHECK(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr));
    GL_CHECK(glEnableVertexAttribArray(1));
    normals.reset();

    GL_CHECK(glGenBuffers(1, &vboTexCoords));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(QVector2D), texCoords.get(), GL_STATIC_DRAW));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords));
    GL_CHECK(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr));
    GL_CHECK(glEnableVertexAttribArray(2));
    texCoords.reset();

    GL_CHECK(glGenBuffers(1, &vboIndices));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, numFaces * 3 * sizeof(unsigned int), indices.get(), GL_STATIC_DRAW));
    indices.reset();

    GL_CHECK(glFlush());

}

void Model::createTexCoords()
{
    texCoords = std::make_unique<QVector2D[]>(numVertices);

    // Compute minimum and maximum values
    auto minz = std::numeric_limits<float>::max();
    auto maxz = std::numeric_limits<float>::lowest();

    for (unsigned int i = 0; i < numVertices; ++i)
    {
        minz = std::min(vertices[i].z(), minz);
        maxz = std::max(vertices[i].z(), maxz);
    }

    for (unsigned int i = 0; i < numVertices; ++i)
    {
        auto s = (std::atan2(vertices[i].y(), vertices[i].x()) + M_PI) / (2 * M_PI);
        auto t = 1.0f - (vertices[i].z() - minz) / (maxz - minz);
        texCoords[i] = QVector2D(s, t);
    }
}

void Model::loadTexture(const QString imagepath)
{
    QImage image;
    image.load(imagepath);
    image = image.convertToFormat(QImage::Format_RGBA8888);

    if (textureID)
    {
        GL_CHECK(glDeleteTextures(1, &textureID));
    }

    GL_CHECK(glGenTextures(1, &textureID));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, textureID));
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits()));

    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));

    GL_CHECK(glFlush());
}
