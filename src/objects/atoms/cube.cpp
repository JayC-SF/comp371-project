#include "cube.h"
#include "atom.h"
#define U 1
#define X_DIR vec3(1.f, 0, 0)
#define Y_DIR vec3(0, 1.f, 0)
#define Z_DIR vec3(0, 0, 1.f)

using namespace glm;
using namespace std;


static vector<vec3> cubeVertices = {
    // front face 
    vec3(-U/2., -U/2., U/2.), vec3(U/2., -U/2., U/2.), vec3(U/2., U/2., U/2.),
    vec3(U/2., U/2., U/2.), vec3(-U/2., U/2., U/2.), vec3(-U/2., -U/2., U/2.),
    // back face
    vec3(U/2., -U/2., -U/2.), vec3(-U/2., -U/2., -U/2.), vec3(-U/2., U/2., -U/2.),
    vec3(-U/2., U/2., -U/2.), vec3(U/2., U/2., -U/2.), vec3(U/2., -U/2., -U/2.),
    // right face
    vec3(U/2., -U/2., U/2.), vec3(U/2., -U/2., -U/2.), vec3(U/2., U/2., -U/2.),
    vec3(U/2., U/2., -U/2.), vec3(U/2., U/2., U/2.), vec3(U/2., -U/2., U/2.),
    // left face
    vec3(-U/2., -U/2., -U/2.), vec3(-U/2., -U/2., U/2.), vec3(-U/2., U/2., U/2.),
    vec3(-U/2., U/2., U/2.), vec3(-U/2., U/2., -U/2.), vec3(-U/2., -U/2., -U/2.),
    // top face
    vec3(-U/2., U/2., U/2.), vec3(U/2., U/2., U/2.), vec3(U/2., U/2., -U/2.),
    vec3(U/2., U/2., -U/2.), vec3(-U/2., U/2., -U/2.), vec3(-U/2., U/2., U/2.),
    // bottom face
    vec3(-U/2., -U/2., -U/2.), vec3(U/2., -U/2., -U/2.), vec3(U/2., -U/2., U/2.),
    vec3(U/2., -U/2., U/2.), vec3(-U/2., -U/2., U/2.), vec3(-U/2., -U/2., -U/2.),
};

static  vector<GLuint> cubeLineIndices = {
    // front face
    0, 1,
    1, 2,
    3, 4,
    4, 5,

    // back face
    6, 7, 
    7, 8,
    9, 10,
    10, 11,

    // side lines
    0, 7,
    1, 6, 
    3, 10,
    4, 9
};

// to be initialized once
static vector<GLuint> cubePointAndTriangleIndices;
static vector<GLuint> cubeTriangleInvert;

static vector<vec3> normals = {
    // front face
    Z_DIR, Z_DIR, Z_DIR, Z_DIR, Z_DIR, Z_DIR,
    // back face
    -Z_DIR, -Z_DIR, -Z_DIR, -Z_DIR, -Z_DIR, -Z_DIR,
    // right face
    X_DIR, X_DIR, X_DIR, X_DIR, X_DIR, X_DIR,
    // left face
    -X_DIR, -X_DIR, -X_DIR, -X_DIR, -X_DIR, -X_DIR,
    // top face
    Y_DIR, Y_DIR, Y_DIR, Y_DIR, Y_DIR, Y_DIR,
    // bottom face
    -Y_DIR, -Y_DIR, -Y_DIR, -Y_DIR, -Y_DIR, -Y_DIR,
};

static bool areStaticDataInit = false;

// create a vector of 6 faces with 6 vertex per face
vector<vec2> UV(6*6);

static void setCubeUV(GLfloat pUVMaxCoords, GLuint face, GLuint direction) {
    GLuint i = face*6;
    if (direction == CUBE_Y_DIR || direction == CUBE_XY_DIR) {
        // top right
        UV[i + 2].y = pUVMaxCoords;
        // top right
        UV[i + 3].y =pUVMaxCoords;
        // top left
        UV[i + 4].y = pUVMaxCoords;
    }
    if (direction == CUBE_X_DIR || direction == CUBE_XY_DIR) {
        // bottom right
        UV[i + 1].x = pUVMaxCoords;
        // top right
        UV[i + 2].x = pUVMaxCoords;
        // top right
        UV[i + 3].x = pUVMaxCoords;
    }
}



static void initStaticData() { 
    if (areStaticDataInit)
        return;
    for (int i = 0; i<cubeVertices.size(); i++)
        cubePointAndTriangleIndices.push_back(i);
    
    // set all faces to be scaled properly
    for (int i = 0; i<CUBE_BOTTOM_FACE; i++)
        setCubeUV(1.f, i, CUBE_XY_DIR);
    
    for (int i = 0; i<cubePointAndTriangleIndices.size(); i += 3) {
        cubeTriangleInvert.push_back(cubePointAndTriangleIndices[i+2]);
        cubeTriangleInvert.push_back(cubePointAndTriangleIndices[i+1]);
        cubeTriangleInvert.push_back(cubePointAndTriangleIndices[i]);
    }
    
    areStaticDataInit = true;
}

