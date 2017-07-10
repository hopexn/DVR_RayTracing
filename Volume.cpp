#include "Volume.h"

int Volume::getPow2(int size) {
    double tmp = size / 2.0;
    int i = 2;
    while (tmp > 1.0) {
        i *= 2;
        tmp = tmp / 2.0;
    }
    return i;
}

bool Volume::loadRawData(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        cout << "Can't open file " << filename << endl;
        return false;
    }

    // read volume information
    char dataFile[1024];
    char opacityFile[1024];
    fscanf(fp, "%d %d %d\n", &xiSize, &yiSize, &ziSize);
    fscanf(fp, "%lf %lf %lf\n", &xSpace, &ySpace, &zSpace);
    fscanf(fp, "%s", dataFile);

    fclose(fp);

    // calculate the padding size and the normalized size
    xfSize = xiSize * xSpace;
    yfSize = yiSize * ySpace;
    zfSize = ziSize * zSpace;
    xpSize = getPow2(xiSize);
    ypSize = getPow2(yiSize);
    zpSize = getPow2(ziSize);
    double maxSize = max(max(xfSize, yfSize), zfSize);
    xfSize = xfSize / maxSize;
    yfSize = yfSize / maxSize;
    zfSize = zfSize / maxSize;

    string filePath(filename);
    filePath = filePath.substr(0, filePath.rfind('/') + 1);
    filePath += dataFile;

    // read volume data
    fp = fopen(filePath.c_str(), "rb");
    if (fp == NULL) {
        cout << "Can't open data file " << dataFile << endl;
        return false;
    }

    delete[] data;
    data = new unsigned char[xiSize * yiSize * ziSize];

    if (data == NULL) {
        cout << "Memory is full!" << endl;
        exit(1);
    }

    fread(data, sizeof(unsigned char), xiSize * yiSize * ziSize, fp);

    return true;
}
