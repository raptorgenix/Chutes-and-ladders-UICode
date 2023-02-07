#include "Shape.h"
#include <QDebug>
#include <QPainter>

ShapeObj::ShapeObj(int x, int y, QColor c) : x_(x), y_(y)
{
    width_ = 20;
    colr = c;
    qDebug() << colr;
}

// where is this object located
// always a rectangle, Qt uses this to know "where" the user
// would be interacting with this object
QRectF ShapeObj::boundingRect() const
{
    return QRectF(x_, y_, width_, width_);
}

// define the actual shape of the object
QPainterPath ShapeObj::shape() const
{
    QPainterPath path;
    path.addEllipse(x_, y_, width_, width_);
    return path;
}

// called by Qt to actually display the point
void ShapeObj::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)



    // update the line for setBrush to be this
    painter->setBrush(QBrush(colr));
    painter->drawEllipse(QRect(this->x_, this->y_, this->width_, this->width_));
}

