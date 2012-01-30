#include "BotWindow.h"

#include "MindSpaceGraphWidget.h"

#include <QGraphicsPolygonItem>
 
BotWindow::BotWindow()
	: QWidget()
{
// 	QGraphicsView *gv = new QGraphicsView();
// 	
// 	QGraphicsScene *scene = new QGraphicsScene();
// 	
// 	QPolygonF polygon = QPolygonF()
// 		<< QPointF(0., 10.)
// 		<< QPointF(10.,10.)
// 		<< QPointF( 5., 0.);
// 	
// 	QGraphicsPolygonItem *polyItem = new QGraphicsPolygonItem(polygon);
// 		
// 	scene->addItem(polyItem);
// 	
// 	gv->setScene(scene);
	
	
	QVBoxLayout *vbox = new QVBoxLayout(this);
	//vbox->addWidget(gv);
	
	MindSpaceGraphWidget *gw = new MindSpaceGraphWidget();
	vbox->addWidget(gw);

}
