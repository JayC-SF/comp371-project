#include "SoundEngine.h"
#include <iostream>

ISoundEngine* getSoundEngine() {
    static ISoundEngine* engine = createIrrKlangDevice();
    return engine;
}