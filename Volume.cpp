#include "Volume.h"

bool Volume::loadRawData(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        cout << "Can't open file :" << filename << endl;
        return false;
    }

    // read volume information
    char dataFile[1024];
    fscanf(fp, "%d %d %d\n", &xiSize, &yiSize, &ziSize);
    fscanf(fp, "%f %f %f\n", &xSpace, &ySpace, &zSpace);
    fscanf(fp, "%s", dataFile);

    fclose(fp);

    // calculate the padding size and the normalized size
    xfSize = xiSize * xSpace;
    yfSize = yiSize * ySpace;
    zfSize = ziSize * zSpace;
    float maxSize = std::max(std::max(xfSize, yfSize), zfSize);
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
    string tfFile = filePath.substr(0, filePath.rfind('.') + 1);
    tfFile += "TF1D";
    tf1d.loadTF(tfFile);

    for (int i = 0; i < tf1d.keys.size(); i++) {
        cout << tf1d.keys[i].index << ": " << tf1d.keys[i].color.r << " " << tf1d.keys[i].color.g << " "
             << tf1d.keys[i].color.b << " " << tf1d.keys[i].color.a << endl;
    }

    xfCenter = xfSize / 2;
    yfCenter = yfSize / 2;
    zfCenter = zfSize / 2;

    fread(data, sizeof(unsigned char), xiSize * yiSize * ziSize, fp);

    return true;
}

float Volume::getVolumeValue(vec3 &pos) {
    int xIndex, yIndex, zIndex;

    if (pos.x < 0 || pos.x > 1.0f || pos.y < 0 || pos.y > 1.0f || pos.z < 0 || pos.z > 1.0f) {
        return 0;
    }

    float xFraction, yFraction, zFraction;
    xFraction = (pos.x + xfSize / 2) * (xiSize - 1);
    yFraction = (pos.y + yfSize / 2) * (yiSize - 1);
    zFraction = (pos.z + zfSize / 2) * (ziSize - 1);

    xIndex = (int) xFraction;
    yIndex = (int) yFraction;
    zIndex = (int) zFraction;

    xFraction = xFraction - xIndex;
    yFraction = yFraction - yIndex;
    zFraction = zFraction - zIndex;

    int xNext = (xIndex < xiSize - 1) ? 1 : 0;
    int yNext = (yIndex < yiSize - 1) ? xiSize : 0;
    int zNext = (zIndex < ziSize - 1) ? xiSize * yiSize : 0;

    unsigned char f000, f001, f010, f011, f100, f101, f110, f111;
    int index = zIndex * xiSize * yiSize + yIndex * xiSize + xIndex;
    f000 = data[index];
    f001 = data[index + zNext];
    f010 = data[index + yNext];
    f011 = data[index + yNext + zNext];
    f100 = data[index + xNext];
    f101 = data[index + xNext + zNext];
    f110 = data[index + xNext + zNext];
    f111 = data[index + xNext + yNext + zNext];

    float value = f000 * (1 - xFraction) * (1 - yFraction) * (1 - zFraction) +
                  f001 * (1 - xFraction) * (1 - yFraction) * zFraction +
                  f010 * (1 - xFraction) * yFraction * (1 - zFraction) +
                  f011 * (1 - xFraction) * yFraction * zFraction +
                  f100 * xFraction * (1 - yFraction) * (1 - zFraction) +
                  f101 * xFraction * (1 - yFraction) * zFraction +
                  f110 * xFraction * yFraction * (1 - zFraction) +
                  f111 * xFraction * yFraction * zFraction;
    return value;
}
