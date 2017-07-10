#ifndef RAYTRACING_VOLUMERENDER_H
#define RAYTRACING_VOLUMERENDER_H


#include <QPainter>
#include <QWidget>
#include <QPainter>
#include <QImage>
#include <QRect>

class VolumeRender : public QWidget {
Q_OBJECT
public:
    VolumeRender(QWidget *parent = 0);

    void setImage(QImage *image);

    void paintEvent(QPaintEvent *event);

protected:


private:
    QPainter *canvas;
    QImage *image;
    QRect *rect;
};

#endif //RAYTRACING_VOLUMERENDER_H
