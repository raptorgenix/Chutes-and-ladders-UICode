#ifndef SHAPE_H
#define SHAPE_H

#include <QObject>
#include <QGraphicsItem>


class ShapeObj : public QObject, public QGraphicsItem {

    // this makes it so that we can emit signals
    Q_OBJECT

public:
    ShapeObj(int x, int y, QColor c);

    QRectF boundingRect() const override;
    QPainterPath shape() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;

    int x_;
    int y_;
    int width_;
    QColor colr;

};

#endif // SHAPE_H
