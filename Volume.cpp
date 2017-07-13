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

    maxSize = std::max(std::max(xfSize, yfSize), zfSize);

    xfSize = xfSize / maxSize;
    yfSize = yfSize / maxSize;
    zfSize = zfSize / maxSize;
    cout << "iSize: " << xiSize << " " << yiSize << " " << ziSize << endl;
    cout << "fSize: " << xfSize << " " << yfSize << " " << zfSize << endl;

    step_dist = 3.0f / maxSize;

    string filePath(filename);
    filePath = filePath.substr(0, filePath.rfind('/') + 1);
    filePath += dataFile;

    // read volume data
    fp = fopen(filePath.c_str(), "rb");
    if (fp == NULL) {
        cout << "Can't open data file " << dataFile << endl;
        return false;
    }
    if (data == NULL) delete[] data;
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

    fread(data, sizeof(unsigned char), xiSize * yiSize * ziSize, fp);

    return true;
}



float Volume::getVolumeValue(vec3 pos) {
    if (pos.x <= -xfSize / 2 || pos.x >= xfSize / 2
        || pos.y <= -yfSize / 2 || pos.y >= yfSize / 2
        || pos.z <= -zfSize / 2 || pos.z >= zfSize / 2) {
        return 0;
    }

    int xIndex, yIndex, zIndex;
    float xFraction, yFraction, zFraction;

    pos = pos + vec3(xfSize / 2, yfSize / 2, zfSize / 2);


    xFraction = pos.x * maxSize;
    yFraction = pos.y * maxSize;
    zFraction = pos.z * maxSize;

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
