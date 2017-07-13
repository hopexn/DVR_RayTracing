#include <iostream>
#include <opencv2/opencv.hpp>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <QApplication>
#include <QImage>
#include <QPainter>
#include <QLabel>
#include <omp.h>

#include "Volume.h"
#include "VolumeRender.h"

using namespace std;
using namespace glm;

int main(int argc, char **argv) {

    #pragma omp parallel
    printf("Hello from thread %d, nthreads %d\n", omp_get_thread_num(), omp_get_num_threads());

    string filename = "assets/volume/engine.vifo";

    QApplication app(argc, argv);

    VolumeRender render;
    render.updateVolume(filename);

    render.updateImage();

    render.show();

    return app.exec();
}