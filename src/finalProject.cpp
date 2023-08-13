#define GLEW_STATIC 1 // This allows linking with Static Library on Windows, without DLL
#include <iostream>
#include <GL/glew.h>                    // Include GLEW - OpenGL Extension Wrangler
#include <GLFW/glfw3.h>                 // GLFW provides a cross-platform interface for creating a graphical context initializing OpenGL and binding inputs
#include <glm/glm.hpp>                  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>
#include <random>
#include <fstream> // std::ifstream
#include <sstream> // std::stringstream, std::stringbuf
#include "glm/gtx/string_cast.hpp"

#include "objects/include.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace glm;
using namespace std;

// Color vectors
#pragma region
vec3 colorRed(1.0f, 0.20f, 0.20f);
vec3 colorGreen(0.0f, 1.0f, 0.0f);
vec3 colorBlue(0.0f, 0.0f, 1.0f);
vec3 colorYellow(1.0f, 1.0f, 0.0f);
vec3 colorSkin(0.626f, 0.555f, 0.44f);
vec3 colorBlack(0.0f, 0.0f, 0.0f);
vec3 colorGrey(0.827f, 0.827f, 0.827f);
vec3 colorDarkSkin(0.426f, 0.555f, 0.6f);
vec3 colorPurple(1.0f, 0.0f, 1.0f);
vec3 colorPink(0.706f, 0.1911f, 0.462f);
vec3 colorBeige(0.92f, 0.725f, 0.67f);
vec3 colorSkyBlue(0.357f, 0.506f, 0.831f);
vec3 colorBrown(0.733f, 0.18f, 0.18f);
vec3 colorDarkBlue(0.098f, 0.231f, 0.522f);
vec3 colorWhite(1.0f, 1.0f, 1.0f);
vec3 colorLightBlue(0.678f, 0.847f, 0.902f);
#pragma endregion

// 0 for triangles, 1 for points and 2 for lines
int renderMode = 0;
int currentRenderMode = GL_TRIANGLES;

vec3 normal_1;
vec3 normal_2;

vec3 normals[2] = { // normals for racket1 (+x) and racket2 (-x)
    normal_1 = vec3(1.0f),
    normal_2 = vec3(1.0f)
};

vec3 centre_1;
vec3 centre_2;

vec3 centers[2] = { // normals for racket1 (+x) and racket2 (-x)
    centre_1 = vec4(1.0f),
    centre_2 = vec4(1.0f)
};

// Directional vectors
vec3 MY_LEFT(-1.0f, 0.0f, 0.0f);
vec3 MY_RIGHT(1.0f, 0.0f, 0.0f);
vec3 MY_UP(0.0f, 1.0f, 0.0f);
vec3 MY_DOWN(0.0f, -1.0f, 0.0f);
vec3 MY_FORWARD(0.0f, 0.0f, -1.0f);
vec3 MY_BACKWARD(0.0f, 0.0f, 1.0f);

// Identity Matrix
mat4 IDENTITY_MATRIX(1.0f);

TennisBall tennisBall;

GLuint loadTexture(const char *filename)
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

void checkCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}
int shader(const char *vertexPath, const char *fragmentPath) // doesn't change, just receives the sources
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure &e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();
    // 2. compile shaders
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    // shader Program
    int ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return ID;
}

struct TexturedColoredVertex
{
    TexturedColoredVertex(vec3 _position, vec3 _normal, vec2 _uv)
        : position(_position), normal(_normal), uv(_uv) {}

    vec3 position;
    vec3 normal;
    vec2 uv;
};

int create_modelCube_VAO()
{
    const TexturedColoredVertex texturedCubeVertexArray[] = {
        //                    position,                normal                  texture
        TexturedColoredVertex(vec3(-0.5f, -0.5f, -0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)), // left
        TexturedColoredVertex(vec3(-0.5f, -0.5f, 0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 5.0f)),

        TexturedColoredVertex(vec3(-0.5f, -0.5f, -0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f, -0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f)),

        TexturedColoredVertex(vec3(0.5f, 0.5f, -0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 1.0f)), // far
        TexturedColoredVertex(vec3(-0.5f, -0.5f, -0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 0.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f, -0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 1.0f)),

        TexturedColoredVertex(vec3(0.5f, 0.5f, -0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 1.0f)),
        TexturedColoredVertex(vec3(0.5f, -0.5f, -0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 0.0f)),
        TexturedColoredVertex(vec3(-0.5f, -0.5f, -0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 0.0f)),

        TexturedColoredVertex(vec3(0.5f, -0.5f, 0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 1.0f)), // bottom
        TexturedColoredVertex(vec3(-0.5f, -0.5f, -0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 0.0f)),
        TexturedColoredVertex(vec3(0.5f, -0.5f, -0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 0.0f)),

        TexturedColoredVertex(vec3(0.5f, -0.5f, 0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 1.0f)),
        TexturedColoredVertex(vec3(-0.5f, -0.5f, 0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 1.0f)),
        TexturedColoredVertex(vec3(-0.5f, -0.5f, -0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 0.0f)),

        TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f)), // near
        TexturedColoredVertex(vec3(-0.5f, -0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f)),
        TexturedColoredVertex(vec3(0.5f, -0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f)),

        TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f)),
        TexturedColoredVertex(vec3(0.5f, -0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f)),

        TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)), // right
        TexturedColoredVertex(vec3(0.5f, -0.5f, -0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
        TexturedColoredVertex(vec3(0.5f, 0.5f, -0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f)),

        TexturedColoredVertex(vec3(0.5f, -0.5f, -0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
        TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)),
        TexturedColoredVertex(vec3(0.5f, -0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f)),

        TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f)), // top
        TexturedColoredVertex(vec3(0.5f, 0.5f, -0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 0.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f, -0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)),

        TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f, -0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 1.0f))};

    // Create a vertex array (VAO)
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    // Create a VBO
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texturedCubeVertexArray), texturedCubeVertexArray, GL_STATIC_DRAW);

    int indicesLines[] = {

    };
    // Create an EBO for the LINE draw
    GLuint EBO_LINES;
    glGenBuffers(1, &EBO_LINES);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_LINES);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesLines), indicesLines, GL_STATIC_DRAW);

    // Create poition pointer
    glVertexAttribPointer(0,                             // attribute 0 matches aPos in Vertex Shader
                          3,                             // size
                          GL_FLOAT,                      // type
                          GL_FALSE,                      // normalized?
                          sizeof(TexturedColoredVertex), // stride - each vertex contain 2 vec3 (position, color)
                          (void *)0                      // array buffer offset
    );
    glEnableVertexAttribArray(0);

    // create normal pointer
    glVertexAttribPointer(2, // attribute 2 matches normal in Vertex Shader
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(TexturedColoredVertex),
                          (void *)(sizeof(vec3)) // uv is offseted by 2 vec3 (comes after position and color)
    );
    glEnableVertexAttribArray(2);

    // create UV pointer
    glVertexAttribPointer(3, // attribute 3 matches aUV in Vertex Shader
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(TexturedColoredVertex),
                          (void *)((sizeof(vec3)) + sizeof(vec3)) // uv is offseted by 2 vec3 (comes after position and color)
    );
    glEnableVertexAttribArray(3);

    // Clear all the buffers
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return vertexArrayObject;
}

