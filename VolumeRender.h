#ifndef RAYTRACING_VOLUMERENDER_H
#define RAYTRACING_VOLUMERENDER_H


#include <QPainter>
#include <QWidget>
#include <QPainter>
#include <QImage>
#include <QRect>
#include <glm/glm.hpp>
#include <glm/gtx/fast_square_root.hpp>
#include "Volume.h"

#include <QMouseEvent>

using namespace std;
using namespace glm;

class VolumeRender : public QWidget {
Q_OBJECT
public:
    VolumeRender(QWidget *parent = 0);

    void updateImage();

    void updateVolume(string filename);

    ~VolumeRender() {
        delete image;
    }

    float caculate_enter_leave(vec3 ray_dir, int enter_or_leave);

protected:
    void paintEvent(QPaintEvent *event);

    void mousePressEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

    void rotate();

private:
    QImage *image;

    int width, height;

    vec3 cam_pos, cam_dir, cam_up, cam_right;

    float cam_screen_dist;

    Volume volume;

    QPoint last_pos;

    float rotationX, rotationY, rotationZ;
};


#endif //RAYTRACING_VOLUMERENDER_H
