#ifndef MindSpaceGraphWidget_H
#define MindSpaceGraphWidget_H

#include <QtGui/QGraphicsView>

class MindSpaceGraphNode;

class MindSpaceGraphWidget : public QGraphicsView
{
    Q_OBJECT

public:
	MindSpaceGraphWidget();
	
	void itemMoved();

protected:
	void keyPressEvent(QKeyEvent *);
	void timerEvent(QTimerEvent *);
	void wheelEvent(QWheelEvent *);
	void drawBackground(QPainter *, const QRectF &);
	
	void scaleView(qreal scaleFactor);

private:
	int m_timerId;
	MindSpaceGraphNode *m_centerNode;
};

#endif
