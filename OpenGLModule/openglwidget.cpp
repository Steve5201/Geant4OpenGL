#include "openglwidget.h"
#include <QtMath>

OpenGLWidget::OpenGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    rotateY = 0;
    fov = 45.0f;
    fod = 1000.1f;
    cameraMoveSpeed = 0.1f;
    cameraRotateSpeed = 0.1f;
    drawLineMode = inputEnabled = false;
    moveR = moveL = moveF = moveB = false;
    texture = nullptr;
    currentObject = nullptr;
    AmbientColor = {255, 255, 255};
    AmbientStrength = 0.4;
    AmbientLightColor = {255,255,255};
    AmbientLightDirection = {1,1,1};
    program = new QOpenGLShaderProgram(this);
    projectionMat.perspective(fov,width()/(float)height(),0.1f,fod);
    updateTimer = new QTimer(this);
    connect(updateTimer,&QTimer::timeout,this,&OpenGLWidget::refresh);
    QSurfaceFormat fat = this->format();
    fat.setSamples(16);
    this->setFormat(fat);
    updateTimer->start(15);
}

OpenGLWidget::~OpenGLWidget()
{
    delete updateTimer;
    delete program;
    delete texture;
}

void OpenGLWidget::startInView()
{
    inputEnabled = true;
    setFocus();
    lastPos = {width()/2, height()/2};
    QPoint widgetCenterGlobalPos = this->mapToGlobal(lastPos);
    QCursor::setPos(widgetCenterGlobalPos);
    QApplication::setOverrideCursor(Qt::BlankCursor);
}

void OpenGLWidget::stopInView()
{
    inputEnabled = false;
    QApplication::restoreOverrideCursor();
}

void OpenGLWidget::addObject(OpenGLObject *obj)
{
    addObject(obj, obj->getPosition());
}

void OpenGLWidget::addObject(OpenGLObject *obj, QVector3D pos)
{
    if(obj && !objects.contains(obj))
    {
        obj->setPosition(pos);
        objects.push_back(obj);
        currentObject = obj;
        emit selectObjectChanged(obj);
        this->update();
    }
}

bool OpenGLWidget::addObjectAtScreenPos(OpenGLObject *obj, QPoint pos)
{
    if(rect().contains(pos))
    {
        qreal hw = width() / 2;
        qreal hh = height() / 2;
        qreal fx = (pos.x() - hw) / (hw);
        qreal fy = (hh - pos.y()) / (hh);
        qreal depth = 0.96;
        QVector3D screenPos(fx, fy, depth);
        QVector4D clipCoords(screenPos, 1.0f);
        QMatrix4x4 invProjection = projectionMat.inverted();
        QVector4D eyeCoords = invProjection * clipCoords;
        eyeCoords /= eyeCoords.w();
        QMatrix4x4 invView = camera.getViewMat().inverted();
        QVector4D worldCoords = invView * eyeCoords;
        worldCoords /= worldCoords.w();
        addObject(obj,worldCoords.toVector3D());
        return true;
    }
    return false;
}

void OpenGLWidget::removeAllObject()
{
    while(!objects.empty())
    {
        emit objectRemoved(objects[0]->getOjectName());
        removeObject(0);
    }
    emit sourceRemoved();
}

void OpenGLWidget::removeObject(const int objidx)
{
    if(objidx >= objects.size() || objidx < 0)
    {
        return;
    }
    if(objects[objidx] == currentObject)
    {
        currentObject = nullptr;
        emit selectObjectChanged(nullptr);
    }
    if(objects[objidx]->getTypeID() ==5)
    {
        emit sourceRemoved();
    }
    delete objects[objidx];
    objects.removeAt(objidx);
    this->update();
}

void OpenGLWidget::removeObject(OpenGLObject *obj)
{
    int idx = objects.indexOf(obj);
    if(idx == -1)
    {
        return;
    }
    if(objects[idx] == currentObject)
    {
        currentObject = nullptr;
        emit selectObjectChanged(nullptr);
    }
    if(objects[idx]->getTypeID() ==5)
    {
        emit sourceRemoved();
    }
    delete objects[idx];
    objects.removeAt(idx);
    this->update();
}

OpenGLObject *OpenGLWidget::getObjectByName(QString name) const
{
    OpenGLObject *rt = nullptr;
    foreach (auto obj, objects)
    {
        if(obj->getOjectName() == name)
        {
            rt = obj;
        }
    }
    return rt;
}