int create_modelCube_VAO_skyBox()
{
    TexturedColoredVertex y_axisArray[] = {// without EBO
                                           // cube to make the x- axis in GREEN

                                           TexturedColoredVertex(vec3(-0.5f, -0.5f, -0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f)), // left wall
                                           TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f)),
                                           TexturedColoredVertex(vec3(-0.5f, -0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
                                           TexturedColoredVertex(vec3(-0.5f, -0.5f, -0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f)),
                                           TexturedColoredVertex(vec3(-0.5f, 0.5f, -0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)),
                                           TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f)),
                                           TexturedColoredVertex(vec3(-0.5f, 0.5f, -0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f)), // front wall
                                           TexturedColoredVertex(vec3(-0.5f, -0.5f, -0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f)),
                                           TexturedColoredVertex(vec3(0.5f, -0.5f, -0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f)),
                                           TexturedColoredVertex(vec3(-0.5f, 0.5f, -0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f)),
                                           TexturedColoredVertex(vec3(0.5f, -0.5f, -0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f)),
                                           TexturedColoredVertex(vec3(0.5f, 0.5f, -0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f)),
                                           TexturedColoredVertex(vec3(0.5f, -0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f)), // floor
                                           TexturedColoredVertex(vec3(0.5f, -0.5f, -0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)),
                                           TexturedColoredVertex(vec3(-0.5f, -0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 0.0f)),
                                           TexturedColoredVertex(vec3(-0.5f, -0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f)),
                                           TexturedColoredVertex(vec3(0.5f, -0.5f, -0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 1.0f)),
                                           TexturedColoredVertex(vec3(-0.5f, -0.5f, -0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)),
                                           TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 1.0f)), // back wall
                                           TexturedColoredVertex(vec3(0.5f, -0.5f, 0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 0.0f)),
                                           TexturedColoredVertex(vec3(-0.5f, -0.5f, 0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 0.0f)),
                                           TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 1.0f)),
                                           TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 1.0f)),
                                           TexturedColoredVertex(vec3(0.5f, -0.5f, 0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 0.0f)),
                                           TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)), // right wall
                                           TexturedColoredVertex(vec3(0.5f, 0.5f, -0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f)),
                                           TexturedColoredVertex(vec3(0.5f, -0.5f, -0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)),
                                           TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
                                           TexturedColoredVertex(vec3(0.5f, -0.5f, -0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)),
                                           TexturedColoredVertex(vec3(0.5f, -0.5f, 0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f)),
                                           TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 1.0f)), // ceiling wall
                                           TexturedColoredVertex(vec3(0.5f, 0.5f, -0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 0.0f)),
                                           TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 1.0f)),
                                           TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 1.0f)),
                                           TexturedColoredVertex(vec3(-0.5f, 0.5f, -0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 0.0f)),
                                           TexturedColoredVertex(vec3(0.5f, 0.5f, -0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 0.0f))

    };

    
    // Create a vertex array (VAO)
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    // Create a VBO
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(y_axisArray), y_axisArray, GL_STATIC_DRAW);

    // Create poition pointer
    glVertexAttribPointer(0,                             // attribute 0 matches aPos in Vertex Shader
                          3,                             // size
                          GL_FLOAT,                      // type
                          GL_FALSE,                      // normalized?
                          sizeof(TexturedColoredVertex), // stride - each vertex contain 2 vec3 (position, color)
                          (void *)0                      // array buffer offset
    );
    glEnableVertexAttribArray(0);

    // create normal pointer
    glVertexAttribPointer(2, // attribute 2 matches normal in Vertex Shader
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(TexturedColoredVertex),
                          (void *)(sizeof(vec3)) // uv is offseted by 2 vec3 (comes after position and color)
    );
    // glEnableVertexAttribArray(2);

    // create UV pointer
    glVertexAttribPointer(3, // attribute 3 matches aUV in Vertex Shader
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(TexturedColoredVertex),
                          (void *)((sizeof(vec3)) + sizeof(vec3)) // uv is offseted by 2 vec3 (comes after position and color)
    );
    glEnableVertexAttribArray(3);

    // Clear all the buffers
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return vertexArrayObject;
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front)
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void setProjectionMatrix(int shaderProgram, mat4 projectionMatrix)
{
    glUseProgram(shaderProgram);
    GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
}
void setViewMatrix(int shaderProgram, mat4 viewMatrix)
{
    glUseProgram(shaderProgram);
    GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
}
void setModelMatrix(int shaderProgram, mat4 modelMatrix)
{
    glUseProgram(shaderProgram);
    GLuint modelMatrixLocation = glGetUniformLocation(shaderProgram, "modelMatrix");
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
}
void setColorUniform(int shaderProgram, vec3 colorVec)
{
    glUseProgram(shaderProgram);
    GLuint colorLocation = glGetUniformLocation(shaderProgram, "myColor");
    glUniform3fv(colorLocation, 1, &colorVec[0]);
}
void setUseTexture(int shaderProgram, int status)
{
    glUseProgram(shaderProgram);
    GLuint useTextureLocation = glGetUniformLocation(shaderProgram, "useTexture");
    glUniform1i(useTextureLocation, status);
}
void setMaterial(int shaderProgram, vec3 setAmbient, vec3 setDiffuse, vec3 setSpecular, float shine)
{
    // MODIFY THE LIGHT COMPONENTS TO REALLY SHINY.
    glUseProgram(shaderProgram);
    // set the ambient factor
    GLuint vec3Location = glGetUniformLocation(shaderProgram, "setAmbient");
    glUniform3fv(vec3Location, 1, &setAmbient[0]);
    // set the diffuse factor
    vec3Location = glGetUniformLocation(shaderProgram, "setDiffuse");
    glUniform3fv(vec3Location, 1, &setDiffuse[0]);
    // set the specular factor and shininess
    vec3Location = glGetUniformLocation(shaderProgram, "setSpecular");
    glUniform3fv(vec3Location, 1, &setSpecular[0]);
    GLuint shineLoc = glGetUniformLocation(shaderProgram, "shine");
    glUniform1f(shineLoc, shine);
}
void setSpotLight(int shaderProgram, vec3 position, float intensity, int currentCameraDecision, vec3 LookAtVector)
{
    glUseProgram(shaderProgram);

    GLuint positionLocation = glGetUniformLocation(shaderProgram, "spotLPos");
    glUniform3fv(positionLocation, 1, &position[0]);

    GLuint intentsityLocation = glGetUniformLocation(shaderProgram, "spotLIntensity");
    glUniform1f(intentsityLocation, currentCameraDecision == 3 ? intensity : 0.0f);

    GLuint LALocation = glGetUniformLocation(shaderProgram, "spotLlookAtPos");
    glUniform3fv(LALocation, 1, &LookAtVector[0]);
}
void setLightPos(int shaderProgram, vec3 pos){
    GLuint texturedLightLocation = glGetUniformLocation(shaderProgram, "lightPos");
    glUniform3fv(texturedLightLocation, 1, &pos[0]);
}
void setShadowMap(int shaderProgram, int value){
    GLuint depthTexturedLocation = glGetUniformLocation(shaderProgram, "shadowMap");
    glUniform1i(depthTexturedLocation, value);
}

// ************************* GLOBALIZATION FOR THE DRAWSCENE FUNCTION PARAMETERS ***************************
// load textures
GLuint brickID,skyID,cementID,glossyID,woodID,fabricID,metalID,tennisID, ballID;

// *** Creating the VAOs ***

// the xyz axis coordinates
int baseCube_VAO;

// skybox with inverted direction
int skyBox_VAO_inside;

// Group matrix array used to refer to from within the loop
// To create the rackets. These matrices set the inital position of the
// Rackets
mat4 translationMatrixArray[2] = {
    translate(IDENTITY_MATRIX, vec3(-25.0f, 0.5, 8.0f)), // racket 1
    translate(IDENTITY_MATRIX, vec3(25.0f, 0.5, -8.0f))}; // racket 2