// Constructor function of the Cube object.
// initialize the aCurrentEBO reference to default EBO.
Cube::Cube() : Atom(){
    // init missing indices
    initStaticData();
    // UV locations for texture mapping.
    
    aVAO.Bind();
    // init and bind vertex vbo
    aVBOVertex.Init(cubeVertices.size()*sizeof(vec3), &cubeVertices[0][0], GL_STATIC_DRAW);
    aVAO.VAttribPointer(aVBOVertex, 0, 3, GL_FLOAT, sizeof(vec3), NULL);

    // init and bind uv vbo
    aVBOUV.Init(UV.size()*sizeof(vec2), &UV[0][0], GL_STATIC_DRAW);
    aVAO.VAttribPointer(aVBOUV, 1, 2, GL_FLOAT, sizeof(vec2), NULL);

    // init and bind normal vbo
    aVBONormals.Init(normals.size()*sizeof(vec3), &normals[0][0], GL_STATIC_DRAW);
    aVAO.VAttribPointer(aVBOVertex, 2, 3, GL_FLOAT, sizeof(vec3), NULL);
    
    // default EBO.
    aEBO.Init(cubePointAndTriangleIndices.size(), &cubePointAndTriangleIndices[0], GL_STATIC_DRAW, GL_TRIANGLES);
    // lines EBO
    aEBOLines.Init(cubeLineIndices.size(), &cubeLineIndices[0], GL_STATIC_DRAW, GL_LINES);
    // points EBO
    aEBOPoints.Init(cubePointAndTriangleIndices.size(), &cubePointAndTriangleIndices[0], GL_STATIC_DRAW, GL_POINTS);
    
    aEBOInvert.Init(cubeTriangleInvert.size(), &cubeTriangleInvert[0], GL_STATIC_DRAW, GL_TRIANGLES);

    aVAO.Unbind();
}

void Cube::SetLineDrawingMode() {
    aCurrentEBO = &aEBOLines;
}
void Cube::SetTriangleDrawingMode() {
    aCurrentEBO = &aEBO;
}
void Cube::SetPointDrawingMode() {
    aCurrentEBO = &aEBOPoints;
}

void Cube::SetInvertDrawingMode() {
    aCurrentEBO = &aEBOInvert;
}


void Cube::SetRepeatUVCoord(GLfloat pUVMaxCoords, GLuint face, GLuint direction) {
    setCubeUV(pUVMaxCoords, face, direction);
    aVAO.Bind();
    aVBOUV.SubBufferData(UV.size()*sizeof(vec2), &UV[0][0]);
}
void Cube::SetRepeatUVCoord(vec3 pDims) {
    SetRepeatUVCoord(pDims.y, CUBE_FRONT_FACE, CUBE_Y_DIR);
    SetRepeatUVCoord(pDims.y, CUBE_BACK_FACE, CUBE_Y_DIR);
    SetRepeatUVCoord(pDims.y, CUBE_RIGHT_FACE, CUBE_Y_DIR);
    SetRepeatUVCoord(pDims.y, CUBE_LEFT_FACE, CUBE_Y_DIR);
    
    SetRepeatUVCoord(pDims.x, CUBE_FRONT_FACE, CUBE_X_DIR);
    SetRepeatUVCoord(pDims.x, CUBE_BACK_FACE, CUBE_X_DIR);
    SetRepeatUVCoord(pDims.x, CUBE_TOP_FACE, CUBE_X_DIR);
    SetRepeatUVCoord(pDims.x, CUBE_BOTTOM_FACE, CUBE_X_DIR);
    
    SetRepeatUVCoord(pDims.z, CUBE_TOP_FACE, CUBE_Y_DIR);
    SetRepeatUVCoord(pDims.z, CUBE_BOTTOM_FACE, CUBE_Y_DIR);
    SetRepeatUVCoord(pDims.z, CUBE_RIGHT_FACE, CUBE_X_DIR);
    SetRepeatUVCoord(pDims.z, CUBE_LEFT_FACE, CUBE_X_DIR);
}

Cube * Cube::GetInstance() {
    static Cube instance = Cube();
    return &instance;
}

void Cube::ResetUVCoords() {
    // set all faces to be scaled properly
    for (int i = CUBE_FRONT_FACE; i<=CUBE_BOTTOM_FACE; i++) {
        SetRepeatUVCoord(1.f, i, CUBE_XY_DIR);
    }

}

void Cube::DrawFace(GLuint face) {
    if (aCurrentEBO->GetDrawMode() == GL_TRIANGLES) {
        aCurrentEBO->Bind();
        glDrawElements(aCurrentEBO->GetDrawMode(), aCurrentEBO->GetLength()/6, GL_UNSIGNED_INT, (void*)(6*face*(sizeof(GLuint))));
        aCurrentEBO->Unbind();
    }
}

Cube::~Cube() {}