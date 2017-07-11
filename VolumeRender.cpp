#include "VolumeRender.h"
#include <iostream>

using namespace std;

VolumeRender::VolumeRender(QWidget *parent) {
    QPainter *canvas;

    image = NULL;
    rect = NULL;
}

void VolumeRender::setImage(QImage *image) {
    if (image == NULL) {
        cout << "Image is NULL" << endl;
        return;
    }
    this->image = image;
    this->rect = new QRect(0, 0, image->width(), image->height());
    this->setFixedWidth(image->width());
    this->setFixedHeight(image->height());
}


void VolumeRender::paintEvent(QPaintEvent *event) {
    canvas = new QPainter(this);
    if (image == NULL) {
        cout << "Image is NULL" << endl;
        return;
    }
    canvas->drawImage(*rect, *image);
    delete canvas;
}
