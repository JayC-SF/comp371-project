#include "texture.h"
#include <cstdio>

#include <stb_image.h>

Textures::Textures()
{
    ClearAttributes();
}
Textures::Textures(const char *pFileLocation)
{
    ClearAttributes();
    aFileLocation = pFileLocation;
    LoadTexture();
}

void Textures::LoadTexture()
{
    unsigned char *textData = stbi_load(aFileLocation, &aWidth, &aHeight, &aBitDepth, 0);
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

void Textures::ClearAttributes()
{
    aFileLocation = NULL, aTextureID = 0, aBitDepth = 0, aHeight = 0, aWidth = 0;
}
void Textures::ClearTexture()
{
    glDeleteTextures(1, &aTextureID);
    ClearAttributes();
}

void Textures::UseTexture()
{
    glBindTexture(GL_TEXTURE_2D, aTextureID);
}

Textures::~Textures()
{
    ClearTexture();
}

Textures *Textures::GetClayTexture()
{
    static Textures instance("../assets/textures/clay-tennis-court.png");
    return &instance;
}
Textures *Textures::GetGlossyTexture()
{
    static Textures instance("../assets/textures/glossy.jpg");
    return &instance;
}

Textures *Textures::GetTennisBallTexture()
{
    static Textures instance("../assets/textures/tennis-ball.jpeg");
    return &instance;
}

Textures *Textures::GetArmTexture()
{
    static Textures instance("../assets/textures/arm.jpeg");
    return &instance;
}

Textures *Textures::GetSilverMetalTexture()
{
    static Textures instance("../assets/textures/silver-metal.jpeg");
    return &instance;
}

Textures *Textures::GetTennisStrapTexture()
{
    static Textures instance("../assets/textures/tennis-strap.jpg");
    return &instance;
}

Textures *Textures::GetWoodTexture()
{
    static Textures instance("../assets/textures/wood.jpeg");
    return &instance;
}

Textures *Textures::GetCementTexture()
{
    static Textures instance("../assets/textures/cement.jpeg");
    return &instance;
}

Textures *Textures::GetTattooTexture()
{
    static Textures instance("../assets/textures/tattoo.jpeg");
    return &instance;
}

Textures *Textures::GetSkyTexture()
{
    static Textures instance("../assets/textures/sky.jpeg");
    return &instance;
}