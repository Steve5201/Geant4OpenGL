#include "openglobject.h"
#include <string>
#include <QFile>
#include <QDir>
#include <QFileInfo>

OpenGLObject::OpenGLObject()
{
    valid = useUserColor = false;
    userColor = {126, 132, 247, 255};
    scaleFactor = 1.0;
    transMat = new QMatrix4x4;
    rotMat = new QMatrix4x4;
    position = {0.0,0.0,0.0};
    upDirection = QVector3D(0.0f, 1.0f, 0.0f);
    frontDirection = QVector3D(0.0f, 0.0f, 1.0f);
    rightDirection = QVector3D(1.0f, 0.0f, 0.0f);
}

OpenGLObject::OpenGLObject(QString file)
{
    loadModel(file);
}

OpenGLObject::OpenGLObject(const OpenGLObject &other)
{
    valid = other.valid;
    userColor = other.userColor;
    useUserColor = other.useUserColor;
    fileName = other.fileName;
    position = other.position;
    transMat = new QMatrix4x4;
    *transMat = *other.transMat;
    rotMat = new QMatrix4x4;
    *rotMat = *other.rotMat;
    meshs = other.meshs;
    upDirection = other.upDirection;
    frontDirection = other.frontDirection;
    rightDirection = other.rightDirection;
}

OpenGLObject::~OpenGLObject()
{
    delete rotMat;
    delete transMat;
}

OpenGLObject& OpenGLObject::operator=(const OpenGLObject &other)
{
    valid = other.valid;
    userColor = other.userColor;
    useUserColor = other.useUserColor;
    fileName = other.fileName;
    position = other.position;
    *transMat = *other.transMat;
    *rotMat = *other.rotMat;
    meshs = other.meshs;
    upDirection = other.upDirection;
    frontDirection = other.frontDirection;
    rightDirection = other.rightDirection;
    return *this;
}

int OpenGLObject::getTypeID()
{
    return 0;
}

void OpenGLObject::reset()
{
    scaleFactor = 1.0;
    if(transMat)
    {
        delete transMat;
        transMat = new QMatrix4x4;
    }
    if(rotMat)
    {
        delete rotMat;
        rotMat = new QMatrix4x4;
    }
    userColor = {126, 132, 247, 255};
    upDirection = QVector3D(0.0f, 1.0f, 0.0f);
    frontDirection = QVector3D(0.0f, 0.0f, 1.0f);
    rightDirection = QVector3D(1.0f, 0.0f, 0.0f);
}

bool OpenGLObject::isValid() const
{
    return valid;
}

bool OpenGLObject::getConfiged() const
{
    bool obj_configed = true;
    foreach (auto v, meshs)
    {
        if(v.getConfiged() == false)
        {
            obj_configed = false;
            break;
        }
    }
    return obj_configed;
}

void OpenGLObject::loadModel(QString file)
{
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(file.toStdString(), aiProcess_Triangulate | aiProcess_FlipUVs);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        valid = false;
        return;
    }
    fileName = file;
    processNode(scene->mRootNode, scene);
    valid = true;
}

