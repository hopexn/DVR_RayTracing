#include "VolumeRender.h"
#include <iostream>
#include <QLabel>
#include <QPixmap>

#define VOLUME_MAXIMUM_VALUE 255

using namespace std;

VolumeRender::VolumeRender(QWidget *parent) : QWidget(parent) {
    width = 256;
    height = 256;
    this->setFixedWidth(width);
    this->setFixedHeight(height);
    image = new QImage(width, height, QImage::Format_RGB32);

    cam_pos_init = vec3(0.0f, 0.0f, 6.0f);
    cam_right_init = vec3(1.0f, 0.0f, 0.0f);
    cam_screen_dist = 4.0f;

    rotationX = 0;
    rotationY = 0;
    rotationZ = 0;

    rotate();
}

void VolumeRender::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    if (image == NULL) {
        cout << "Image is NULL" << endl;
        return;
    }

    QRect rect(0, 0, image->width(), image->height());
    painter.drawImage(rect, *image);
    cout << "Paint completed" << endl;
}

void VolumeRender::updateVolume(string filename) {
    volume.loadRawData(filename.c_str());
}

/**
 * 为每一个面编号：
 * 底： 1
 * 左： 2
 * 后： 3
 * 右： 4
 * 前： 5
 * 上： 6
 */
float VolumeRender::caculate_enter_leave(vec3 ray_dir, int enter_or_leave) {
    float lamda = 0.0f;
    vec3 dst_pos;

    if ((cam_pos.x > 0.0f) ^ enter_or_leave) {
        lamda = (0.5f * volume.xfSize - cam_pos.x) / ray_dir.x;
    } else {
        lamda = (-0.5f * volume.xfSize - cam_pos.x) / ray_dir.x;
    }
    dst_pos = cam_pos + lamda * ray_dir;
    if (dst_pos.y > -0.5f * volume.yfSize && dst_pos.y < 0.5f * volume.yfSize
        && dst_pos.z > -0.5f * volume.zfSize && dst_pos.z < 0.5f * volume.zfSize) {
        return lamda;
    }

    if ((cam_pos.y > 0.0f) ^ enter_or_leave) {
        lamda = (0.5f * volume.yfSize - cam_pos.y) / ray_dir.y;
    } else {
        lamda = (-0.5f * volume.yfSize - cam_pos.y) / ray_dir.y;
    }
    dst_pos = cam_pos + lamda * ray_dir;
    if (dst_pos.x > -0.5f * volume.xfSize && dst_pos.x < 0.5f * volume.xfSize
        && dst_pos.z > -0.5f * volume.zfSize && dst_pos.z < 0.5f * volume.zfSize) {
        return lamda;
    }

    if ((cam_pos.z > 0.0f) ^ enter_or_leave) {
        lamda = (0.5f * volume.zfSize - cam_pos.z) / ray_dir.z;
    } else {
        lamda = (-0.5f * volume.zfSize - cam_pos.z) / ray_dir.z;
    }
    dst_pos = cam_pos + lamda * ray_dir;
    if (dst_pos.x > -0.5f * volume.xfSize && dst_pos.x < 0.5f * volume.xfSize
        && dst_pos.y > -0.5f * volume.yfSize && dst_pos.y < 0.5f * volume.yfSize) {
        return lamda;
    }
    if (!enter_or_leave) {
        return INFINITY;
    } else {
        return 0;
    }
}

void VolumeRender::updateImage() {
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
    float threshold = 1.0f;
    vec3 screen_center = cam_screen_dist * cam_dir + cam_pos;
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            vec3 pixel_pos = screen_center
                             + (-0.5f + 1.0f * i / width) * cam_right
                             + (-0.5f + 1.0f * j / height) * cam_up;
            //光线方向
            vec3 ray_dir = fastNormalize(pixel_pos - cam_pos);

            //计算进入点与离开点
            float begin = caculate_enter_leave(ray_dir, 0);
            float end = caculate_enter_leave(ray_dir, 1);

            glm::vec3 color_cum(0, 0, 0);
            float opacity_cum = 0;

            for (int k = 0; begin + k * volume.step_dist < end; k++) {
                vec3 pos = cam_pos + (begin + k * volume.step_dist) * ray_dir;
                float value = volume.getVolumeValue(pos);
                glm::vec4 color_and_alpha = volume.tf1d.trans_func(value);
                color_cum.r = glm::min(color_cum.r + (1.0f - opacity_cum) * color_and_alpha.r, threshold);
                color_cum.g = glm::min(color_cum.g + (1.0f - opacity_cum) * color_and_alpha.g, threshold);
                color_cum.b = glm::min(color_cum.b + (1.0f - opacity_cum) * color_and_alpha.b, threshold);
                opacity_cum = glm::min(opacity_cum + (1.0f - opacity_cum) * color_and_alpha.a, threshold);
                if (opacity_cum >= threshold) break;
            }
            image->setPixel(i, j, qRgb((int) (255 * (1 - color_cum.r)),
                                       (int) (255 * (1 - color_cum.g)),
                                       (int) (255 * (1 - color_cum.b))));
        }
    }
    cout << "Update Image completed" << endl;
}