// Set the initial orientation of the rackets
mat4 rotationMatrixArray[2] = {
    rotate(IDENTITY_MATRIX, radians(-30.0f), vec3(1.0f, 0.0f, 0.0f)),  // racket 1
    rotate(IDENTITY_MATRIX, radians(-30.0f), vec3(1.0f, 0.0f, 0.0f))}; // racket 2

mat4 fullModel_translationMatrix(1.0f);
mat4 fullModel_rotationMatrix(1.0f);
mat4 racketHandle_groupMatrix;

float totalShoulderRotation1 = radians(0.0f);
float totalElbowRotation1 = radians(0.0f);
float totalElbowRotationbackwards1 = radians(0.0f);
float totalElbowRotationbackwards2 = radians(0.0f);
float totalWristRotation1 = radians(0.0f);

float totalShoulderRotation2 = radians(0.0f);
float totalElbowRotation2 = radians(0.0f);
float totalWristRotation2 = radians(0.0f);

mat4 elbowFlexor_rotationMatrix1;
mat4 wristFlexor_rotationMatrix1;
mat4 elbowFlexor_rotationMatrix2;
mat4 wristFlexor_rotationMatrix2;

mat4 elbow [2] = {
    elbowFlexor_rotationMatrix1 = IDENTITY_MATRIX,
    elbowFlexor_rotationMatrix2 = IDENTITY_MATRIX
};

mat4 wrist [2] = {
    wristFlexor_rotationMatrix1 = IDENTITY_MATRIX,
    wristFlexor_rotationMatrix2 = IDENTITY_MATRIX
};

// sphere VAO 
// source code obtained from here: https://github.com/carl-vbn/opengl-gravity-simulator/blob/main/src/rendering/baseModels/sphere.cpp
struct TexturedVertex
{
    TexturedVertex(glm::vec3 _position, glm::vec2 _uv)
            : position(_position), uv(_uv) {}

    glm::vec3 position;
    glm::vec2 uv; // UV coordinates
};

GLuint createSphere(int resolution, float radius)
{
    std::vector<TexturedVertex> vertices;
    std::vector<GLuint> indices;

    for (int i = 0; i < resolution / 2; i++)
    {
        for (int j = 0; j < resolution; j++)
        {
            float iRads = static_cast<float>(i) / resolution * glm::two_pi<float>();
            float jRads = static_cast<float>(j) / resolution * glm::two_pi<float>();

            float x = std::sin(jRads) * std::sin(iRads);
            float y = std::cos(iRads);
            float z = std::cos(jRads) * std::sin(iRads);

            float u = static_cast<float>(j) / (resolution - 1);
            float v = static_cast<float>(i) / ((resolution / 2) - 1);

            vertices.push_back(TexturedVertex(glm::vec3(x, y, z) * radius, glm::vec2(u, v)));
        }
    }

    // Bottom vertex
    vertices.push_back(TexturedVertex(glm::vec3(0, -radius, 0), glm::vec2(0.0f, 0.0f)));

    int vertexCount = vertices.size();

    for (int i = 0; i < resolution * ((resolution - 2) / 2); i++)
    {
        indices.push_back(i);
        indices.push_back(i + resolution - 1);
        indices.push_back(i + resolution);

        indices.push_back(i + resolution);
        indices.push_back(i + 1);
        indices.push_back(i);
    }

    // Bottom faces
    for (int i = 0; i < resolution - 1; i++)
    {
        indices.push_back(vertexCount - 1);
        indices.push_back(vertexCount - 1 - resolution + i + 1);
        indices.push_back(vertexCount - 1 - resolution + i);
    }

    // Last triangle
    indices.push_back(vertexCount - resolution - 1);
    indices.push_back(vertexCount - 2);
    indices.push_back(vertexCount - 1);

    std::vector<GLfloat> vertexDataVector;

    for (int i = 0; i < vertexCount; i++)
    {
        TexturedVertex vertex = vertices[i];
        vertexDataVector.push_back(vertex.position.x);
        vertexDataVector.push_back(vertex.position.y);
        vertexDataVector.push_back(vertex.position.z);
        vertexDataVector.push_back(vertex.uv.x);
        vertexDataVector.push_back(vertex.uv.y);
    }


    // Create a vertex array object
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, vertexDataVector.size() * sizeof(GLfloat), &vertexDataVector[0], GL_STATIC_DRAW);

    // Set the vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0); // Vertex position
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0); // Vertex position
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))); // UV coordinates

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    // Upload Index Buffer to the GPU, keep a reference to it (indexBufferObject)
    GLuint indexBufferObject;
    glGenBuffers(1, &indexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    // Unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return vertexArrayObject;
}
// sphere variables
int resolution = 65;
int vertexCount = 6 * (resolution / 2) * resolution + 1;
GLuint sphere;