bool OpenGLObject::exportModel(const QString file)
{
    Assimp::Exporter exporter;
    std::string str = file.toStdString();

    // 获取导出文件的基本信息
    QFileInfo fileInfo(file);
    QString baseName = fileInfo.completeBaseName();  // 获取文件名 (不带扩展名)
    QString outputDir = fileInfo.absolutePath() + "/" + baseName;  // 新文件夹路径
    QDir().mkdir(outputDir);  // 创建文件夹


    // 创建 aiScene
    aiScene scene;
    scene.mRootNode = new aiNode();
    scene.mMeshes = new aiMesh*[meshs.size()];
    scene.mNumMeshes = meshs.size();
    scene.mMaterials = new aiMaterial*[meshs.size()];
    scene.mNumMaterials = meshs.size();
    scene.mRootNode->mNumMeshes = meshs.size();
    scene.mRootNode->mMeshes = new unsigned int[scene.mNumMeshes];

    for (unsigned int i = 0; i < meshs.size(); i++)
    {
        OpenGLMesh &mesh = meshs[i];

        // 创建 aiMesh
        aiMesh *ai_mesh = new aiMesh();
        ai_mesh->mNumVertices = mesh.getVerticesSize();
        ai_mesh->mVertices = new aiVector3D[ai_mesh->mNumVertices];
        ai_mesh->mNormals = new aiVector3D[ai_mesh->mNumVertices];
        ai_mesh->mTextureCoords[0] = new aiVector3D[ai_mesh->mNumVertices];

        // 填充顶点数据
        for (unsigned int j = 0; j < ai_mesh->mNumVertices; j++)
        {
            const OpenGLMesh::Vertex &vertex = mesh.getVerticesRef()[j];
            ai_mesh->mVertices[j] = aiVector3D(vertex.position.x(), vertex.position.y(), vertex.position.z());
            ai_mesh->mNormals[j] = aiVector3D(vertex.normal.x(), vertex.normal.y(), vertex.normal.z());
            ai_mesh->mTextureCoords[0][j] = aiVector3D(vertex.texCoords.x(), vertex.texCoords.y(), 0.0f); // 假设使用 2D 纹理坐标
        }

        // 填充索引数据
        ai_mesh->mNumFaces = mesh.getIndicesSize() / 3;
        ai_mesh->mFaces = new aiFace[ai_mesh->mNumFaces];
        for (unsigned int j = 0; j < ai_mesh->mNumFaces; j++)
        {
            aiFace &face = ai_mesh->mFaces[j];
            face.mNumIndices = 3;
            face.mIndices = new unsigned int[3];
            for (unsigned int k = 0; k < 3; k++)
            {
                face.mIndices[k] = mesh.getIndicesRef()[j * 3 + k];
            }
        }

        // 处理材质
        aiMaterial *material = new aiMaterial();
        foreach(const OpenGLMesh::Texture &texture, mesh.getTexturesRef())
        {
            aiString texPath(texture.path.toStdString());
            QString originalTexPath = mesh.getFileName().mid(0, mesh.getFileName().lastIndexOf('/') + 1) + texture.path;  // 原始路径

            // 复制纹理文件到新的文件夹
            QString newTexPath = outputDir + "/" + QFileInfo(originalTexPath).fileName();
            if (QFile::exists(originalTexPath))
            {
                QFile::copy(originalTexPath, newTexPath);
            }
            else
            {
                qDebug() << "纹理文件不存在: " << originalTexPath;
            }

            // 将新的纹理路径设置到 Assimp 的材质系统中
            QFileInfo newfileinfo(newTexPath);
            aiString aiTexPath((baseName + "/" + newfileinfo.fileName()).toStdString());
            if (texture.type == OpenGLMesh::DIFFUSE)
            {
                material->AddProperty(&aiTexPath, AI_MATKEY_TEXTURE_DIFFUSE(0));
            }
            else if (texture.type == OpenGLMesh::SPECULAR)
            {
                material->AddProperty(&aiTexPath, AI_MATKEY_TEXTURE_SPECULAR(0));
            }
        }

        ai_mesh->mMaterialIndex = i;
        scene.mMeshes[i] = ai_mesh;
        scene.mMaterials[i] = material;
        scene.mRootNode->mMeshes[i] = i;
    }

    // 导出模型
    if (exporter.Export(&scene, "obj", str + ".obj", aiProcess_FlipUVs) == AI_SUCCESS)
    {
        return true;
    }
    return false;
}

QString OpenGLObject::getFileName() const
{
    return fileName;
}

QMatrix4x4 OpenGLObject::getTransformationMat() const
{
    return (*transMat) * (*rotMat);
}

QVector<OpenGLMesh> OpenGLObject::getMeshs() const
{
    return meshs;
}

QVector<OpenGLMesh> &OpenGLObject::getMeshsRef()
{
    return meshs;
}

void OpenGLObject::setScaleFactor(float factor)
{
    if(transMat)
    {
        if(factor <= 0.0)
        {
            factor = 0.0000001;
        }
        scaleFactor = factor;
        delete transMat;
        transMat = new QMatrix4x4;
        transMat->translate(position);
        QMatrix4x4 sc;
        sc.scale(scaleFactor);
        *transMat *= sc;
    }
}

float OpenGLObject::getScaleFactor() const
{
    return scaleFactor;
}

QVector3D OpenGLObject::getPosition() const
{
    return position;
}

void OpenGLObject::setPosition(const QVector3D &newPosition)
{
    if(transMat)
    {
        position = newPosition;
        delete transMat;
        transMat = new QMatrix4x4;
        transMat->translate(position);
        QMatrix4x4 sc;
        sc.scale(scaleFactor);
        *transMat *= sc;
    }
}

QVector3D OpenGLObject::getRotation() const
{
    return rotation;
}

void OpenGLObject::setRotation(const QVector3D &newRotation)
{
    if(rotMat)
    {
        delete rotMat;
        rotMat = new QMatrix4x4;
        upDirection = QVector3D(0.0f, 1.0f, 0.0f);
        frontDirection = QVector3D(0.0f, 0.0f, 1.0f);
        rightDirection = QVector3D(1.0f, 0.0f, 0.0f);
        rotation = newRotation;
        rotate(rotation.x(), {1.0,0.0,0.0});
        rotate(rotation.y(), {0.0,1.0,0.0});
        rotate(rotation.z(), {0.0,0.0,1.0});
    }
}

void OpenGLObject::rotate(float angle, const QVector3D &vector)
{
    if(rotMat)
    {
        QMatrix4x4 rot;
        rot.rotate(angle, vector);
        *rotMat *= rot;
        upDirection = rot * upDirection;
        frontDirection = rot * frontDirection;
        rightDirection = rot * rightDirection;
    }
}