void VolumeRender::mousePressEvent(QMouseEvent *event) {
    last_pos = event->pos();
}

void VolumeRender::mouseMoveEvent(QMouseEvent *event) {
    float dx = float(event->x() - last_pos.x()) / width;
    float dy = float(event->y() - last_pos.y()) / height;

    if (event->buttons() & Qt::LeftButton) {
        rotationX += 3.14 * dx;
        rotationY += 3.14 * dy;
        rotate();
        updateImage();
        repaint();
    }
    last_pos = event->pos();
}


void VolumeRender::rotate() {
    if (rotationX == 0 && rotationY == 0) {
        cam_pos = cam_pos_init;
        cam_right = cam_right_init;
        cam_dir = fastNormalize(volume.center - cam_pos);
        cam_up = fastNormalize(cross(cam_dir, cam_right));
        return;
    }
    vec3 axis = fastNormalize(cross(vec3(0, 0, 1), vec3(rotationX, rotationY, 0)));
    int sign = rotationX * rotationY > 0 ? 1 : -1;
    float theta = (float) (glm::sqrt(glm::pow(rotationX, 2) + glm::pow(rotationY, 2)));

    float u = axis.x;
    float v = axis.y;
    float w = axis.z;
    float m00, m01, m02, m10, m11, m12, m20, m21, m22;

    m00 = cosf(theta) + (u * u) * (1 - cosf(theta));
    m01 = u * v * (1 - cosf(theta)) + w * sinf(theta);
    m02 = u * w * (1 - cosf(theta)) - v * sinf(theta);

    m10 = u * v * (1 - cosf(theta)) - w * sinf(theta);
    m11 = cosf(theta) + v * v * (1 - cosf(theta));
    m12 = w * v * (1 - cosf(theta)) + u * sinf(theta);

    m20 = u * w * (1 - cosf(theta)) + v * sinf(theta);
    m21 = v * w * (1 - cosf(theta)) - u * sinf(theta);
    m22 = cosf(theta) + w * w * (1 - cosf(theta));

    cam_pos.x = cam_pos_init.x * m00 + cam_pos_init.y * m01 + cam_pos_init.z * m02;
    cam_pos.y = cam_pos_init.x * m10 + cam_pos_init.y * m11 + cam_pos_init.z * m12;
    cam_pos.z = cam_pos_init.x * m20 + cam_pos_init.y * m21 + cam_pos_init.z * m22;

    cam_right.x = cam_right_init.x * m00 + cam_right_init.y * m01 + cam_right_init.z * m02;
    cam_right.y = cam_right_init.x * m10 + cam_right_init.y * m11 + cam_right_init.z * m12;
    cam_right.z = cam_right_init.x * m20 + cam_right_init.y * m21 + cam_right_init.z * m22;

    cam_dir = fastNormalize(volume.center - cam_pos);
    cam_up = fastNormalize(cross(cam_dir, cam_right));

    double dist = glm::pow(cam_pos.x, 2) + glm::pow(cam_pos.y, 2) + glm::pow(cam_pos.z, 2);
    cout << "cam_pos:" << cam_pos.x << " " << cam_pos.y << " " << cam_pos.z << " " << dist << endl;
    cout << "cam_right:" << cam_right.x << " " << cam_right.y << " " << cam_right.z << endl;
    cout << "cam_up:" << cam_up.x << " " << cam_up.y << " " << cam_up.z << endl;

    cout << "rotate:" << rotationX << " " << rotationY << " " << rotationZ << endl;
}

