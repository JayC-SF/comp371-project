#include "texture.h"
#include <cstdio>

#include <stb_image.h>

Texture::Texture() {
    ClearAttributes();
}
Texture::Texture(const char * pFileLocation) {
    ClearAttributes();
    aFileLocation = pFileLocation;
    LoadTexture();
}

void Texture::LoadTexture() {
    unsigned char * textData = stbi_load(aFileLocation, &aWidth, &aHeight, &aBitDepth, 0);
    if (!textData)
        fprintf(stderr, "FAILED TO FIND %s\n", aFileLocation);
    
    glGenTextures(1, &aTextureID);
    glBindTexture(GL_TEXTURE_2D, aTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = 0;
  if (aBitDepth == 1)
      format = GL_RED;
  else if (aBitDepth == 3)
      format = GL_RGB;
  else if (aBitDepth == 4)
      format = GL_RGBA;
    
    glTexImage2D(GL_TEXTURE_2D, 0, format, aWidth, aHeight, 0, format, GL_UNSIGNED_BYTE, textData);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // free image for future uses.
    stbi_image_free(textData);
}

void Texture::ClearAttributes() {
    aFileLocation = NULL, aTextureID = 0, aBitDepth = 0, aHeight = 0, aWidth = 0;
}
void Texture::ClearTexture() {
    glDeleteTextures(1, &aTextureID);
    ClearAttributes();
}

void Texture::UseTexture() {
    // glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, aTextureID);
}

Texture::~Texture() {
    ClearTexture();
}

Texture * Texture::GetClayTexture() {
    static Texture instance("../assets/textures/clay-tennis-court.png");
    return &instance;
}
Texture * Texture::GetGlossyTexture() {
    static Texture instance("../assets/textures/glossy.jpg");
    return &instance;
}

Texture * Texture::GetTennisBallTexture() {
    static Texture instance("../assets/textures/tennis-ball.jpeg");
    return &instance;
}

Texture * Texture::GetArmTexture() {
    static Texture instance("../assets/textures/arm.jpeg");
    return &instance;
}

Texture * Texture::GetSilverMetalTexture() {
    static Texture instance("../assets/textures/silver-metal.jpeg");
    return &instance;
}

Texture * Texture::GetTennisStrapTexture() {
    static Texture instance("../assets/textures/tennis-strap.jpg");
    return &instance;
}

Texture * Texture::GetWoodTexture() {
    static Texture instance("../assets/textures/wood.jpeg");
    return &instance;
}

Texture * Texture::GetCementTexture() {
    static Texture instance("../assets/textures/cement.jpeg");
    return &instance;
}

Texture * Texture::GetTattooTexture() {
    static Texture instance("../assets/textures/tattoo.jpeg");
    return &instance;
}

Texture * Texture::GetSkyTexture() {
    static Texture instance("../assets/textures/sky.jpeg");
    return &instance;
}