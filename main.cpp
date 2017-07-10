#include <iostream>
#include <opencv2/opencv.hpp>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <QApplication>
#include <QImage>
#include <QRgb>
#include <QPainter>
#include <QRect>
#include <QLabel>

#include "Volume.h"
#include "VolumeRender.h"

using namespace std;
using namespace cv;

double trans_func(unsigned char val) {
    return val / 255.0;
}

int main(int argc, char **argv) {
    int img_width = 256, img_height = 256;

    glm::vec3 eye_pos(0.5, 0.5, 10);
    glm::vec3 up(0, 1, 0);
    glm::vec3 right(1, 0, 0);
    glm::vec3 toward(0, 0, -1);

    Volume volume;
    string filename = "assets/volume/aneurism.vifo";
    volume.loadRawData(filename.c_str());




//    1. 从前向后
//    C'(i+1) = C(i) + (1 - A(i)) * C'(i)
//     - C'(x) 表示从起点到x的累积光强
//     - C(x) 表示x点发光强度
//     - A(x) 表示x点的不透明度
//
//    2. 从后到前
//    C'(i) = C'(i + 1) + (1 - A'(i+1)) * C(i)
//    A'(i) = A'(i + 1) + (1 - A'(i+1)) * A(i)
//     - C'(x)表示从x到终点所有能到终点的光强之和
//     - C(x)表示x点的发光强度
//     - A'(x)表示从x点到终点的不透明度
    QApplication app(argc, argv);
    QImage image(img_width, img_height, QImage::Format_ARGB32);
    Mat mat(256, 256, 4);
    for (int i = 0; i < volume.xiSize; i++) {
        for (int j = 0; j < volume.yiSize; j++) {
            glm::vec3 color_cum(0, 0, 0);
            float opacity_cum = 0;
            for (int k = volume.ziSize - 1; k >= 0; k--) {
                glm::vec4 color_and_alpha = volume.tf1d.trans_func(
                        volume.data[k * volume.xiSize * volume.yiSize + j * volume.yiSize + i]);
                color_cum.r = min(color_cum.r + (1.0f - opacity_cum) * color_and_alpha.r, 1.0f);
                color_cum.g = min(color_cum.g + (1.0f - opacity_cum) * color_and_alpha.g, 1.0f);
                color_cum.b = min(color_cum.b + (1.0f - opacity_cum) * color_and_alpha.b, 1.0f);
                opacity_cum = min(opacity_cum + (1.0f - opacity_cum) * color_and_alpha.a, 1.0f);
                //if (opacity_cum == 1.0) break;
            }
            image.setPixel(i, j, qRgb((int)(255 * color_cum.r), (int)(255 * color_cum.g), (int)(255 * color_cum.b)));
        }
    }

//    QLabel label;
//    label.setPixmap(QPixmap::fromImage(image));
//
//    label.show();

    VolumeRender render;

    render.setImage(&image);

    render.show();

    return app.exec();
}