#ifndef RAYTRACING_VOLUME_H
#define RAYTRACING_VOLUME_H

#include <iostream>
#include "utils.h"

class Volume {

public:
    Volume(){};

    bool loadRawData(const char *filename);

    int getPow2(int size);

    unsigned char *data;

    //the size of volume in three dimensions
    int xiSize, yiSize, ziSize;

    double xSpace, ySpace, zSpace;

    double xfSize, yfSize, zfSize;

    double xpSize, ypSize, zpSize;
};


#endif //RAYTRACING_VOLUME_H
