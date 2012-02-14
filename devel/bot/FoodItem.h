#ifndef FoodItem_H
#define FoodItem_H

#include <QtGui>

class FoodItem : public QObject,
                 public QGraphicsItem
{
	Q_OBJECT
public:
	FoodItem(QColor color = Qt::green);

	QRectF boundingRect() const { return m_rect; }
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
	QRectF m_rect;
	QColor m_color;
	
};

#endif
