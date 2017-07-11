#include <iostream>
#include <opencv2/opencv.hpp>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/fast_square_root.hpp>
#include <QApplication>
#include <QImage>
#include <QRgb>
#include <QPainter>
#include <QRect>
#include <QLabel>

#include "Volume.h"
#include "VolumeRender.h"

using namespace std;
using namespace glm;

int main(int argc, char **argv) {
    string filename = "assets/volume/aneurism.vifo";

    QApplication app(argc, argv);

    VolumeRender render;

    render.updateVolume(filename);

    render.updateImage();

    render.show();

    return app.exec();
}