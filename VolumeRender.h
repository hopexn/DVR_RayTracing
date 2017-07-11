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

    float caculate_enter_dist(vec3 cam_pos, vec3 ray_dir);

    float caculate_leave_dist(vec3 cam_pos, vec3 ray_dir);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QImage *image;
    int width, height;

    vec3 cam_pos, cam_dir, cam_up, cam_right;
    vec3 volume_center;

    float cam_screen_dist;

    float step_dist;

    Volume volume;
};


#endif //RAYTRACING_VOLUMERENDER_H
