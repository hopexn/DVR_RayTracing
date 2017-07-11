#include "VolumeRender.h"
#include <iostream>

using namespace std;

VolumeRender::VolumeRender(QWidget *parent) : QWidget(parent) {
    width = 256;
    height = 256;
    this->setFixedWidth(width);
    this->setFixedHeight(height);
    cam_pos = vec3(0.5f, 0.5f, 10.0f);
    cam_up = vec3(0.0f, 1.0f, 0.0f);
    volume_center = vec3(0.5f, 0.5f, 0.5f);
    cam_dir = fastNormalize(volume_center - cam_pos);
    cam_right = fastNormalize(cross(cam_dir, cam_pos));
    cam_screen_dist = 8.0f;
    image = new QImage(width, height, QImage::Format_ARGB32);
    step_dist = 1.0f / volume.ziSize;
}

void VolumeRender::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    if (image == NULL) {
        cout << "Image is NULL" << endl;
        return;
    }
    QRect rect(0, 0, image->width(), image->height());
    painter.drawImage(rect, *image);
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
float VolumeRender::caculate_enter_dist(vec3 cam_pos, vec3 ray_dir) {
    float lamda = 0.0f;
    vec3 dst_pos;
    if (cam_pos.x > 0.5f) {
        lamda = (1.0f - cam_pos.x) / ray_dir.x;
        dst_pos = cam_pos + lamda * ray_dir;
        if (dst_pos.y > 0.0f && dst_pos.y < 1.0f && dst_pos.z > 0.0f && dst_pos.z < 1.0f) {
            return lamda;
        }
    } else {
        lamda = (0.0f - cam_pos.x) / ray_dir.x;
        dst_pos = cam_pos + lamda * ray_dir;
        if (dst_pos.y > 0.0f && dst_pos.y < 1.0f && dst_pos.z > 0.0f && dst_pos.z < 1.0f) {
            return lamda;
        }
    }

    if (cam_pos.y > 0.5f) {
        lamda = (1.0f - cam_pos.y) / ray_dir.y;
        dst_pos = cam_pos + lamda * ray_dir;
        if (dst_pos.x > 0.0f && dst_pos.x < 1.0f && dst_pos.z > 0.0f && dst_pos.z < 1.0f) {
            return lamda;
        }
    } else {
        lamda = (0.0f - cam_pos.y) / ray_dir.y;
        dst_pos = cam_pos + lamda * ray_dir;
        if (dst_pos.x > 0.0f && dst_pos.x < 1.0f && dst_pos.z > 0.0f && dst_pos.z < 1.0f) {
            return lamda;
        }
    }

    if (cam_pos.z > 0.5f) {
        lamda = (1.0f - cam_pos.z) / ray_dir.z;
        dst_pos = cam_pos + lamda * ray_dir;
        if (dst_pos.x > 0.0f && dst_pos.x < 1.0f && dst_pos.y > 0.0f && dst_pos.y < 1.0f) {
            return lamda;
        }
    } else {
        lamda = (0.0f - cam_pos.z) / ray_dir.z;
        dst_pos = cam_pos + lamda * ray_dir;
        if (dst_pos.x > 0.0f && dst_pos.x < 1.0f && dst_pos.y > 0.0f && dst_pos.y < 1.0f) {
            return lamda;
        }
    }
    return 0;
}

float VolumeRender::caculate_leave_dist(vec3 cam_pos, vec3 ray_dir) {
    float lamda;
    vec3 dst_pos;
    if (cam_pos.x < 0.5f) {
        lamda = (1.0f - cam_pos.x) / ray_dir.x;
        dst_pos = cam_pos + lamda * ray_dir;
        if (dst_pos.y > 0.0f && dst_pos.y < 1.0f && dst_pos.z > 0.0f && dst_pos.z < 1.0f) {
            return lamda;
        }
    } else {
        lamda = (0.0f - cam_pos.x) / ray_dir.x;
        dst_pos = cam_pos + lamda * ray_dir;
        if (dst_pos.y > 0.0f && dst_pos.y < 1.0f && dst_pos.z > 0.0f && dst_pos.z < 1.0f) {
            return lamda;
        }
    }

    if (cam_pos.y < 0.5f) {
        lamda = (1.0f - cam_pos.y) / ray_dir.y;
        dst_pos = cam_pos + lamda * ray_dir;
        if (dst_pos.x > 0.0f && dst_pos.x < 1.0f && dst_pos.z > 0.0f && dst_pos.z < 1.0f) {
            return lamda;
        }
    } else {
        lamda = (0.0f - cam_pos.y) / ray_dir.y;
        dst_pos = cam_pos + lamda * ray_dir;
        if (dst_pos.x > 0.0f && dst_pos.x < 1.0f && dst_pos.z > 0.0f && dst_pos.z < 1.0f) {
            return lamda;
        }
    }

    if (cam_pos.z < 0.5f) {
        lamda = (1.0f - cam_pos.z) / ray_dir.z;
        dst_pos = cam_pos + lamda * ray_dir;
        if (dst_pos.x > 0.0f && dst_pos.x < 1.0f && dst_pos.y > 0.0f && dst_pos.y < 1.0f) {
            return lamda;
        }
    } else {
        lamda = (0.0f - cam_pos.z) / ray_dir.z;
        dst_pos = cam_pos + lamda * ray_dir;
        if (dst_pos.x > 0.0f && dst_pos.x < 1.0f && dst_pos.y > 0.0f && dst_pos.y < 1.0f) {
            return lamda;
        }
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
    cout << "Updating Image" << endl;
    vec3 screen_center = cam_screen_dist * cam_dir + cam_pos;
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            vec3 pixel_pos = screen_center + (-0.5f + (float) i / width) * cam_right +
                             (-0.5f + (float) j / height) * cam_up;
            //光线方向
            vec3 ray_dir = fastNormalize(pixel_pos - cam_pos);
            //计算进入点与离开点
            float begin = caculate_enter_dist(cam_pos, ray_dir);
            float end = caculate_leave_dist(cam_pos, ray_dir);
            glm::vec3 color_cum(0, 0, 0);
            float opacity_cum = 0;
            for (int k = 0; begin + k * step_dist < end; k++) {
                float value = volume.getVolumeValue(vec3(i, j, k));
                glm::vec4 color_and_alpha = volume.tf1d.trans_func(value);
                color_cum.r = glm::min(color_cum.r + (1.0f - opacity_cum) * color_and_alpha.r, 1.0f);
                color_cum.g = glm::min(color_cum.g + (1.0f - opacity_cum) * color_and_alpha.g, 1.0f);
                color_cum.b = glm::min(color_cum.b + (1.0f - opacity_cum) * color_and_alpha.b, 1.0f);
                opacity_cum = glm::min(opacity_cum + (1.0f - opacity_cum) * color_and_alpha.a, 1.0f);
                if (opacity_cum >= 1.0) break;
            }
            image->setPixel(i, j, qRgb((int) (255 * (1 - color_cum.r)),
                                       (int) (255 * (1 - color_cum.g)),
                                       (int) (255 * (1 - color_cum.b))));
        }
    }
    cout << "Update Image completed" << endl;
}