QVector<QVector3D> OpenGLObject::getRect3D() const
{
    QVector<QVector3D> result;
    if(meshs.empty())
    {
        result.resize(8);
        return result;
    }
    QVector<QVector3D> vertices;
    foreach (auto v, meshs)
    {
        vertices.append(v.getRect3D());
    }
    qreal minx = vertices[0].x();
    qreal miny = vertices[0].y();
    qreal minz = vertices[0].z();
    qreal maxx = minx;
    qreal maxy = miny;
    qreal maxz = minz;
    foreach (auto vet, vertices)
    {
        qreal x = vet.x();
        qreal y = vet.y();
        qreal z = vet.z();
        if(x > maxx)
        {
            maxx = x;
        }
        if(x < minx)
        {
            minx = x;
        }
        if(y > maxy)
        {
            maxy = y;
        }
        if(y < miny)
        {
            miny = y;
        }
        if(z > maxz)
        {
            maxz = z;
        }
        if(z < minz)
        {
            minz = z;
        }
    }
    result.append(QVector3D(minx,miny,minz));
    result.append(QVector3D(minx,maxy,minz));
    result.append(QVector3D(minx,miny,maxz));
    result.append(QVector3D(minx,maxy,maxz));
    result.append(QVector3D(maxx,miny,minz));
    result.append(QVector3D(maxx,maxy,minz));
    result.append(QVector3D(maxx,miny,maxz));
    result.append(QVector3D(maxx,maxy,maxz));
    return result;
}

QColor OpenGLObject::getUserColor() const
{
    return userColor;
}

void OpenGLObject::setUserColor(const QColor &newUserColor)
{
    userColor = newUserColor;
}

bool OpenGLObject::getUseUserColor() const
{
    return useUserColor;
}

void OpenGLObject::setUseUserColor(bool newUseUserColor)
{
    useUserColor = newUseUserColor;
}

QVector3D OpenGLObject::getUpDirection() const
{
    return upDirection;
}

QVector3D OpenGLObject::getFrontDirection() const
{
    return frontDirection;
}

QVector3D OpenGLObject::getRightDirection() const
{
    return rightDirection;
}

QString OpenGLObject::getOjectName() const
{
    return ojectName;
}

void OpenGLObject::setObjectName(const QString &newOjectName)
{
    ojectName = newOjectName;
}

QString OpenGLObject::getCustomerName1() const
{
    return customerName1;
}

void OpenGLObject::setCustomerName1(const QString &newCustomerName1)
{
    customerName1 = newCustomerName1;
}

QString OpenGLObject::getCustomerName2() const
{
    return customerName2;
}

void OpenGLObject::setCustomerName2(const QString &newCustomerName2)
{
    customerName2 = newCustomerName2;
}

void OpenGLObject::processNode(aiNode *node, const aiScene *scene)
{
    if(!node || !scene)
    {
        return;
    }
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        OpenGLMesh amesh = processMesh(mesh, scene);
        if(!amesh.isNull())
        {
            amesh.setFileName(fileName);
            meshs.push_back(amesh);
        }
    }
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

OpenGLMesh OpenGLObject::processMesh(aiMesh *mesh, const aiScene *scene)
{
    if(!mesh || !scene)
    {
        return OpenGLMesh();
    }
    QVector<OpenGLMesh::Vertex> vertices;
    QVector<unsigned int> indices;
    QVector<OpenGLMesh::Texture> textures;

    unsigned int count_vertices = mesh->mNumVertices;
    for(unsigned int i = 0; i < count_vertices; i++)
    {
        OpenGLMesh::Vertex v;
        QVector3D pos;
        if(mesh->mVertices)
        {
            pos.setX(mesh->mVertices[i].x);
            pos.setY(mesh->mVertices[i].y);
            pos.setZ(mesh->mVertices[i].z);
            v.position = pos;
        }
        QVector3D nor;
        if(mesh->mNormals)
        {
            nor.setX(mesh->mNormals[i].x);
            nor.setY(mesh->mNormals[i].y);
            nor.setZ(mesh->mNormals[i].z);
            v.normal = nor;
        }
        QVector2D tex;
        if(mesh->mTextureCoords[0])
        {
            tex.setX(mesh->mTextureCoords[0][i].x);
            tex.setY(mesh->mTextureCoords[0][i].y);
            v.texCoords = tex;
        }
        vertices.push_back(v);
    }

    unsigned int count_faces = mesh->mNumFaces;
    for(unsigned int i = 0; i < count_faces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    if(mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        if(material)
        {
            QVector<OpenGLMesh::Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE);
            textures.append(diffuseMaps);
            QVector<OpenGLMesh::Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR);
            textures.append(specularMaps);
        }
    }

    return OpenGLMesh(vertices, indices, textures);
}

QVector<OpenGLMesh::Texture> OpenGLObject::loadMaterialTextures(aiMaterial *mat, aiTextureType type)
{
    OpenGLMesh::Texture texture;
    QVector<OpenGLMesh::Texture> result;
    unsigned int count = mat->GetTextureCount(type);
    for(unsigned int i = 0; i < count; i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        texture.path = str.C_Str();
        if(type == aiTextureType_DIFFUSE)
        {
            texture.type = OpenGLMesh::DIFFUSE;
        }
        else if(type == aiTextureType_SPECULAR)
        {
            texture.type = OpenGLMesh::SPECULAR;
        }
        texture.ptex = nullptr;
        result.push_back(texture);
    }
    return result;
}