QVector<OpenGLObject*> OpenGLWidget::getObjects() const
{
    return objects;
}

QVector<OpenGLObject*> &OpenGLWidget::getObjectsRef()
{
    return objects;
}

float OpenGLWidget::getFod() const
{
    return fod;
}

void OpenGLWidget::setFod(float newFod)
{
    fod = newFod;
    QMatrix4x4 prj;
    prj.perspective(fov, width()/(float)height(), 0.1f, fod);
    projectionMat = prj;
}

float OpenGLWidget::getFov() const
{
    return fov;
}

void OpenGLWidget::setFov(float newFov)
{
    fov = newFov;
    QMatrix4x4 prj;
    prj.perspective(fov, width()/(float)height(), 0.1f, fod);
    projectionMat = prj;
}

OpenGLCamera OpenGLWidget::getCamera() const
{
    return camera;
}

void OpenGLWidget::setCamera(const OpenGLCamera &newCamera)
{
    camera = newCamera;
}

float OpenGLWidget::getCameraRotateSpeed() const
{
    return cameraRotateSpeed;
}

void OpenGLWidget::setCameraRotateSpeed(float newCameraRotateSpeed)
{
    cameraRotateSpeed = newCameraRotateSpeed;
}

float OpenGLWidget::getCameraMoveSpeed() const
{
    return cameraMoveSpeed;
}

void OpenGLWidget::setCameraMoveSpeed(float newCameraMoveSpeed)
{
    cameraMoveSpeed = newCameraMoveSpeed;
}

QMatrix4x4 OpenGLWidget::getProjectionMat() const
{
    return projectionMat;
}

void OpenGLWidget::setProjectionMat(const QMatrix4x4 &newProjectionMat)
{
    projectionMat = newProjectionMat;
}

OpenGLObject *OpenGLWidget::getCurrentObject() const
{
    return currentObject;
}

void OpenGLWidget::setCurrentObject(OpenGLObject *newCurrentObject)
{
    currentObject = newCurrentObject;
    emit selectObjectChanged(newCurrentObject);
}

QColor OpenGLWidget::getAmbientColor() const
{
    return AmbientColor;
}

void OpenGLWidget::setAmbientColor(const QColor &newAmbientColor)
{
    AmbientColor = newAmbientColor;
}

GLfloat OpenGLWidget::getAmbientStrength() const
{
    return AmbientStrength;
}

void OpenGLWidget::setAmbientStrength(GLfloat newAmbientStrength)
{
    AmbientStrength = newAmbientStrength;
}

QVector3D OpenGLWidget::getAmbientLightDirection() const
{
    return AmbientLightDirection;
}

void OpenGLWidget::setAmbientLightDirection(const QVector3D &newAmbientLightDirection)
{
    AmbientLightDirection = newAmbientLightDirection;
}

bool OpenGLWidget::getDrawLineMode() const
{
    return drawLineMode;
}

void OpenGLWidget::setDrawLineMode(bool newDrawLineMode)
{
    drawLineMode = newDrawLineMode;
}

QColor OpenGLWidget::getAmbientLightColor() const
{
    return AmbientLightColor;
}

void OpenGLWidget::setAmbientLightColor(const QColor &newAmbientLightColor)
{
    AmbientLightColor = newAmbientLightColor;
}

QMap<std::string, QMap<int, QMap<int, QVector<QVector3D>>>> OpenGLWidget::getUserLines() const
{
    return userLines;
}

void OpenGLWidget::setUserLines(const QMap<std::string, QMap<int, QMap<int, QVector<QVector3D>>>> &newUserLines)
{
    userLines = newUserLines;
}

void OpenGLWidget::setDrawUserLines(bool newDrawUserLines)
{
    drawUserLines = newDrawUserLines;
}

void OpenGLWidget::initializeGL()
{
    QOpenGLWidget::initializeGL();
    initializeOpenGLFunctions();
    glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    configProgram();
}

void OpenGLWidget::paintGL()
{
    if(drawLineMode)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    QOpenGLWidget::paintGL();
    enableSelectMode(false);
    updateEnvironment();
    updateProgramView(camera.getViewMat());
    updateProgramprojection(projectionMat);
    if(programValid)
    {
        if(objects.size()>0)
        {
            foreach (auto obj, objects)
            {
                updateProgramModel(obj->getTransformationMat());
                updateUserColor(obj->getUseUserColor(), obj->getUserColor());
                drawObject(*obj);
            }
        }
    }
    drawCurrentObjectFrame();
    if(drawUserLines)
    {
        updateProgramModel(QMatrix4x4());
        drawUserLinesOnScene();
    }
}

