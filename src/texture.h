#ifndef TEXTURE_H
#define TEXTURE_H
#include <GL/glew.h>
class Textures {
    public:
    static Textures * GetClayTexture();
    static Textures * GetGlossyTexture();
    static Textures * GetTennisBallTexture();
    static Textures * GetArmTexture();
    static Textures * GetSilverMetalTexture();
    static Textures * GetTennisStrapTexture();
    static Textures * GetWoodTexture();
    static Textures * GetCementTexture();
    static Textures * GetTattooTexture();
    static Textures * GetSkyTexture();
    static Textures * GetGrassTexture();
    static Textures * GetWallTexture();
    static Textures * GetTennisCourtTexture();

    Textures();
    Textures(const char *pFileLocation);
    ~Textures();

    void LoadTexture();
    void UseTexture();
    void ClearTexture();

private:
    void ClearAttributes();
    GLuint aTextureID;
    int aWidth, aHeight, aBitDepth;
    const char *aFileLocation;
};
#endif