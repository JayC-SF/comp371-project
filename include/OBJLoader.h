#ifndef OBJLOADER_H
#define OBJLOADER_H
// Resources: https://learnopengl.com/Model-Loading/Mesh
// https://learnopengl.com/Model-Loading/Model
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

GLuint loadTextureAssimp(const char *filename)
{
    // Step1 Create and bind textures
    GLuint textureId = 0;
    glGenTextures(1, &textureId);
    assert(textureId != 0);

    glBindTexture(GL_TEXTURE_2D, textureId);

    // Step2 Set filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Step3 Load Textures with dimension data
    int width, height, nrChannels;
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
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
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
                 0, format, GL_UNSIGNED_BYTE, data);

    // Step5 Free resources
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureId;
}

// In assimp, a mesh is a subset of a model
// Ex: tire is a mesh of a car model

// Vertex struct
struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 UV;
};

// Texture Struct
struct Texture
{
    unsigned int textureID;
    std::string path;
};

// Mesh class for models loaded using assimp
class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    GLuint VAO, VBO, EBO;

    // Constructor
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        generateVAO();
    }

    void generateVAO()
    {
        // Generate and bind vao
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // bind VBO and data
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
        // set attribs
        // 0 = aPos
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
        glEnableVertexAttribArray(0);
        // 2 = aNormal
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(sizeof(glm::vec3)));
        glEnableVertexAttribArray(2);
        // 3 = aUV
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(2 * sizeof(glm::vec3)));
        glEnableVertexAttribArray(3);

        // bind EBO and data
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Clear all the buffers
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void draw()
    {
        // Bind VAO, draw model, unbind VAO
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void *)0);
        glBindVertexArray(0);
    }
};

// Model class to hold all the meshes that are part of the model
// Converts assimp data to glm
class Model
{
public:
    // vector to store all mesh data
    std::vector<Mesh> meshes;
    // directory name
    std::string directory;
    // Constructor
    // Takes path to .obj file
    Model(std::string path)
    {
        loadModel(path);
    }

    // Load model as a scene object
    // then process the nodes of the scene
    // each node is a seperate mesh
    void loadModel(std::string path)
    {
        // Create Importer object to import model
        Assimp::Importer modelImporter;
        const aiScene *scene = modelImporter.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs); // reads obj file and stores data in aiScene struct

        // Check if scene data is loaded
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP::" << modelImporter.GetErrorString() << std::endl;
            return;
        }

        directory = path.substr(0, path.find_last_of('/'));
        processNode(scene->mRootNode, scene);
    }

    // Process node of a scene. Go through each node and process them to get the mesh data
    // Then process the children of the nodes
    void processNode(aiNode *node, const aiScene *scene)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    // Process mesh by transforming everything to glm vars
    Mesh processMesh(aiMesh *mesh, const aiScene *scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        // Process positions, normals and UVs
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

            // Check if there are any UVs to process
            // If not, just push 0 length UV vector
            if (mesh->HasTextureCoords(0))
            {
                vertex.UV = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            }
            else
            {
                vertex.UV = glm::vec2(0.0f, 0.0f);
            }
        }

        // Process indices
        // Each mesh has a set of faces
        // faces are triangles, and in the obj, they contain the indices of the vertices of the triangles
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        // Process Textures
        // Only using diffuse textures
        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
            std::vector<Texture> diffuseMaps = loadModelTextures(material, aiTextureType_DIFFUSE);
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        }
        return Mesh(vertices, indices, textures);
    }

    // Function to load textures from the model
    std::vector<Texture> loadModelTextures(aiMaterial *mat, aiTextureType type)
    {
        std::vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            Texture texture;
            std::string filepath = directory + '/' + str.C_Str();
            texture.textureID = loadTextureAssimp(filepath.c_str());
            texture.path = str.C_Str();
            textures.push_back(texture);
        }
        return textures;
    }

    // Draw model
    void draw()
    {
        for (int i = 0; i < meshes.size(); i++)
        {
            meshes[i].draw();
        }
    };
};
#endif