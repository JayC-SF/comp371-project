#include "sphere.h"
#include <cmath>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

using namespace glm;
using namespace std;

static GLuint index(GLuint i, GLuint j, GLuint verticesPerSlice) {
    return i*verticesPerSlice + j;    
}

void Sphere::computeIndices(GLuint pSubDivisions, GLuint verticesSize, vector<GLuint> & pIndices) {
    // number of vertices per column and per row
    GLuint numVerticesPerSidePerFace = pSubDivisions + 1;
    // total number of vertices per face
    GLuint numVerticesPerFace = numVerticesPerSidePerFace * numVerticesPerSidePerFace;
    // total of number faces, max 6
    GLuint numFaces = verticesSize/(numVerticesPerFace);

    for (GLuint i = 0; i<numFaces; i++) {
        for (int y = i*numVerticesPerSidePerFace; y < (i+1)*numVerticesPerSidePerFace - 1; y++) {
            for (int x = 0; x < numVerticesPerSidePerFace-1; x++) {
                // bottom left
                pIndices.push_back(index(y,x, numVerticesPerSidePerFace));
                // bottom right
                pIndices.push_back(index(y,x+1, numVerticesPerSidePerFace));
                // top right
                pIndices.push_back(index(y+1,x+1, numVerticesPerSidePerFace));
                // top right
                pIndices.push_back(index(y+1,x+1, numVerticesPerSidePerFace));
                // top left
                pIndices.push_back(index(y+1,x, numVerticesPerSidePerFace));
                // bottom left
                pIndices.push_back(index(y,x, numVerticesPerSidePerFace));
            }
        }
    }
}

void Sphere::computeVertices(GLuint pSubdivisionsPerFace, vector<vec3> & pVertices) {
    // initial direction
    GLfloat increments = 90.f/pSubdivisionsPerFace;
    // compute x+ face
     // latitude
    for (GLfloat theta = -45.f; theta <= 45.f; theta += increments) {
        GLfloat thetaRads = radians(theta);
        vec3 n1(-sin(thetaRads), cosf(thetaRads), 0);
        // longitude
        for (GLfloat phi = -45.f; phi <= 45.f; phi += increments) {
            float phiRads = radians(phi);
            vec3 n2(sinf(phiRads), 0, cos(phiRads));
            pVertices.push_back(normalize(cross(n1, n2)));
        }
    }
    // compute x- face
    GLuint numOfVerticesPerFace = pVertices.size();
    for (GLuint i = 0; i<numOfVerticesPerFace; i++) {
        pVertices.push_back(vec3(-pVertices[i].x, pVertices[i].y, -pVertices[i].z));
    }
    // compute y+ and y- face using vertices of x+ and x- face
    for (GLuint i = 0; i<numOfVerticesPerFace*2; i++) {
        pVertices.push_back(vec3(pVertices[i].y, -pVertices[i].z, -pVertices[i].x));
    }
    // compute z+ and z- using vertices of x+ and x- face
    for (GLuint i = 0; i<numOfVerticesPerFace*2; i++) {
        pVertices.push_back(vec3(-pVertices[i].y, pVertices[i].x, pVertices[i].z));
    }
    
}

void Sphere::computeUV(GLuint pVerticesSize, vector<vec2> & UV) {
    for (int i = 0; i<pVerticesSize; i += 6) {
        // bottom left
        UV.push_back(vec2(0,0));
        // bottom right
        UV.push_back(vec2(0,1));
        // top right
        UV.push_back(vec2(1,1));
        // top right
        UV.push_back(vec2(1,1));
        // top left
        UV.push_back(vec2(0,1));
        // bottom left
        UV.push_back(vec2(0,0));
    }
}
// creates a sphere of radius 1 with its center at origin.
Sphere::Sphere() : Atom(){
    
    GLuint subDivisions = 7;
    // compute vertices
    vector<vec3> tmpVertices;
    vector<vec3> vertices;

    computeVertices(subDivisions, tmpVertices);
    
    // compute indices
    vector<GLuint> tmpIndices;
    vector<GLuint> indices;
    computeIndices(subDivisions, tmpVertices.size(), tmpIndices);

    // we need to repeat some values for textures, therefore we recompute vertices based on indices values
    for (GLuint i = 0; i<tmpIndices.size(); i++) {
        vertices.push_back(tmpVertices[tmpIndices[i]]);
        indices.push_back(i);
    }
    // remove indices
    vector<vec2> UV;
    computeUV(vertices.size(), UV);

    // buffer setup
    aVAO.Bind();
    // set the vbo for the vertices
    aVBOVertex.Init(vertices.size()*sizeof(vec3), &vertices[0][0], GL_STATIC_DRAW);
    aVAO.VAttribPointer(aVBOVertex, 0, 3, GL_FLOAT, sizeof(vec3), 0);
    
    aVBOUV.Init(UV.size()*sizeof(vec2), &UV[0][0], GL_STATIC_DRAW);
    aVAO.VAttribPointer(aVBOUV, 3, 2, GL_FLOAT, sizeof(vec2), 0);

    // set the vbo for the normals which is simply 
    aVBONormals.Init(vertices.size()*sizeof(vec3), &vertices[0][0], GL_STATIC_DRAW);
    aVAO.VAttribPointer(aVBONormals, 2, 3, GL_FLOAT, sizeof(vec3), 0);

    aEBO.Init(indices.size(), &indices[0], GL_STATIC_DRAW, GL_TRIANGLES);
    // clean up and unbind
    aVAO.Unbind();
}

// sphere.cpp
// This function returns the single instance of the sphere if it is not initialized.
Sphere& Sphere::GetInstance() {
    static Sphere instance = Sphere();
    return instance;
}

Sphere::~Sphere() {}