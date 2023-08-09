 #version 330 core
 layout (location = 0) in vec3 aPos;
 layout (location = 1) in vec3 aColor;
 layout (location = 2) in vec3 aNormal;
 layout (location = 3) in vec2 aUV;

 uniform mat4 modelMatrix = mat4(1.0); 
 uniform mat4 viewMatrix = mat4(1.0); 
 uniform mat4 projectionMatrix = mat4(1.0); 
 uniform vec3 myColor;
 uniform vec3 lightPos;
 uniform mat4 light_MVP_Matrix;
// vertex color
 out vec3 vertexColor;
// lighting
 out vec3 normal;
 out vec3 lightDir;
 out vec3 eyeDir;
 out vec4 fragPos_inLight;
 out vec3 pos;
// texture
 out vec2 vertexUV;

 void main()
 {
    // color and position
   vertexColor = myColor;
   mat4 MVP = projectionMatrix * viewMatrix * modelMatrix;
   gl_Position = MVP * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    // normal
   normal = (transpose(inverse(viewMatrix * modelMatrix)) * vec4(aNormal, 1)).xyz;
    // camera space vertex calculations
   vec3 vertexPosCamera = (viewMatrix * modelMatrix * vec4(aPos,1)).xyz;
   eyeDir = vec3(0,0,0) - vertexPosCamera;
   // camera space light calculations
   vec3 lightPosCamera = (viewMatrix * vec4(lightPos, 1)).xyz;
   lightDir = lightPosCamera + eyeDir;
    // texture calculations
   vertexUV = aUV;
    // position of the fragment in the light view from the world view
   pos = (modelMatrix * vec4(aPos, 1.0)).xyz;
   fragPos_inLight = light_MVP_Matrix * vec4(pos, 1.0);
}