void OpenGLWidget::resizeGL(int w, int h)
{
    QOpenGLWidget::resizeGL(w, h);
    QMatrix4x4 prj;
    prj.perspective(fov,width()/(float)height(),0.1f,fod);
    projectionMat = prj;
}

void OpenGLWidget::wheelEvent(QWheelEvent *event)
{
    QOpenGLWidget::wheelEvent(event);
}

void OpenGLWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QOpenGLWidget::mouseDoubleClickEvent(event);
    startInView();
}

void OpenGLWidget::keyPressEvent(QKeyEvent *event)
{
    QOpenGLWidget::keyPressEvent(event);
    switch (event->key()) {
    case Qt::Key_W:
        moveF = true;
        break;
    case Qt::Key_S:
        moveB = true;
        break;
    case Qt::Key_A:
        moveL = true;
        break;
    case Qt::Key_D:
        moveR = true;
        break;
    case Qt::Key_Escape:
        stopInView();
        break;
    default:
        break;
    }
}

void OpenGLWidget::keyReleaseEvent(QKeyEvent *event)
{
    QOpenGLWidget::keyReleaseEvent(event);
    switch (event->key()) {
    case Qt::Key_W:
        moveF = false;
        break;
    case Qt::Key_S:
        moveB = false;
        break;
    case Qt::Key_A:
        moveL = false;
        break;
    case Qt::Key_D:
        moveR = false;
        break;
    default:
        break;
    }
}

void OpenGLWidget::mousePressEvent(QMouseEvent *event)
{
    QOpenGLWidget::mousePressEvent(event);
    if(event->button() == Qt::LeftButton)
    {
        OpenGLObject *obj = getSelectObject(event->pos());
        currentObject = obj;
        emit selectObjectChanged(obj);
    }
    if(event->button() == Qt::RightButton)
    {
        OpenGLObject *obj = getSelectObject(event->pos());
        if(obj)
        {
            emit objectRemoved(obj->getOjectName());
            removeObject(obj);
        }
    }
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QOpenGLWidget::mouseReleaseEvent(event);
}

void OpenGLWidget::refresh()
{
    if(inputEnabled)
    {
        QPoint mouseGlobalPos = QCursor::pos();
        QPoint widgetMousePos = this->mapFromGlobal(mouseGlobalPos);
        int dx = widgetMousePos.x() - lastPos.x();
        int dy = widgetMousePos.y() - lastPos.y();
        QCursor::setPos(this->mapToGlobal(lastPos));
        if(abs(dx) > abs(dy))
        {
            camera.rotate(dx * cameraRotateSpeed,{0,1,0});
        }
        else if(abs(dx) < abs(dy))
        {
            if(rotateY + dy * cameraRotateSpeed > -90 && rotateY + dy * cameraRotateSpeed < 90)
            {
                rotateY += dy * cameraRotateSpeed;
                camera.rotate(dy * cameraRotateSpeed,camera.getCameraRight());
            }
        }
        if(moveF)
        {
            camera.moveFront(cameraMoveSpeed);
        }
        if(moveB)
        {
            camera.moveBack(cameraMoveSpeed);
        }
        if(moveR)
        {
            camera.moveRight(cameraMoveSpeed);
        }
        if(moveL)
        {
            camera.moveLeft(cameraMoveSpeed);
        }
    }
    this->update();
}

