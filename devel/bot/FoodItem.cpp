#include "FoodItem.h"

FoodItem::FoodItem(QColor color)
	: QObject()
	, QGraphicsItem()
	, m_color(color)
{
	m_rect = QRectF(0,0,15,15);
}

void FoodItem::paint(QPainter *p, const QStyleOptionGraphicsItem */*option*/, QWidget *)
{
	QRectF rect = m_rect;
	
	//p->setPen(Qt::black);
	p->fillRect(rect, m_color);
	p->drawRect(rect);
}
