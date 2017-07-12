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

    unsigned char *data;

    //the size of volume in three dimensions
    int xiSize, yiSize, ziSize;

    double xSpace, ySpace, zSpace;

    double xfSize, yfSize, zfSize;

    TF1D tf1d;
};


#endif //RAYTRACING_VOLUME_H
