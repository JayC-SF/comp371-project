#version 330 core
 in vec3 vertexColor;
 // in light caluclation vectors
 in vec3 normal;
 in vec3 lightDir;
 in vec3 eyeDir;
 in vec3 pos;
 in vec2 vertexUV;
 // fragment position in light 
 in vec4 fragPos_inLight;
 // texture uniforms
 uniform int useTexture;
 uniform sampler2D textureSampler;
 // material control 
 uniform vec3 setAmbient;
 uniform vec3 setDiffuse;
 uniform vec3 setSpecular;
 uniform float shine;
 //shadow sampler
 uniform sampler2D shadowMap;
 // spot light calculations
 uniform vec3 spotLPos;
 uniform vec3 spotLlookAtPos;
 uniform float spotLIntensity;
 // fragment color
 out vec4 FragColor;

 void main()
{
   // texture calculations
   vec4 textureColor = texture(textureSampler, vertexUV);
   // ********** light calculations **********
   // light properties
   vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
   float lightPower = 3.0f;
   float dist = length(lightDir);
   float attenuation = 1/((0.032 * dist * dist) +  (0.09 * dist) + 1);
   // vector normalization for light calculations
   vec3 n = normalize(normal);
   vec3 l = normalize(lightDir);
   // components of ambient light
   vec3 ambientFactor = setAmbient;
   // components of diffusion light
   float cosTheta = clamp (dot(n,l), 0, 1);
   vec3 diffuseFactor = setDiffuse;
   // components of the specular light
   float shininess = shine;
   vec3 specularFactor = setSpecular;
   vec3 E = normalize(eyeDir);
   vec3 R = normalize(reflect(-l, n));
   float cosThetaSpecular = clamp(dot(E, R), 0, 1);
   // ************ end of light calculations ************

   // spot light calculations
    vec3 spotDir = spotLlookAtPos - spotLPos;
    vec3 fragDir = pos - spotLPos;
    float fragCos = dot(normalize(spotDir), normalize(fragDir));
    float cutoffCos = cos(radians(30.0f));
   // final light calculations
   // phong components
   vec3 ambientColor;
   vec3 diffuseColor;
   vec3 specularColor;
   // shadow calculations
   // transform from DNC to 0 --> 1
   vec3 shadowTextureCoord = fragPos_inLight.xyz * 0.5 + 0.5;
   float worldDepth = shadowTextureCoord.z;
   float mappedDepth = texture(shadowMap, shadowTextureCoord.xy).r;
   float shadow = 1.0f;
   float bias = 0.0005f;
    if (worldDepth - bias > mappedDepth){
    shadow = 0.4f;
    }
   // conditional definition of the fragment color
   if (useTexture == 1)
   {
       if(textureColor.a < 0.1){
         discard;
       }
       ambientColor = vec3(textureColor) * ambientFactor;
       diffuseColor = vec3(textureColor) * diffuseFactor * lightColor * lightPower * cosTheta * attenuation;
   }
   else if (useTexture == 0)
   {
      ambientColor = vertexColor * ambientFactor;
      diffuseColor = vertexColor * diffuseFactor * lightColor * lightPower * cosTheta * attenuation;
   }
   specularColor = specularFactor * vertexColor * lightPower * lightColor * pow(cosThetaSpecular, shininess) * attenuation;
   FragColor = shadow * vec4(ambientColor + diffuseColor + specularColor, 1.0f);
 }