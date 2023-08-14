#ifndef TEXTURE_H
#define TEXTURE_H
#include <GL/glew.h>
class Texture {
    public:
    static Texture * GetClayTexture();
    static Texture * GetGlossyTexture();
    static Texture * GetTennisBallTexture();
    static Texture * GetArmTexture();
    static Texture * GetSilverMetalTexture();
    static Texture * GetTennisStrapTexture();
    static Texture * GetWoodTexture();
    static Texture * GetCementTexture();
    static Texture * GetTattooTexture();
    static Texture * GetSkyTexture();
    static Texture * GetGrassTexture();
    static Texture * GetWallTexture();
    static Texture * GetTennisCourtTexture();

    Texture();
    Texture(const char * pFileLocation);
    ~Texture();

    void LoadTexture();
    void UseTexture();
    void ClearTexture();
    private:
    void ClearAttributes();
    GLuint aTextureID;
    int aWidth, aHeight, aBitDepth;
    const char * aFileLocation;
};
#endif