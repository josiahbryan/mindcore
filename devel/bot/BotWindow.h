#ifndef BotWindow_H
#define BotWindow_H

#include <QtGui>

#include "MindSpace.h"
using namespace MindSpace;
class MSpaceViewerWidget;

class TweakedGraphicsView : public QGraphicsView
{
	Q_OBJECT
public:
	TweakedGraphicsView();
	void scaleView(qreal scaleFactor);
	
protected:
	void wheelEvent(QWheelEvent *event);
	void drawBackground(QPainter *painter, const QRectF &rect);
};

class BotWindow : public QWidget
{
	Q_OBJECT
public:
	BotWindow();
	~BotWindow() {}

public slots:
	
	
private slots:
	
	
protected:
	void closeEvent(QCloseEvent*);
	
private:
	MSpaceViewerWidget *m_mspaceViewer;
	MSpace *m_mspace;
	
	TweakedGraphicsView *m_gv;
};


#endif