void OpenGLWidget::drawUserLinesOnScene()
{
    makeCurrent();
    if(userLines.empty())
    {
        return;
    }
    foreach (auto lineType, userLines.keys())
    {
        QColor col;
        if(lineType == "gamma")
        {
            col = Qt::green;
        }
        else if(lineType == "e-")
        {
            col = Qt::red;
        }
        else
        {
            col = Qt::gray;
        }
        updateUserColor(true, col, true);
        int esize = userLines[lineType].size();
        int estep = esize / 500;
        foreach (auto ets, userLines[lineType].keys())
        {
            if(estep > 0)
            {
                estep--;
                continue;
            }
            estep = esize / 500;

            QVector<QVector<QVector3D>> etsVector;
            uint size = 0;
            foreach (auto line, userLines[lineType][ets])
            {
                etsVector.append(line);
                size += line.size();
            }
            if(size < 1)
            {
                continue;
            }

            unsigned int VBO, EBO;
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);
            float *vertices = new float[size * 3];
            uint start = 0;
            for (int i = 0; i < etsVector.size(); ++i)
            {
                for (int j = 0; j < etsVector[i].size(); ++j)
                {
                    vertices[start + j * 3] = etsVector[i][j].x();
                    vertices[start + j * 3 + 1] = etsVector[i][j].y();
                    vertices[start + j * 3 + 2] = etsVector[i][j].z();
                }
                start += etsVector[i].size() * 3;
            }
            size_t numIndices = size * 2 - etsVector.size() * 2;
            if(numIndices < 1)
            {
                delete[] vertices;
                continue;
            }
            unsigned int *indices = new unsigned int[numIndices];
            start = 0;
            uint vstart = 0;
            for (int i = 0; i < etsVector.size(); ++i)
            {
                for (int j = 0; j < etsVector[i].size() - 1; ++j)
                {
                    indices[start + j * 2] = vstart + j;
                    indices[start + j * 2 + 1] = vstart + j + 1;
                }
                start += etsVector[i].size() * 2 - 2;
                vstart += etsVector[i].size();
            }

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, size * 3 * sizeof(float), vertices, GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), indices, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glLineWidth(1.0);
            glDrawElements(GL_LINES, numIndices, GL_UNSIGNED_INT, 0);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
            delete[] vertices;
            delete[] indices;
        }
    }
}

void OpenGLWidget::drawCurrentObjectFrame()
{
    makeCurrent();
    if(currentObject)
    {
        updateProgramModel(currentObject->getTransformationMat());
        QVector<QVector3D> rect3D = currentObject->getRect3D();
        float lenthx = qAbs(rect3D[0].x() - rect3D[7].x());
        float lenthy = qAbs(rect3D[0].x() - rect3D[6].x());
        float lenthz = qAbs(rect3D[0].x() - rect3D[5].x());
        lenthx = lenthx/3 + lenthy/3 + lenthz/3;
        float vertices [36] =
            {
                rect3D[0].x(),rect3D[0].y(),rect3D[0].z(),
                rect3D[1].x(),rect3D[1].y(),rect3D[1].z(),
                rect3D[2].x(),rect3D[2].y(),rect3D[2].z(),
                rect3D[3].x(),rect3D[3].y(),rect3D[3].z(),
                rect3D[4].x(),rect3D[4].y(),rect3D[4].z(),
                rect3D[5].x(),rect3D[5].y(),rect3D[5].z(),
                rect3D[6].x(),rect3D[6].y(),rect3D[6].z(),
                rect3D[7].x(),rect3D[7].y(),rect3D[7].z(),
                0,0,0,
                lenthx,0,0,
                0,lenthx,0,
                0,0,lenthx
            };
        unsigned int indices[30] = {0,1,3,2, 3,2,6,7, 6,7,5,4, 5,4,0,1, 0,4,6,2, 1,3,7,5, 8,9, 8,10, 8,11};
        unsigned int VBO;
        unsigned int EBO;
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        updateUserColor(true, QColor(255,75,75,175), true);
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (void*)16);
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (void*)32);
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (void*)48);
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (void*)64);
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (void*)80);
        glLineWidth(3.0);
        updateUserColor(true, QColor(255,75,75,175), true);
        glDrawElements(GL_LINE_LOOP, 2, GL_UNSIGNED_INT, (void*)96);
        updateUserColor(true, QColor(75,255,75,175), true);
        glDrawElements(GL_LINE_LOOP, 2, GL_UNSIGNED_INT, (void*)104);
        updateUserColor(true, QColor(75,75,255,175), true);
        glDrawElements(GL_LINE_LOOP, 2, GL_UNSIGNED_INT, (void*)112);
        glLineWidth(1.0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
}

