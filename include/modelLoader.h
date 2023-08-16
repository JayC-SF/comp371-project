// resources: https://learnopengl.com/Model-Loading/Mesh
// https://learnopengl.com/Model-Loading/Model

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 UV;
};

struct Texture
{
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh
{
public:
    // Mesh data
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
    {
        // set data
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        // generate VAO for the mesh
        generateVAO();
    }

    void draw()
    {
        // bind texture
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textures[0].id);
        }

        // Render
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    // data for rendering
    unsigned int VAO, VBO, EBO;

    void generateVAO()
    {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        // setting attrib data
        // aPosition = 0
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
        // aNormal = 2
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(sizeof(glm::vec3)));
        // aUV = 3
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(2 * sizeof(glm::vec3)));

        // set EBO
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);
    }
};

class Model
{
public:
    Model()
    {
        return;
    }

    Model(std::string filePath)
    {
        loadModel(filePath);
    }

    void draw()
    {
        // loop through all meshes and draw them
        for (int i = 0; i < meshes.size(); i++)
        {
            meshes[i].draw();
        }
    }

private:
    std::vector<Mesh> meshes;
    std::vector<Texture> textures_loaded;
    std::string modelDirectory;

    void loadModel(std::string filePath)
    {
        // Import scene data from model file
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(filePath, aiProcess_Triangulate);

        // check if scene is valid
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
            return;
        }

        // set directory path
        modelDirectory = filePath.substr(0, filePath.find_last_of('/'));

        // Process the nodes of the scene
        processNode(scene->mRootNode, scene);
    }

    // recursively process the nodes, followed by the child nodes
    void processNode(aiNode *node, const aiScene *scene)
    {
        // process current node's meshes
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }

        // call processNode for all the child nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    // Process mesh and returns a Mesh object
    Mesh processMesh(aiMesh *mesh, const aiScene *scene)
    {
        // temp vectors
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        // loop through meshes and process the data
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            // Process positions, normals and uvs
            Vertex vertex;

            // Position
            glm::vec3 vector;
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            // Normal
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;

            // UV
            // check if there is textures first
            if (mesh->mTextureCoords[0])
            {
                glm::vec2 texVector;
                texVector.x = mesh->mTextureCoords[0][i].x;
                texVector.y = mesh->mTextureCoords[0][i].y;
                vertex.UV = texVector;
            }
            else
            {
                vertex.UV = glm::vec2(0.0f, 0.0f);
            }

            vertices.push_back(vertex);
        }

        // process indices
        // each face is a triangle, therefore always 3 indices per face
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        // Process textures
        // check if there are textures
        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
            std::vector<Texture> diffuseTextures = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());
        }

        return Mesh(vertices, indices, textures);
    }

    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
    {
        std::vector<Texture> textures;

        // Check if texture has already been loaded
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            bool skip = false;
            for (int j = 0; j < textures_loaded.size(); j++)
            {
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }
            if (!skip)
            {
                Texture texture;
                texture.id = loadTexture(str.C_Str(), modelDirectory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }
        return textures;
    }

    unsigned int loadTexture(const std::string filePath, const std::string &directory)
    {
        std::string filename = directory + '/' + filePath;

        // Step 1 Create and bind textures
        GLuint textureId = 0;
        glGenTextures(1, &textureId);
        assert(textureId != 0);

        glBindTexture(GL_TEXTURE_2D, textureId);

        // Step 2 Set filter parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Step3 Load Textures with dimension data
        int width, height, nrChannels;
        unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
        if (!data)
        {
            std::cerr << "Error::Texture could not load texture file: " << filename << std::endl;
            return 0;
        }

        // Step4 Upload the texture to the PU
        GLenum format = 0;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        // Step5 Free resources
        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0);
        return textureId;
    }
};