#include "Plane.h"

Plane::Plane(GLfloat pWidth, GLfloat pHeight, vec3 pNormal, vec3 pCenterPosition) {
    aWidth = pWidth;
    aHeight = pHeight;
    aWidth = pWidth;
    aNormal = normalize(pNormal);
    aCenterPosition  = pCenterPosition;
}

vec3 Plane::GetNormal() {
    return aNormal;
}
GLfloat Plane::GetWidth() {
    return aWidth;
}
GLfloat Plane::GetHeight() {
    return aHeight;
}
vec3 Plane::GetCenterPosition() {
    return aCenterPosition;
}
vec3 Plane::GetEdgeDirection() {
    return aEdgeDirection;
}