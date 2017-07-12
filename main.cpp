#include <iostream>
#include <opencv2/opencv.hpp>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <QApplication>
#include <QImage>
#include <QPainter>
#include <QLabel>

#include "Volume.h"
#include "VolumeRender.h"

using namespace std;
using namespace glm;

int main(int argc, char **argv) {
    string filename = "assets/volume/engine.vifo";

    QApplication app(argc, argv);

    VolumeRender render;
    render.updateVolume(filename);

    render.updateImage();

    render.show();

    return app.exec();
}