void OpenGLWidget::configMesh(OpenGLMesh &mesh, QOpenGLFramebufferObject *fbo)
{
    makeCurrent();
    if(fbo)
    {
        fbo->bind();
    }
    QOpenGLBuffer *vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    QOpenGLBuffer *ebo = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    QOpenGLVertexArrayObject *vao = new QOpenGLVertexArrayObject;
    if(!vbo->create() || !ebo->create() || !vao->create())
    {
        delete vbo;
        delete ebo;
        delete vao;
        return;
    }
    QVector<quint32> &indices = mesh.getIndicesRef();
    QVector<OpenGLMesh::Vertex> &vertexs = mesh.getVerticesRef();
    vao->bind();
    ebo->bind();
    vbo->bind();
    ebo->allocate(indices.data(),indices.size()*(int)sizeof(quint32));
    vbo->allocate(vertexs.data(),vertexs.size()*(int)sizeof(OpenGLMesh::Vertex));
    updateProgramVertexs();
    vao->release();
    ebo->release();
    vbo->release();
    mesh.setVAO(vao);
    mesh.setEBO(ebo);
    mesh.setVBO(vbo);
    QImage image;
    QString strPath;
    QVector<OpenGLMesh::Texture> &textures = mesh.getTexturesRef();
    for (int i = 0; i < textures.size(); ++i)
    {
        OpenGLMesh::Texture &tex = textures[i];
        strPath = mesh.getFileName().mid(0, mesh.getFileName().lastIndexOf('/') + 1) + tex.path;
        if(loadedTextures.contains(strPath))
        {
            tex.ptex = loadedTextures.value(strPath);
        }
        else
        {
            image.load(strPath);
            if(!image.isNull())
            {
                texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
                if(texture && texture->create())
                {
                    texture->bind();
                    texture->setData(image);
                    texture->allocateStorage();
                    texture->release();
                    tex.ptex = texture;
                    loadedTextures.insert(strPath,texture);
                    texture = nullptr;
                }
            }
        }
    }
}

void OpenGLWidget::drawMesh(OpenGLMesh &mesh, QOpenGLFramebufferObject *fbo)
{
    makeCurrent();
    if(fbo)
    {
        fbo->bind();
    }
    if(!mesh.getConfiged())
    {
        configMesh(mesh);
    }
    mesh.getVAO()->bind();
    QVector<OpenGLMesh::Texture> textures = mesh.getTextures();
    QString sampler;
    int idxDiff = 0, idxSpec = 0;
    for (int i = 0; i < textures.size() && i < 15; ++i)
    {
        glActiveTexture(GL_TEXTURE0 + 0);
        OpenGLMesh::Texture &tex = textures[i];
        tex.ptex->bind();
        if(tex.type == OpenGLMesh::DIFFUSE && idxDiff < 1)
        {
            sampler = "DIFFUSE" + QString::asprintf("%02d",idxDiff);
            idxDiff++;
        }
        else if(tex.type == OpenGLMesh::SPECULAR && idxSpec < 1)
        {
            sampler = "SPECULAR" + QString::asprintf("%02d",idxSpec);
            idxSpec++;
        }
        int pos = program->uniformLocation(sampler);
        if(pos > -1)
        {
            program->setUniformValue(pos,i);
        }
        sampler = "";
    }
    glActiveTexture(GL_TEXTURE0);
    glDrawElements(GL_TRIANGLES,mesh.getIndicesSize(),GL_UNSIGNED_INT,(void*)0);
    mesh.getVAO()->release();
    for (int i = 0; i < textures.size() && i < 15; ++i)
    {
        textures[i].ptex->release();
    }
}

void OpenGLWidget::configObject(OpenGLObject &obj, QOpenGLFramebufferObject *fbo)
{
    for (int i = 0; i < obj.getMeshsRef().size(); ++i)
    {
        OpenGLMesh &mesh = obj.getMeshsRef()[i];
        configMesh(mesh, fbo);
    }
}

void OpenGLWidget::drawObject(OpenGLObject &obj, QOpenGLFramebufferObject *fbo)
{
    for (int i = 0; i < obj.getMeshsRef().size(); ++i)
    {
        OpenGLMesh &mesh = obj.getMeshsRef()[i];
        drawMesh(mesh, fbo);
    }
}

void OpenGLWidget::configProgram()
{
    makeCurrent();
    programValid = false;
    if(!program->isLinked())
    {
        if(!program->addShaderFromSourceFile(QOpenGLShader::Vertex,":/shader/OpenGLShader/Vertex.shd"))
        {
            log = "顶点着色器编译失败";
        }
        if(!program->addShaderFromSourceFile(QOpenGLShader::Fragment,":/shader/OpenGLShader/Fragment.shd"))
        {
            log = "片断着色器编译失败";
        }
        if(!program->link())
        {
            log = "着色器程序链接失败";
        }
        program->removeAllShaders();
    }
    if(!program->bind())
    {
        log = "着色器绑定失败";
    }
    programValid = true;
}

