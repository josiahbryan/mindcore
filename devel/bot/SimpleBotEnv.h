#ifndef SimpleBotEnv_H
#define SimpleBotEnv_H

#include <QtGui>

class SimpleBotEnv : public QGraphicsScene
{
public:
	SimpleBotEnv();

protected:
	void drawForeground(QPainter *painter, const QRectF&);
};


#endif
