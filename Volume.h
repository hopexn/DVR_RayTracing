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
    Volume() {
        center = vec3(0.0f, 0.0f, 0.0f);
    };

    bool loadRawData(const char *filename);

    float getVolumeValue(vec3 &pos);

    TF1D tf1d;

    float step_dist;

    vec3 center;

private:

    int xiSize, yiSize, ziSize;

    float xSpace, ySpace, zSpace;

    float xfSize, yfSize, zfSize;

    unsigned char *data = NULL;

};


#endif //RAYTRACING_VOLUME_H