void OpenGLWidget::updateEnvironment()
{
    makeCurrent();
    if(programValid)
    {
        int idx = program->uniformLocation("AmbientColor");
        if(idx != -1)
        {
            program->setUniformValue(idx, AmbientColor);
        }
        idx = program->uniformLocation("AmbientStrength");
        if(idx != -1)
        {
            program->setUniformValue(idx, AmbientStrength);
        }
        idx = program->uniformLocation("AmbientLightColor");
        if(idx != -1)
        {
            program->setUniformValue(idx, AmbientLightColor);
        }
        idx = program->uniformLocation("AmbientLightDirection");
        if(idx != -1)
        {
            program->setUniformValue(idx, AmbientLightDirection);
        }
    }
}

void OpenGLWidget::updateProgramVertexs()
{
    makeCurrent();
    if(programValid)
    {
        int idx = program->attributeLocation("aPos");
        if(idx == -1)
        {
            log = "aPos设置失败";
        }
        else
        {
            program->setAttributeBuffer(idx,GL_FLOAT,0,3,32);
            program->enableAttributeArray(idx);
        }
        idx = program->attributeLocation("aNormal");
        if(idx == -1)
        {
            log = "aNormal设置失败";
        }
        else
        {
            program->setAttributeBuffer(idx,GL_FLOAT,12,3,32);
            program->enableAttributeArray(idx);
        }
        idx = program->attributeLocation("aTexCoords");
        if(idx == -1)
        {
            log = "aTexCoords设置失败";
        }
        else
        {
            program->setAttributeBuffer(idx,GL_FLOAT,24,2,32);
            program->enableAttributeArray(idx);
        }
    }
}

void OpenGLWidget::updateUserColor(bool use, QColor color, bool drawfra)
{
    makeCurrent();
    if(programValid)
    {
        int idx = program->uniformLocation("UseUserColor");
        if(idx != -1)
        {
            program->setUniformValue(idx, use);
        }
        idx = program->uniformLocation("UserColor");
        if(idx != -1)
        {
            program->setUniformValue(idx, color);
        }
        idx = program->uniformLocation("DrawFrame");
        if(idx != -1)
        {
            program->setUniformValue(idx, drawfra);
        }
    }
}

void OpenGLWidget::updateProgramModel(const QMatrix4x4 &mat)
{
    makeCurrent();
    if(programValid)
    {
        int idx = program->uniformLocation("model");
        if(idx != -1)
        {
            program->setUniformValue(idx,mat);
        }
        idx = program->uniformLocation("normalMatrix");
        if(idx != -1)
        {
            program->setUniformValue(idx,mat.inverted().transposed());
        }
    }
}

void OpenGLWidget::updateProgramView(const QMatrix4x4 &mat)
{
    makeCurrent();
    if(programValid)
    {
        int idx = program->uniformLocation("view");
        if(idx != -1)
        {
            program->setUniformValue(idx,mat);
        }
    }
}

void OpenGLWidget::updateProgramprojection(const QMatrix4x4 &mat)
{
    makeCurrent();
    if(programValid)
    {
        int idx = program->uniformLocation("projection");
        if(idx != -1)
        {
            program->setUniformValue(idx,mat);
        }
    }
}

void OpenGLWidget::enableSelectMode(bool select)
{
    makeCurrent();
    int idx = program->uniformLocation("Select");
    if(idx != -1)
    {
        program->setUniformValue(idx,select);
    }
}

void OpenGLWidget::setSelectId(const QVector4D &id)
{
    makeCurrent();
    int idx = program->uniformLocation("SelectId");
    if(idx != -1)
    {
        program->setUniformValue(idx,id);
    }
}

OpenGLObject *OpenGLWidget::getSelectObject(QPointF pos)
{
    OpenGLObject *result = nullptr;
    QHash<quint32,OpenGLObject*> mapper;
    QOpenGLFramebufferObject *fbo = new QOpenGLFramebufferObject(this->grabFramebuffer().size(),QOpenGLFramebufferObject::Depth);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    enableSelectMode(true);
    if(objects.size()>0)
    {
        for (int i = 0; i < objects.size(); ++i)
        {
            updateProgramModel(objects[i]->getTransformationMat());
            QColor id(i);
            setSelectId(QVector4D(id.redF(),id.greenF(),id.blueF(),id.alphaF()));
            mapper.insert(i,objects[i]);
            drawObject(*objects[i],fbo);
        }
    }
    QImage img = fbo->toImage().scaled(this->width(),this->height());
    quint32 key = img.pixel(pos.toPoint());
    key &= 16777215;
    result = mapper.value(key);
    glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
    return result;
}
