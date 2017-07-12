#ifndef RAYTRACING_VOLUME_H
#define RAYTRACING_VOLUME_H

#include <iostream>
#include <glm/glm.hpp>
#include "utils.h"
#include "TF1D.h"

using namespace std;
using namespace glm;

class Volume {

public:
    Volume() {};

    bool loadRawData(const char *filename);

    float getVolumeValue(vec3 &pos);

    TF1D tf1d;

private:

    int xiSize, yiSize, ziSize;

    float xSpace, ySpace, zSpace;

    float xfSize, yfSize, zfSize;

    float xfCenter, yfCenter, zfCenter;

    unsigned char *data;

};


#endif //RAYTRACING_VOLUME_H
