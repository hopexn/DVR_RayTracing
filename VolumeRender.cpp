#include "VolumeRender.h"
#include <iostream>
#include <QLabel>
#include <QPixmap>

using namespace std;

VolumeRender::VolumeRender(QWidget *parent) : QWidget(parent) {
    width = 256;
    height = 256;
    cam_pos_o = vec3(0.0f, 0.0f, 6.0f);
    cam_up_o = vec3(0.0f, 1.0f, 0.0f);
    cam_dir = fastNormalize(volume_center - cam_pos);
    cam_right = fastNormalize(cross(cam_dir, cam_up));
    cam_screen_dist = 4.0f;
    step_dist = 0.02f;
    rotationX = 0;
    rotationY = 0;
    rotationZ = 0;
    rotate();
    this->setFixedWidth(width);
    this->setFixedHeight(height);
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
float VolumeRender::caculate_enter_dist(vec3 ray_dir) {
    float lamda = 0.0f;
    vec3 dst_pos;

    if (cam_pos.x > 0.0f) {
        lamda = (0.5f - cam_pos.x) / ray_dir.x;
    } else {
        lamda = (-0.5f - cam_pos.x) / ray_dir.x;
    }
    dst_pos = cam_pos + lamda * ray_dir;
    if (dst_pos.y > -0.5f && dst_pos.y < 0.5f && dst_pos.z > -0.5f && dst_pos.z < 0.5f) {
        return lamda;
    }

    if (cam_pos.y > 0.0f) {
        lamda = (0.5f - cam_pos.y) / ray_dir.y;
    } else {
        lamda = (-0.5f - cam_pos.y) / ray_dir.y;
    }
    dst_pos = cam_pos + lamda * ray_dir;
    if (dst_pos.x > -0.5f && dst_pos.x < 0.5f && dst_pos.z > -0.5f && dst_pos.z < 0.5f) {
        return lamda;
    }

    if (cam_pos.z > 0.0f) {
        lamda = (0.5f - cam_pos.z) / ray_dir.z;
    } else {
        lamda = (-0.5f - cam_pos.z) / ray_dir.z;
    }
    dst_pos = cam_pos + lamda * ray_dir;
    if (dst_pos.x > -0.5f && dst_pos.x < 0.5f && dst_pos.y > -0.5f && dst_pos.y < 0.5f) {
        return lamda;
    }

    return 0;
}

float VolumeRender::caculate_leave_dist(vec3 ray_dir) {
    float lamda = 0.0f;
    vec3 dst_pos;

    if (cam_pos.x < 0.0f) {
        lamda = (0.5f - cam_pos.x) / ray_dir.x;
    } else {
        lamda = (-0.5f - cam_pos.x) / ray_dir.x;
    }
    dst_pos = cam_pos + lamda * ray_dir;
    if (dst_pos.y > -0.5f && dst_pos.y < 0.5f && dst_pos.z > -0.5f && dst_pos.z < 0.5f) {
        return lamda;
    }

    if (cam_pos.y < 0.0f) {
        lamda = (0.5f - cam_pos.y) / ray_dir.y;
    } else {
        lamda = (-0.5f - cam_pos.y) / ray_dir.y;
    }
    dst_pos = cam_pos + lamda * ray_dir;
    if (dst_pos.x > -0.5f && dst_pos.x < 0.5f && dst_pos.z > -0.5f && dst_pos.z < 0.5f) {
        return lamda;
    }

    if (cam_pos.z < 0.0f) {
        lamda = (0.5f - cam_pos.z) / ray_dir.z;
    } else {
        lamda = (-0.5f - cam_pos.z) / ray_dir.z;
    }
    dst_pos = cam_pos + lamda * ray_dir;
    if (dst_pos.x > -0.5f && dst_pos.x < 0.5f && dst_pos.y > -0.5f && dst_pos.y < 0.5f) {
        return lamda;
    }

    return 0;
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
    delete image;
    float threshold = 0.8;
    image = new QImage(width, height, QImage::Format_RGB32);
    vec3 screen_center = cam_screen_dist * cam_dir + cam_pos;
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            vec3 pixel_pos = screen_center + (-0.5f + 1.0f * i / width) * cam_right +
                             (-0.5f + 1.0f * j / height) * cam_up;
            //光线方向
            vec3 ray_dir = fastNormalize(pixel_pos - cam_pos);
            //计算进入点与离开点
            float begin = caculate_enter_dist(ray_dir);
            float end = caculate_leave_dist(ray_dir);
            step_dist = (float) abs(0.005f * (ray_dir.x * 0.5 + ray_dir.y * 0.5 + ray_dir.z * 0.5));
            glm::vec3 color_cum(0, 0, 0);
            float opacity_cum = 0;
            for (int k = 0; begin + k * step_dist < end; k++) {
                vec3 pos = cam_pos + (begin + k * step_dist) * ray_dir;
                float value = volume.getVolumeValue(pos);
                glm::vec4 color_and_alpha = volume.tf1d.trans_func(value);
                color_cum.r = glm::min(color_cum.r + (1.0f - opacity_cum) * color_and_alpha.r, threshold);
                color_cum.g = glm::min(color_cum.g + (1.0f - opacity_cum) * color_and_alpha.g, threshold);
                color_cum.b = glm::min(color_cum.b + (1.0f - opacity_cum) * color_and_alpha.b, threshold);
                opacity_cum = glm::min(opacity_cum + (1.0f - opacity_cum) * color_and_alpha.a, threshold);
                if (opacity_cum >= threshold) break;
            }
            color_cum.r = glm::min(color_cum.r, 1.0f);
            color_cum.g = glm::min(color_cum.g, 1.0f);
            color_cum.b = glm::min(color_cum.b, 1.0f);
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

int flag = 0;

void VolumeRender::mouseMoveEvent(QMouseEvent *event) {
    float dx = float(event->x() - last_pos.x()) / width;
    float dy = float(event->y() - last_pos.y()) / height;

    if (event->buttons() & Qt::LeftButton) {
        rotationX += 3.14 * dy;
        rotationY += 3.14 * dx;
        rotate();
        updateImage();
        repaint();
    }
    last_pos = event->pos();
}

float norm(vec3 vec) {
    return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}

void VolumeRender::rotate() {
    vec3 axis = fastNormalize(cross(vec3(0, 0, 1), vec3(rotationX, rotationY, 0)));
    float theta = (float) glm::sqrt(glm::pow(rotationX, 2) + glm::pow(rotationY, 2));

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

    cam_pos.x = cam_pos_o.x * m00 + cam_pos_o.y * m01 + cam_pos_o.z * m02;
    cam_pos.y = cam_pos_o.x * m10 + cam_pos_o.y * m11 + cam_pos_o.z * m12;
    cam_pos.z = cam_pos_o.x * m20 + cam_pos_o.y * m21 + cam_pos_o.z * m22;

    cam_up.x = cam_up_o.x * m00 + cam_up_o.y * m01 + cam_up_o.z * m02;
    cam_up.y = cam_up_o.x * m10 + cam_up_o.y * m11 + cam_up_o.z * m12;
    cam_up.z = cam_up_o.x * m20 + cam_up_o.y * m21 + cam_up_o.z * m22;

    cout << "cam_up:" << cam_up.x << " " << cam_up.y << " " << cam_up.z << endl;
    double dist = glm::pow(cam_pos.x, 2) + glm::pow(cam_pos.y, 2) + glm::pow(cam_pos.z, 2);
    cout << "cam_pos:" << cam_pos.x << " " << cam_pos.y << " " << cam_pos.z << " " << dist << endl;
    cout << "rotate:" << rotationX << " " << rotationY << " " << rotationZ << endl;

    cam_dir = fastNormalize(volume_center - cam_pos);
    cam_right = fastNormalize(cross(cam_dir, cam_up));
}

