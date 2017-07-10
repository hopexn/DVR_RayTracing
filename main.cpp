#include <iostream>
#include <opencv2/opencv.hpp>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>

#include "Volume.h"

using namespace std;

double trans_func(unsigned char val) {
    return val / 255.0;
}

int main() {

    int img_width = 256, img_height = 256;

    cv::Mat image(img_width, img_height, 4);

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

    for (int i = 0; i < volume.xiSize; i++) {
        for (int j = 0; j < volume.yiSize; j++) {
            double color_cum = 0;
            double opacity_cum = 0;
            for (int k = volume.ziSize - 1; k >= 0; k--) {
                double opacity = trans_func(volume.data[k * volume.xiSize * volume.yiSize + j * volume.yiSize + i]);
                color_cum = min(color_cum + (1.0 - opacity_cum) * opacity_cum * 0.1, 1.0);
                opacity_cum = min(opacity_cum + (1.0 - opacity_cum) * opacity, 1.0);
                if (opacity_cum == 1.0) break;
            }
            for (int k = 0; k < 3; k++) {
                image.data[(j * volume.yiSize + i) * 4 + k] = color_cum;
            }
            image.data[(j * volume.yiSize + i) * 4 + 3] = opacity_cum;
        }
    }
    cv::imshow("ImageTest", image);
    cv::waitKey(0);
    return 0;
}