void drawScene(int shaderProgram, mat4 elbow [], mat4 wrist[])
{
    // SRT for the upper arm
    // Model matrix components for the upper arm
    mat4 upperArm_scaleMatrix = scale(IDENTITY_MATRIX, vec3(0.2f, 1.7f, 0.35f));
    mat4 upperArm_rotationMatrix = rotate(IDENTITY_MATRIX, radians(-45.0f), vec3(1.0f, 0.0f, 0.0f));
    mat4 upperArm_translationMatrix = IDENTITY_MATRIX;

    // SRT for the lower arm
    // Model matrix components for the lower arm
    mat4 lowerArm_scaleMatrix = scale(IDENTITY_MATRIX, vec3(0.2f, 1.7f, 0.35f));
    mat4 lowerArm_rotationMatrix = rotate(IDENTITY_MATRIX, radians(45.0f), vec3(1.0f, 0.0f, 0.0f));
    mat4 lowerArm_translationMatrix = translate(IDENTITY_MATRIX, vec3(0.0f, 1.7f, 0.0f));

    // SRT for the hand
    // Model matrix components for the hand
    mat4 hand_scaleMatrix = scale(IDENTITY_MATRIX, vec3(0.5f, 0.7f, 0.5f));
    mat4 hand_rotationMatrix = rotate(IDENTITY_MATRIX, radians(30.0f), vec3(1.0f, 0.0f, 0.0f));
    mat4 hand_translationMatrix = translate(IDENTITY_MATRIX, vec3(0.0f, 1.7, 0.0f));

    // TRS for the handle
    mat4 racketHandle_scaleMatrix = scale(IDENTITY_MATRIX, vec3(0.2f, 2.2f, 0.2f));
    mat4 racketHandle_rotationMatrix = rotate(IDENTITY_MATRIX, radians(30.0f), vec3(1.0f, 0.0f, 0.0f));
    mat4 racketHandle_translationMatrix = translate(IDENTITY_MATRIX, vec3(0.0f, 1.7, 0.0f));

    // The bottom bracket
    mat4 bottomBracket_scaleMatrix = scale(IDENTITY_MATRIX, vec3(0.1f, 0.1f, 1.3f));
    mat4 bottomBracket_rotationMatrix = rotate(IDENTITY_MATRIX, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
    mat4 bottomBracket_translationMatrix = translate(IDENTITY_MATRIX, vec3(0.0f, 2.2, 0.0f));

    // The top bracket
    mat4 topBracket_scaleMatrix = scale(IDENTITY_MATRIX, vec3(0.1f, 0.1f, 1.3f));
    mat4 topBracket_rotationMatrix = rotate(IDENTITY_MATRIX, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
    mat4 topBracket_translationMatrix = translate(IDENTITY_MATRIX, vec3(0.0f, 3.8, 0.0f));

    // The Left Bracket
    mat4 leftBracket_scaleMatrix = scale(IDENTITY_MATRIX, vec3(0.1f, 1.7f, 0.1f));
    mat4 leftBracket_rotationMatrix = rotate(IDENTITY_MATRIX, radians(0.0f), vec3(1.0, 0.0, 0.0f));
    mat4 leftBracket_translationMatrix = translate(IDENTITY_MATRIX, vec3(0.0f,2.2f,0.65f));

    // The right Bracket
    mat4 rightBracket_scaleMatrix = scale(IDENTITY_MATRIX, vec3(0.1f, 1.7f, 0.1f));
    mat4 rightBracket_rotationMatrix = rotate(IDENTITY_MATRIX, radians(0.0f), vec3(1.0, 0.0, 0.0f));
    mat4 rightBracket_translationMatrix = translate(IDENTITY_MATRIX, vec3(0.0f,2.2f,-0.65f));

    // matrix that translates the initial cube upwards by 0.5
    mat4 initialCubeTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, 0.5f, 0.0f));
    GLuint initialCubeTranslateLocation = glGetUniformLocation(shaderProgram, "modelMatrix");
    glUniformMatrix4fv(initialCubeTranslateLocation, 1, GL_FALSE, &initialCubeTranslate[0][0]);

    //              ************************** START OF RENDERING **************************
    //                             ************* RENDER THE COURSE NET *************
    #pragma region
    setMaterial(shaderProgram, vec3(1.0), vec3(1.0), vec3(0.4, 0.4, 0.4), 1.f);
    glBindTexture(GL_TEXTURE_2D, brickID);
    // bind the standard cube with outward surfaces.
    glBindVertexArray(baseCube_VAO);
    // Set the color of the grid to black
    GLuint colorLocation = glGetUniformLocation(shaderProgram, "myColor");
    glUniform3fv(colorLocation, 1, &colorBlack[0]);

    // draw the grid by applying the model matrix transformations
    for (int i = 0; i < 6; i++)
    {
        // Model matrix of the grid to set it the long axis net lines
        mat4 grid_modelMatrix = translate(IDENTITY_MATRIX, vec3(0.0f, 0.1f + (i / 2.0f), 0.0f)) *
                                rotate(IDENTITY_MATRIX, radians(0.0f), vec3(0.0f, 1.0f, 0.0f)) *
                                scale(IDENTITY_MATRIX, vec3(0.05f, 0.05f, 36.0f));
        GLuint grid_modelMatrixLocation = glGetUniformLocation(shaderProgram, "modelMatrix");
        glUniformMatrix4fv(grid_modelMatrixLocation, 1, GL_FALSE, &grid_modelMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    // The top part of the net with thicker shape and different color
    glBindTexture(GL_TEXTURE_2D, fabricID);
    mat4 grid_modelMatrix = translate(IDENTITY_MATRIX, vec3(0.0f, 3.1f, 0.0f)) *
                            rotate(IDENTITY_MATRIX, radians(0.0f), vec3(0.0f, 1.0f, 0.0f)) *
                            scale(IDENTITY_MATRIX, vec3(0.08f, 0.5f, 36.0f));
    GLuint grid_modelMatrixLocation = glGetUniformLocation(shaderProgram, "modelMatrix");
    glUniformMatrix4fv(grid_modelMatrixLocation, 1, GL_FALSE, &grid_modelMatrix[0][0]);
    colorLocation = glGetUniformLocation(shaderProgram, "myColor");
    glUniform3fv(colorLocation, 1, &colorWhite[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // the poles to hold the net
    colorLocation = glGetUniformLocation(shaderProgram, "myColor");
    glUniform3fv(colorLocation, 1, &colorGrey[0]);
    glBindTexture(GL_TEXTURE_2D, woodID);
    // The top part of the net with thicker shape and different color
    mat4 poles_modelMatrix = translate(IDENTITY_MATRIX, vec3(0.0f, 1.5f, 0.0f)) *
                             rotate(IDENTITY_MATRIX, radians(90.0f), vec3(0.0f, 0.0f, 1.0f)) *
                             scale(IDENTITY_MATRIX, vec3(6.46f, 0.5f, 0.3f));
    GLuint poles_modelMatrixLocation = glGetUniformLocation(shaderProgram, "modelMatrix");
    glUniformMatrix4fv(poles_modelMatrixLocation, 1, GL_FALSE, &poles_modelMatrix[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // right pole
    glUniformMatrix4fv(poles_modelMatrixLocation, 1, GL_FALSE, &(translate(IDENTITY_MATRIX, vec3(0.0f, 0.0f, 17.8f)) * poles_modelMatrix)[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // left pole pole
    glUniformMatrix4fv(poles_modelMatrixLocation, 1, GL_FALSE, &(translate(IDENTITY_MATRIX, vec3(0.0f, 0.0f, -17.8f)) * poles_modelMatrix)[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    #pragma endregion

    //                             ************* RENDER THE FLOOR *************
    #pragma region
    glBindTexture(GL_TEXTURE_2D, tennisID);
    grid_modelMatrix = translate(IDENTITY_MATRIX, vec3(0.0f, -0.3f, 0.0f)) *
                       rotate(IDENTITY_MATRIX, radians(0.0f), vec3(0.0f, 1.0f, 0.0f)) *
                       scale(IDENTITY_MATRIX, vec3(87.0f, 0.5f, 45.0f));
    grid_modelMatrixLocation = glGetUniformLocation(shaderProgram, "modelMatrix");
    glUniformMatrix4fv(grid_modelMatrixLocation, 1, GL_FALSE, &grid_modelMatrix[0][0]);
    colorLocation = glGetUniformLocation(shaderProgram, "myColor");
    glUniform3fv(colorLocation, 1, &colorLightBlue[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    colorLocation = glGetUniformLocation(shaderProgram, "myColor");
    glUniform3fv(colorLocation, 1, &colorBlack[0]);

    // the rest of the net
    for (int i = 0; i < 73; i++)
    {
        // Model matrix of the grid to set it the short axis net lines
        mat4 grid_modelMatrix = translate(IDENTITY_MATRIX, vec3(0.0f, 1.5f, -18.0f + (i / 2.0f))) *
                                rotate(IDENTITY_MATRIX, radians(90.0f), vec3(0.0f, 0.0f, 1.0f)) *
                                scale(IDENTITY_MATRIX, vec3(3.0f, 0.05f, 0.05f));
        GLuint grid_modelMatrixLocation = glGetUniformLocation(shaderProgram, "modelMatrix");
        glUniformMatrix4fv(grid_modelMatrixLocation, 1, GL_FALSE, &grid_modelMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindTexture(GL_TEXTURE_2D, brickID);
    #pragma endregion



    //                                     **************** RENDER THE TWO MODELS ****************
    
    // a for loop to render the four rackets

    for (int i = 0; i < 2; i++)
    {
        fullModel_translationMatrix = translationMatrixArray[i];
        fullModel_rotationMatrix = rotationMatrixArray[i];

        //                                     **************** RENDER THE UPPER ARM ****************
        glBindTexture(GL_TEXTURE_2D, fabricID);
        setMaterial(shaderProgram, vec3(1.0), vec3(1.0), vec3(0.2, 0.2, 0.2), 1.f);
        // group matrix number 1
        mat4 shoulder_groupMatrix = fullModel_translationMatrix * fullModel_rotationMatrix * upperArm_translationMatrix * upperArm_rotationMatrix;
        // The upper arm model matrix
        mat4 upperArm_modelMatrix = shoulder_groupMatrix * upperArm_scaleMatrix * translate(IDENTITY_MATRIX, vec3(0.0f, 0.5f, 0.0f));;
        // update shader 
        setModelMatrix(shaderProgram, upperArm_modelMatrix);                      
        setColorUniform(shaderProgram, colorSkin);
        glDrawArrays(currentRenderMode, 0, 36);

        //                                    **************** RENDER THE LOWER ARM ****************    
        // Elbow group matrix that contains the shoulder group matrix
        #pragma region 
        // group matrix 2
        mat4 elbow_groupMatrix = shoulder_groupMatrix  * lowerArm_translationMatrix * elbow[i]  *  lowerArm_rotationMatrix;
        // Lower arm model matrix
        mat4 lowerArm_modelMatrix = elbow_groupMatrix * lowerArm_scaleMatrix * translate(IDENTITY_MATRIX, vec3(0.0f, 0.5f, 0.0f));
        // update the shader
        setModelMatrix(shaderProgram, lowerArm_modelMatrix);
        glDrawArrays(currentRenderMode, 0, 36);
        #pragma endregion

        //                                     **************** RENDER THE HAND ****************  
        // Model matrix of the racket handle
        glBindTexture(GL_TEXTURE_2D, fabricID);
        #pragma region 
        // group matrix 3
        mat4 hand_groupMatrix = elbow_groupMatrix * hand_translationMatrix *  wrist[i] *  hand_rotationMatrix;
                                        
        mat4 hand_modelMatrix = hand_groupMatrix * hand_scaleMatrix *translate(IDENTITY_MATRIX, vec3(0.0f, 0.5f, 0.0f));
        setColorUniform(shaderProgram, colorSkin);

        GLuint hand_modelMatrixLocation = glGetUniformLocation(shaderProgram, "modelMatrix");
        glUniformMatrix4fv(hand_modelMatrixLocation, 1, GL_FALSE, &hand_modelMatrix[0][0]);
        glDrawArrays(currentRenderMode, 0, 36);
        #pragma endregione

        // MODIFY THE LIGHT COMPONENTS TO REALLY METALIC LOOK.
        //                                 amb                            diff                             spec                                shine
        setMaterial(shaderProgram, vec3(0.24725, 0.1995, 0.0745), vec3(0.75164, 0.60648, 0.22648), vec3(0.628281, 0.555802, 0.366065), 0.05f);
        glBindTexture(GL_TEXTURE_2D, metalID);

        //                               *** RENDER THE RACKET HANDLE ***
        // Model matrix of the racket handle
        // Retrieve the matrices from the matrix Array

        // create the model matrix
        racketHandle_groupMatrix = elbow_groupMatrix *
                                   racketHandle_translationMatrix *
                                    wrist[i] *
                                   racketHandle_rotationMatrix;

        mat4 racketHandle_modelMatrix = racketHandle_groupMatrix *
                                        racketHandle_scaleMatrix *
                                        initialCubeTranslate;

        colorLocation = glGetUniformLocation(shaderProgram, "myColor");
        glUniform3fv(colorLocation, 1, &colorRed[0]);

        // update shader and draw the handle
        GLuint racketHandle_modelMatrixLocation = glGetUniformLocation(shaderProgram, "modelMatrix");
        glUniformMatrix4fv(racketHandle_modelMatrixLocation, 1, GL_FALSE, &racketHandle_modelMatrix[0][0]);
        glDrawArrays(currentRenderMode, 0, 36);

        // ******************* two centres of the racket planes **********************
        // ***************************************************************************
        
        mat4 aCentre = racketHandle_groupMatrix * translate(IDENTITY_MATRIX, vec3(0.0f, 3.05f, 0.0f)) * scale(IDENTITY_MATRIX, vec3(0.1f, 4.f, 2.5f)) * initialCubeTranslate;
        GLuint centreLocation = glGetUniformLocation(shaderProgram, "modelMatrix");
        glUniformMatrix4fv(centreLocation, 1, GL_FALSE, &aCentre[0][0]);
        glDrawArrays(currentRenderMode, 0, 36);

        // look good for a now
        centers[i] = vec3(aCentre * vec4(0.f, 0.f, 0.f, 1.f));
        // std::cout << glm::to_string(centers[i])<< std::endl;

        if (i == 0){
            normals[i] = normalize(vec3(aCentre * vec4(-1.0f, 0.0f, 0.0f, 0.0f)));
        }else if (i == 1 ){
            normals[i] = normalize(vec3(aCentre * vec4(1.0f, 0.0f, 0.0f, 0.0f)));
        }
        // std::cout << glm::to_string(normals[i])<< std::endl;

        // ***************************************************************************
        // ***************************************************************************

        



        //                                 *** RENDER THE RACKET BOTTOM BRACKET ***
        // Model matrix components for the racket bottom bracket
        // Model matrix of the racket handle
        mat4 bottomRacket_modelMatrix = racketHandle_groupMatrix *
                                        bottomBracket_translationMatrix *
                                        bottomBracket_rotationMatrix *
                                        bottomBracket_scaleMatrix *
                                        initialCubeTranslate;
        GLuint bottomRacket_modelMatrixLocation = glGetUniformLocation(shaderProgram, "modelMatrix");
        glUniformMatrix4fv(bottomRacket_modelMatrixLocation, 1, GL_FALSE, &bottomRacket_modelMatrix[0][0]);
        glDrawArrays(currentRenderMode, 0, 36);

        //                                 *** RENDER THE RACKET TOP BRACKET ***
        // Model matrix components for the racket top bracket
        // Model matrix of the racket handle
        mat4 topRacket_modelMatrix = racketHandle_groupMatrix *
                                     topBracket_translationMatrix *
                                     topBracket_rotationMatrix *
                                     topBracket_scaleMatrix *
                                     initialCubeTranslate;
        GLuint topRacket_modelMatrixLocation = glGetUniformLocation(shaderProgram, "modelMatrix");
        glUniformMatrix4fv(topRacket_modelMatrixLocation, 1, GL_FALSE, &topRacket_modelMatrix[0][0]);
        glDrawArrays(currentRenderMode, 0, 36);

        //                                   *** RENDER THE RACKET LEFT BRACKET ***
        // Model matrix components for the racket left bracket
        // Model matrix of the racket handle
        mat4 leftBracket_modelMatrix = racketHandle_groupMatrix *
                                       leftBracket_translationMatrix *
                                       leftBracket_rotationMatrix *
                                       leftBracket_scaleMatrix *
                                       initialCubeTranslate;
        GLuint leftBracket_modelMatrixLocation = glGetUniformLocation(shaderProgram, "modelMatrix");
        glUniformMatrix4fv(leftBracket_modelMatrixLocation, 1, GL_FALSE, &leftBracket_modelMatrix[0][0]);
        glDrawArrays(currentRenderMode, 0, 36);

        //                                *** RENDER THE RACKET right BRACKET ***
        // Model matrix components for the racket right bracket
        // Model matrix of the racket handle
        mat4 rightBracket_modelMatrix = racketHandle_groupMatrix *
                                        rightBracket_translationMatrix *
                                        rightBracket_rotationMatrix *
                                        rightBracket_scaleMatrix *
                                        initialCubeTranslate;
        GLuint rightBracket_modelMatrixLocation = glGetUniformLocation(shaderProgram, "modelMatrix");
        glUniformMatrix4fv(rightBracket_modelMatrixLocation, 1, GL_FALSE, &rightBracket_modelMatrix[0][0]);
        glDrawArrays(currentRenderMode, 0, 36);

        //                                  *** RENDER THE RACKET MESH ***
        // Model matrix components for the racket mesh
        // The vertical mesh
        for (int i = 0; i < 8; i++)
        {
            mat4 mesh_scaleMatrix = scale(IDENTITY_MATRIX, vec3(0.015f, 0.015f, 1.3f));
            mat4 mesh_rotationMatrix = rotate(IDENTITY_MATRIX, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
            mat4 mesh_translationMatrix = translate(IDENTITY_MATRIX, vec3(0.0f, 2.2 + (i/5.0f), 0.0f));
            // Model matrix of the racket handle
            mat4 mesh_modelMatrix = racketHandle_groupMatrix *
                                    mesh_translationMatrix *
                                    mesh_rotationMatrix *
                                    mesh_scaleMatrix *
                                    initialCubeTranslate;
            colorLocation = glGetUniformLocation(shaderProgram, "myColor");
            glUniform3fv(colorLocation, 1, &colorBlack[0]);
            GLuint mesh_modelMatrixLocation = glGetUniformLocation(shaderProgram, "modelMatrix");
            glUniformMatrix4fv(mesh_modelMatrixLocation, 1, GL_FALSE, &mesh_modelMatrix[0][0]);
            glDrawArrays(currentRenderMode, 0, 36);
        }
        // The horizontal mesh
        for (int j = 0; j < 8; j++)
        {
            mat4 mesh_scaleMatrix = scale(IDENTITY_MATRIX, vec3(0.015f, 0.015f, 1.7f));
            mat4 mesh_rotationMatrix = rotate(IDENTITY_MATRIX, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
            mat4 mesh_translationMatrix = translate(IDENTITY_MATRIX, vec3(0.0f, 3.05f, -0.60f + (j/6.0f)));
            // Model matrix of the racket handle
            mat4 mesh_modelMatrix = racketHandle_groupMatrix *
                                    mesh_translationMatrix *
                                    mesh_rotationMatrix *
                                    mesh_scaleMatrix *
                                    initialCubeTranslate;
            colorLocation = glGetUniformLocation(shaderProgram, "myColor");
            glUniform3fv(colorLocation, 1, &colorBlack[0]);
            GLuint mesh_modelMatrixLocation = glGetUniformLocation(shaderProgram, "modelMatrix");
            glUniformMatrix4fv(mesh_modelMatrixLocation, 1, GL_FALSE, &mesh_modelMatrix[0][0]);
            glDrawArrays(currentRenderMode, 0, 36);
        }
    }

    // ********************************** SHPERE *******************************************
    mat4 sphere_scaleMatrix = scale(IDENTITY_MATRIX, vec3(1.0f, 1.0f, 1.0f));
    mat4 sphere_rotationMatrix = rotate(IDENTITY_MATRIX, radians(0.0f), vec3(0.0f, 1.0f, 0.0f));
    mat4 sphere_translationMatrix = translate(IDENTITY_MATRIX, vec3(23.0f, 5.0f, -10.5f));
    mat4 sphere_MVP = sphere_translationMatrix * sphere_scaleMatrix * sphere_rotationMatrix;
    setColorUniform(shaderProgram, colorWhite);
    setModelMatrix(shaderProgram, sphere_MVP);

    setMaterial(shaderProgram, vec3(1.0f), vec3(1.0f), vec3(0.1, 0.1, 0.1), 2.0f);

    sphere = createSphere(resolution, .3f);
    glBindVertexArray(sphere);
    glBindTexture(GL_TEXTURE_2D, ballID);
    setColorUniform(shaderProgram, colorLightBlue);
    glDrawElements(GL_TRIANGLES, vertexCount ,GL_UNSIGNED_INT,(void*)0);

    tennisBall.Draw(shaderProgram, glGetUniformLocation(shaderProgram, "modelMatrix"));
    
}

void drawSkyCube(int shaderProgram){
    //                             ************* RENDER THE BOX outside the passes ************* 
    // matrix that translates the initial cube upwards by 0.5
    mat4 initialCubeTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, 0.5f, 0.0f));
    GLuint initialCubeTranslateLocation = glGetUniformLocation(shaderProgram, "modelMatrix");
    glUniformMatrix4fv(initialCubeTranslateLocation, 1, GL_FALSE, &initialCubeTranslate[0][0]);

    // change the VAO to the box VAO with reveresed surfaces
    setMaterial(shaderProgram, vec3(1.0f, 1.0, 1.0f), vec3(1.0f), vec3(0.2f, 0.2, 0.2f), 1);
    glBindTexture(GL_TEXTURE_2D, skyID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glBindVertexArray(skyBox_VAO_inside);
    // change the color of in the shader
    setColorUniform(shaderProgram, colorBeige);


    // MVP matrices to create the model matrix of the BOX
    mat4 BOX_scaleMatrix = scale(IDENTITY_MATRIX, vec3(87.0f, 30.0f, 45.0f));
    mat4 BOX_rotationMatrix = rotate(IDENTITY_MATRIX, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
    mat4 BOX_translationMatrix = translate(IDENTITY_MATRIX, vec3(0.0f, -0.3, 0.0f));
    mat4 BOX_modelMatrix = BOX_translationMatrix *
                        BOX_rotationMatrix *
                        BOX_scaleMatrix *
                        initialCubeTranslate;
    // update the shader
    setModelMatrix(shaderProgram, BOX_modelMatrix);
    glDrawArrays(currentRenderMode, 0, 36);
}

int main(int argc, char *argv[])
{
    // Initialize GLFW and OpenGL version
    if (!glfwInit())
    {
        std::cerr << "Error: could not start GLFW3\n";
        return 1;
    }

    // Hints to the GPU
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create Window and rendering context using GLFW, resolution is 800x600
    GLFWwindow *window = glfwCreateWindow(1024, 768, "Comp371 - project", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Apply to the context that is current.
    glfwMakeContextCurrent(window);

    // Initialize GLEW extension handler
    glewExperimental = true; // Needed for core profile (latest version of OpenGl)
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // set the background color
    glClearColor(0.248f, 0.333f, 0.337f, 1.0f);

    // Compile and link shaders
    int shaderProgram = shader("../shaders/vertexShaderSource.glsl", "../shaders/fragmentShaderSource.glsl");
    int depthShaderProgram = shader("../shaders/depthVertexShaderSource.glsl", "../shaders/depthFragmentShaderSource.glsl");

    // *** Creating the VAOs ***
    baseCube_VAO = create_modelCube_VAO();

    // skybox with inverted direction
    skyBox_VAO_inside = create_modelCube_VAO_skyBox();

    // viewMatrix components
    mat4 viewMatrix(1.0f);
    vec3 cameraPosition = vec3(40.0f, 20.0f, 0.0f);
    vec3 lookAtPoint = vec3(0.0f, 0.0f, 0.0f);
    vec3 upVector = vec3(0.0f, 1.0f, 0.0f);

    // EBOs for the rendering modes
    int currentRenderMode = GL_TRIANGLES;
    int lastSpaceBarState = GLFW_RELEASE;
    int lastAState = GLFW_RELEASE;

    // For frame time
    float lastFrameTime = glfwGetTime();
    int lastMouseLeftState = GLFW_RELEASE;

    // speed parameters
    float objectSpeed = 10.0f;
    float rotationAngle = 0.0f;
    vec3 translateModelVector(1.0f);

    mat4 projectionMatrix(1.0f);
    // Enable Backface culling
    glEnable(GL_CULL_FACE);

    // Enable Depth Testing
    glEnable(GL_DEPTH_TEST);

    // matrix that translates the initial cube upwards by 0.5
    mat4 initialCubeTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, 0.5f, 0.0f));
    GLuint initialCubeTranslateLocation = glGetUniformLocation(shaderProgram, "modelMatrix");
    glUniformMatrix4fv(initialCubeTranslateLocation, 1, GL_FALSE, &initialCubeTranslate[0][0]);

    #pragma endregion
    
    vec3 lightPos = vec3(25.0f, 30.0f, -5.0f);
    GLuint lightLocation = glGetUniformLocation(shaderProgram, "lightPos");
    float lightRotationSpeed = 25.0f;

    // texture
    int useTexture = 1;
    setUseTexture(shaderProgram, 1);
    int lastTstate = GLFW_RELEASE;

    // load textures
    brickID = loadTexture("../assets/textures/brick.jpg");
    skyID = loadTexture("../assets/textures/sky.jpg");
    cementID = loadTexture("../assets/textures/cement.jpg");
    glossyID = loadTexture("../assets/textures/glossy.jpg");
    woodID = loadTexture("../assets/textures/wood.jpg");
    fabricID = loadTexture("../assets/textures/fabric.jpg");
    metalID = loadTexture("../assets/textures/metal.jpg");
    tennisID = loadTexture("../assets/textures/court1.jpg");
    ballID = loadTexture("../assets/textures/tennis2.jpg");
    
    // SET THE LIGHT COMPONENTS TO STARTING VALUES
    vec3 setAmbient = vec3(1.0, 1.0, 1.0);
    vec3 setDiffuse = vec3(1.0, 1.0, 1.0);
    vec3 setSpecular = vec3(1.0, 1.0, 1.0);
    float shine = 1.0f;
    setMaterial(shaderProgram, setAmbient, setDiffuse, setSpecular, shine);

    // ******************* SHADOWS ********************
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

    // CREATE THE DEPTH TEXTURE
    GLuint depthTexture;
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    int shadowWidth = 4096;
    int shadowHeight = 4096;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0); // the zero here assigned the shadowmap to the 0 texture index

    glDrawBuffer(GL_NONE);

    // update the size of the buffer based on the screen size
    GLint frameBufferWidth, frameBufferHeight;
    glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);

    GLuint light_MVP_MatrixLocation = glGetUniformLocation(shaderProgram, "light_MVP_Matrix");
    GLuint light_MVP_Depth_MatrixLocation = glGetUniformLocation(depthShaderProgram, "light_MVP_Matrix");

    int selectCamera = 0;
    int lastMState = GLFW_RELEASE;

    vec3 rABCameraPosition = vec3(0.0f, 7, -15.0f);
    vec3 rIBCameraPosition = vec3(0.0f, 7, 15.0f);

    tennisBall = TennisBall(1.f, vec3(0.f, 8.f, 0.f), vec3(5.f, 6.f, 0.f), vec3(0.f, -10.f, 0.f), 20.f);

    // Plane(GLfloat pWidth, GLfloat pHeight, vec3 pNormal, vec3 pUpTiltVector, vec3 pPosition, const char * pPlaneName)
    Plane groundPlane(100, 100, MY_UP, MY_LEFT, vec3(0.f), "Ground");
    // Plane netPlane(100, 100, MY_RIGHT, MY_UP, vec3(0.f), "Tennis Net");
    Plane backCourtPlane(100, 100, MY_LEFT, MY_UP, vec3(37.f, 0.f, 0.f), "backCourt");
    Plane frontCourtPlane(100, 100, MY_RIGHT, MY_UP, vec3(-37.f, 0.f, 0.f), "frontCourtPlane");
    Plane rightCourtPlane(100, 100, MY_FORWARD, MY_UP, vec3(0.f, 0.f, -22.5f), "rightCourtPlane");
    Plane leftCourtPlane(100, 100, MY_BACKWARD, MY_UP, vec3(0.f, 0.f, 22.5f), "leftCourtPlane");

    // racket 1 wasd
    Plane racket1Plane(100, 100, normals[0], MY_UP, centers[0], "racket1Plane");
    // racket 2 arrows
    Plane racket2Plane(4, 4, normals[1], MY_UP, centers[1], "racket2Plane");

    
    tennisBall.AddCollidingPlane(&groundPlane);
    tennisBall.AddCollidingPlane(&racket1Plane);
    tennisBall.AddCollidingPlane(&racket2Plane);
    // tennisBall.AddCollidingPlane(&netPlane);
    tennisBall.AddCollidingPlane(&backCourtPlane);
    tennisBall.AddCollidingPlane(&frontCourtPlane);
    tennisBall.AddCollidingPlane(&rightCourtPlane);
    tennisBall.AddCollidingPlane(&leftCourtPlane);


    // Entering Main Loop
    while (!glfwWindowShouldClose(window))
    {
        // Frame time calculation
        float dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;


        // ***************** SHADOW *****************
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glViewport(0, 0, shadowWidth, shadowHeight);

        // shadowMap buffer clear
        glClear(GL_DEPTH_BUFFER_BIT);

        // FIRST PASS (using depth shader program)
        // ---------------------------------------
        glUseProgram(depthShaderProgram);

        // light doesn't need model matrix, it's already translated to world.
        mat4 lightViewMatrix = lookAt(lightPos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
        mat4 lightProjectionMatrix = ortho(-80.0f, 80.0f, -80.0f, 80.0f, 0.01f, 600.0f);
        mat4 light_MVP_Matrix = lightProjectionMatrix * lightViewMatrix;

        glUniformMatrix4fv(light_MVP_Depth_MatrixLocation, 1, GL_FALSE, &light_MVP_Matrix[0][0]);

        drawScene(depthShaderProgram, elbow, wrist);

        // SECOND PASS (using normal shader program)
        // -----------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // set the shader program
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(light_MVP_MatrixLocation, 1, GL_FALSE, &light_MVP_Matrix[0][0]);
        glViewport(0, 0, frameBufferWidth, frameBufferHeight);

        // Each frame, reset color of each pixel to glClearColor specified before
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // __________updating the light position everyframe_______________
        #pragma region
        // vec4 lightPosV4 = vec4(lightPos, 1.0f);
        // // rotation of the light source around axis (1,1,1)
        // lightPosV4 = (rotate(IDENTITY_MATRIX, radians(lightRotationSpeed * dt), vec3(0.0f, 1.0f, 0.0f)) * lightPosV4);
        // lightPos = vec3(lightPosV4);
        // glUniform3fv(lightLocation, 1, &lightPos[0]);
        // ---------------------------------------------------------------
        #pragma endregion

        drawScene(shaderProgram, elbow, wrist);

        racket1Plane.UpdatePhysics(centers[0], normals[0], racket1Plane.GetUpTiltVector(),dt);
        racket2Plane.UpdatePhysics(centers[1], normals[1], racket2Plane.GetUpTiltVector(),dt);
        tennisBall.UpdatePhysics(dt);


        drawSkyCube(shaderProgram);
        
        setLightPos(shaderProgram, lightPos);

        // update the texture shader program with the shadowmap with 0
        setShadowMap(shaderProgram, 1);

        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, depthTexture);


        // set the shader program
        glUseProgram(shaderProgram);

        // set the view matrix using the parameters needed
        viewMatrix = lookAt(cameraPosition, lookAtPoint, upVector);
        setViewMatrix(shaderProgram, viewMatrix);
        // set the projection matrix based on the parameter needed
        projectionMatrix = perspective(radians(60.0f), 1024.0f / 768.0f, 0.01f, 200.0f);
        setProjectionMatrix(shaderProgram, projectionMatrix);


        // if i'm using only one type of textures I can just activate one texture
        glActiveTexture(GL_TEXTURE0);
        GLuint textureLocation = glGetUniformLocation(shaderProgram, "textureSampler");
        glUniform1i(textureLocation, 0);

        // render the whole scene (net, grid, axis, two rackets and letters, skybox)
        drawScene(shaderProgram, elbow, wrist);


        // End frame
        glfwSwapBuffers(window);

        // Detect inputs
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
        {
            vec4 cameraPos4 = vec4(cameraPosition, 1);
            mat4 translateLookAtPointTo_0 = translate(IDENTITY_MATRIX, -lookAtPoint);
            cameraPos4 = rotate(IDENTITY_MATRIX, radians(-0.6f), vec3(1.0f, 0.0f, 0.0f)) * translateLookAtPointTo_0 * cameraPos4;
            cameraPos4 = inverse(translateLookAtPointTo_0) * cameraPos4;
            cameraPosition = vec3(cameraPos4);
        }
        if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
        {
            vec4 cameraPos4 = vec4(cameraPosition, 1);
            mat4 translateLookAtPointTo_0 = translate(IDENTITY_MATRIX, -lookAtPoint);
            cameraPos4 = rotate(IDENTITY_MATRIX, radians(0.6f), vec3(1.0f, 0.0f, 0.0f)) * translateLookAtPointTo_0 * cameraPos4;
            cameraPos4 = inverse(translateLookAtPointTo_0) * cameraPos4;
            cameraPosition = vec3(cameraPos4);
        }

        // Handle inputs
        // exit the window
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        // resets the position of the racket after moving it
        if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
        {
            translationMatrixArray[0] = translate(IDENTITY_MATRIX, vec3(-25.0f, 0.5, 8.0f));
            translationMatrixArray[1] = translate(IDENTITY_MATRIX, vec3(25.0f, 0.5, -8.0f));
        }

        // first racket movement (WASD)
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            translateModelVector = MY_BACKWARD * (objectSpeed * dt);
            translationMatrixArray[0] = translationMatrixArray[0] * translate(IDENTITY_MATRIX, translateModelVector);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            translateModelVector = MY_FORWARD * (objectSpeed * dt);
            translationMatrixArray[0] = translationMatrixArray[0] * translate(IDENTITY_MATRIX, translateModelVector);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            translateModelVector = MY_RIGHT * (objectSpeed * dt);
            translationMatrixArray[0] = translationMatrixArray[0] * translate(IDENTITY_MATRIX, translateModelVector);
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            translateModelVector = MY_LEFT * (objectSpeed * dt);
            translationMatrixArray[0] = translationMatrixArray[0] * translate(IDENTITY_MATRIX, translateModelVector);
        }

        // second racket movement (ARROWS)
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            translateModelVector = MY_BACKWARD * (objectSpeed * dt);
            translationMatrixArray[1] = translationMatrixArray[1] * translate(IDENTITY_MATRIX, translateModelVector);
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            translateModelVector = MY_FORWARD * (objectSpeed * dt);
            translationMatrixArray[1] = translationMatrixArray[1] * translate(IDENTITY_MATRIX, translateModelVector);
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            translateModelVector = MY_RIGHT * (objectSpeed * dt);
            translationMatrixArray[1] = translationMatrixArray[1] * translate(IDENTITY_MATRIX, translateModelVector);
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            translateModelVector = MY_LEFT * (objectSpeed * dt);
            translationMatrixArray[1] = translationMatrixArray[1] * translate(IDENTITY_MATRIX, translateModelVector);
        }
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && lastTstate == GLFW_RELEASE)
        {
            if (useTexture == 1)
            {
                setUseTexture(shaderProgram, 0);
                useTexture = 0;
            }
            else if (useTexture == 0)
            {
                setUseTexture(shaderProgram, 1);
                useTexture = 1;
            }
        }

        //constant factors for the racket movements.
        const float backwardShouler1 = radians(-20.0f);
        const float backwardShouler2 = radians(20.0f);

        const float minElbowFlex1 = radians(-15.0f); 
        const float minElbowFlex2 = radians(-15.0f); 

        const float maxCurl1 = radians(35.0f);  
        const float maxCurl2 = -radians(35.0f);  

        const float maxShoulder1 = radians(30.0f);
        const float maxShoulder2 = -radians(50.0f);

        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS ) 
        {   
            if(totalElbowRotationbackwards1 > backwardShouler1){
                rotationMatrixArray[1] = rotate(rotationMatrixArray[1], -radians(0.7f), vec3(0.0f, 0.0f, 1.0f));
                totalElbowRotationbackwards1 -=  radians(0.7f);
            }else {
                if(totalElbowRotation1 > minElbowFlex1 ){
                    elbow[1] = rotate(elbow[1], radians(0.3f), vec3(1.0f, 0.0f, 0.0f)); 
                    totalElbowRotation1 -= radians(0.3f);
                }
                if(totalShoulderRotation1 < maxShoulder1){
                    rotationMatrixArray[1] = rotate(rotationMatrixArray[1], radians(1.2f), vec3(0.0f, 0.0f, 1.0f));
                    totalShoulderRotation1 +=  radians(1.2f);
                }
                if(totalWristRotation1 < maxCurl1){
                    wrist[1] = rotate(wrist[1], radians(0.6f), vec3(0.0f, 0.0f, 1.0f)); 
                    totalWristRotation1 += radians(0.6f);
                }
            }
        } else if(glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE){
            elbow[1] = IDENTITY_MATRIX;
            totalElbowRotation1 = 0;
            rotationMatrixArray[1] = rotate(IDENTITY_MATRIX, radians(-30.0f), vec3(1.0f, 0.0f, 0.0f));
            totalShoulderRotation1 = 0;
            wrist[1] = rotate(IDENTITY_MATRIX, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
            totalWristRotation1 = 0;
            totalElbowRotationbackwards1 = 0.0f;
        }

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS ) 
        {
            if(totalElbowRotationbackwards2 < backwardShouler2){
                rotationMatrixArray[0] = rotate(rotationMatrixArray[0], +radians(0.7f), vec3(0.0f, 0.0f, 1.0f));
                totalElbowRotationbackwards2 +=  radians(0.7f);
            } else {
                if(totalElbowRotation2 > minElbowFlex2 ){
                    elbow[0] = rotate(elbow[0], radians(0.5f), vec3(1.0f, 0.0f, 0.0f)); 
                    totalElbowRotation2 -= radians(0.5f);
                }
                if(totalShoulderRotation2 > maxShoulder2){
                    rotationMatrixArray[0] = rotate(rotationMatrixArray[0], -radians(1.2f), vec3(0.0f, 0.0f, 1.0f));
                    totalShoulderRotation2 -=  radians(1.2f);
                }
                if(totalWristRotation2 < maxCurl2){
                    wrist[0] = rotate(wrist[0], -radians(0.6f), vec3(0.0f, 0.0f, 1.0f)); 
                    totalWristRotation2 -= radians(0.6f);
                }
            }
        } else if(glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE){
            elbow[0] = IDENTITY_MATRIX;
            totalElbowRotation2 = 0.0f;
            rotationMatrixArray[0] = rotate(IDENTITY_MATRIX, radians(-30.0f), vec3(1.0f, 0.0f, 0.0f));
            totalShoulderRotation2 = 0.0f;
            wrist[0] = rotate(IDENTITY_MATRIX, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
            totalWristRotation2 = 0.0f;
            totalElbowRotationbackwards2 = 0.0f;
        }

        //                                 *** Update variables at the end of the loop ***
        
        lastTstate = glfwGetKey(window, GLFW_KEY_T);
        if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS){
            cameraPosition = vec3(-40.0f, 20.0f, 0.0f);
        }
        if(glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS){
            cameraPosition = vec3(40.0f, 20.0f, 0.0f);
        }

        // update the space bar status.
        lastSpaceBarState = glfwGetKey(window, GLFW_KEY_SPACE);
        lastAState = glfwGetKey(window, GLFW_KEY_A);
    }

    // unbind VAO
    glBindVertexArray(0);

    // Shutdown GLFW
    glfwTerminate();

    return 0;
}
