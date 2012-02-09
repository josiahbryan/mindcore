#include "MindSpaceGraphWidget.h"
#include "MindSpaceGraphEdge.h"
#include "MindSpaceGraphNode.h"

#include <QDebug>
#include <QGraphicsScene>
#include <QWheelEvent>

#include <QGLWidget>

#include <QTime>

#include <math.h>

#include "MindSpace.h"
using namespace MindSpace;

#define EDGE_WEIGHT_FACTOR 1.0

#include <QSettings>
static QSettings MindSpaceGraphWidget_ColorSettings("graphcolors.ini", QSettings::IniFormat);

MindSpaceGraphWidget::MindSpaceGraphWidget()
	: m_timerId(0)
	, m_mindSpace(0)
	, m_layoutStopped(false)
{
	QGraphicsScene *scene = new QGraphicsScene(this);
	scene->setItemIndexMethod(QGraphicsScene::NoIndex);
	//scene->setSceneRect(-1000000, -1000000, 2000000, 2000000);
	setScene(scene);
	
	setCacheMode(CacheBackground);
	//setViewportUpdateMode(BoundingRectViewportUpdate);
	setRenderHint(QPainter::Antialiasing);
	setTransformationAnchor(AnchorUnderMouse);
	setResizeAnchor(AnchorViewCenter);
	setDragMode(QGraphicsView::ScrollHandDrag);
	
	//setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );
	  // if there are ever graphic glitches to be found, remove this again
	setOptimizationFlags(QGraphicsView::DontAdjustForAntialiasing | QGraphicsView::DontClipPainter | QGraphicsView::DontSavePainterState);

	//setCacheMode(QGraphicsView::CacheBackground);
	//setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
	setOptimizationFlags(QGraphicsView::DontSavePainterState);
	//setTransformationAnchor(AnchorUnderMouse);
	//setResizeAnchor(AnchorViewCenter);
	
	
// 	QGLFormat format = QGLFormat(QGL::DirectRendering); // you can play with other rendering formats like DoubleBuffer or SimpleBuffer
// 	format.setSampleBuffers(false);
// 	QGLWidget *glWidget = new QGLWidget(format);
// 	glWidget->setAutoFillBackground(false);
// 	
// 	setViewport(glWidget);

	//setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));

	srand((int)time(0));
	
	
	scale(qreal(0.025), qreal(0.025));
	setMinimumSize(400, 400);
	setWindowTitle(tr("Elastic Nodes"));
}

QColor MindSpaceGraphWidget::colorForType(MindSpace::MNodeType type)
{
	QVariant var = MindSpaceGraphWidget_ColorSettings.value(type.uuid());
	if(var.isNull())
	{
		QColor color;
		
		const int hueSeparation = 5; // ensure the randomly-picked hues are separated by at least 5 values
		color.setHsv( rand() % (360/hueSeparation) * hueSeparation, rand() % 55 + 200, rand() % 55 + 200 );
		var = color;
		
		MindSpaceGraphWidget_ColorSettings.setValue(type.uuid(), var);
	}
	
	if(var.type() == QVariant::String)
	{
		QStringList parts = var.toString().split(",");
		QColor color(parts[0].toInt(),parts[1].toInt(),parts[2].toInt());
		var = color;
	}
	
	return var.value<QColor>();
}

void MindSpaceGraphWidget::addNode(MNode *node)
{
	if(m_graphNodes.contains(node))
		return;
		
	MindSpaceGraphNode *graphNode;
	graphNode = new MindSpaceGraphNode(this, node->content());
	connect(graphNode, SIGNAL(doubleClicked(MindSpaceGraphNode*)), this, SLOT(graphNodeDoubleClicked(MindSpaceGraphNode *)));
	
	graphNode->setColor(colorForType(node->type()));
	
	QVariant pos = node->property("_graph_pos");
	if(pos.isValid())
		graphNode->setPos(pos.toPointF());
	else
		graphNode->setPos(rand() % 400 - 200, rand() % 400 - 200);
	
	scene()->addItem(graphNode);
	
	m_graphNodes[node] = graphNode;
	m_graphNodesReverse[graphNode] = node;
}

void MindSpaceGraphWidget::removeNode(MNode *node)
{
	MindSpaceGraphNode *ptr = m_graphNodes[node];
	if(!ptr)
		return;
	 
	scene()->removeItem(ptr);
	
	m_graphNodesReverse.remove(ptr);
	m_graphNodes.remove(node);
	
	delete ptr;
	//ptr->deleteLater();
}

void MindSpaceGraphWidget::graphNodeDoubleClicked(MindSpaceGraphNode *graphNode)
{
	MNode *node = m_graphNodesReverse[graphNode];
	if(node)
	{
// 		m_linksProcessed.clear();
// 		clearScene();
// 		m_gw->scaleView(0.05);
// 		
// 		qDebug() << "MindSpaceGraphWidget::graphNodeDoubleClicked(): Mapping node:"<<node;
// 		mapNode(node, 2);
		emit nodeDoubleClicked(node);
	}
}

void MindSpaceGraphWidget::addLink(MLink *link)
{
	if(m_graphLinks.contains(link))
		return;
	
	MindSpaceGraphNode *startNode = m_graphNodes[link->node1()];
	if(!startNode)
	{
		qDebug() << "MindSpaceGraphWidget::addLink: Fatal Error: node1 ("<<link->node1()<<") of link "<<link<<" not found in existing graph nodes, exiting.";
		exit(-1);
	}
	
	connect(link, SIGNAL(truthValueChanged(MTruthValue)), this, SLOT(linkTruthValueChanged(MTruthValue)));
	
	MindSpaceGraphEdgeData data;
	
	QList<MNode*> args = link->arguments();
	if(!args.isEmpty())
	{
		double value = link->truthValue().value();
	
		MindSpaceGraphNode *typeNode;
		typeNode = new MindSpaceGraphNode(this, QString("%2 %1").arg(value<1.0?QString::number(value):"").arg(link->type().name()));
		//typeNode = new MindSpaceGraphNode(this, QString("%1%2").arg(value<1.0?"!":"").arg(link->type().name()));
		
		typeNode->setProperty("_type_node", true);
		
		m_graphNodesReverse[typeNode] = link->node1();
		
		//typeNode->setPos(rand() % 400 - 200, rand() % 400 - 200);
		QVariant pos = link->node1()->property("_type_node_pos");
		if(pos.isValid())
			typeNode->setPos(pos.toPointF());
		else
			typeNode->setPos(rand() % 400 - 200, rand() % 400 - 200);
		
		
		
		//typeNode->setColor(QColor(255,150,50)); // orange
		typeNode->setColor(Qt::white); // orange
		scene()->addItem(typeNode);
		
		data.node = typeNode;
		
		MindSpaceGraphEdge *edge = new MindSpaceGraphEdge(startNode, typeNode);
		scene()->addItem(edge);
		edge->setWeight(value * EDGE_WEIGHT_FACTOR);
		//edge->setLabel(link->type().name());
		
		data.edges << edge;
		
		foreach(MNode* node2, args)
		{
			MindSpaceGraphNode *endNode = m_graphNodes[node2];
			//scene->addItem(new MindSpaceGraphEdge(typeNode, endNode));
			
			if(!endNode)
			{
				qDebug() << "MindSpaceGraphWidget::addLink: Cannot find graph node for ARG node2: "<<link->node2();
				continue;
			}
			
			MindSpaceGraphEdge *edge = new MindSpaceGraphEdge(typeNode, endNode);
			scene()->addItem(edge);
			edge->setWeight(value * EDGE_WEIGHT_FACTOR);
			
			data.edges << edge;
		}
	}
	else
	{
		double value = link->truthValue().value();
		
		MindSpaceGraphNode *endNode = m_graphNodes[link->node2()];
		if(!endNode)
		{
			qDebug() << "MindSpaceGraphWidget::addLink: Cannot find graph node for node2: "<<link->node2();
			//return; 
		}
		else
		{
			
			MindSpaceGraphEdge *edge = new MindSpaceGraphEdge(startNode, endNode);
			scene()->addItem(edge);
			edge->setWeight(value * EDGE_WEIGHT_FACTOR);
			edge->setLabel(QString("%1 %2").arg(link->type().name()).arg(value!=1.0?QString::number(value):""));
			
			data.edges << edge;
		}
	}
	
	m_graphLinks[link] = data;	
}

void MindSpaceGraphWidget::linkTruthValueChanged(MTruthValue tv)
{
	MLink *link = dynamic_cast<MLink*>(sender());
	if(!link)
	{
		//qDebug() << "MindSpaceGraphWidget::linkTruthValueChanged: sender() does not cast to link";
		return;
	}
		
	if(!m_graphLinks.contains(link))
	{
		//qDebug() << "MindSpaceGraphWidget::linkTruthValueChanged: link "<<link<<" not in graph";
		return;
	}
	
	double value = tv.value();
	//qDebug() << "MindSpaceGraphWidget::linkTruthValueChanged: link "<<link<<" tv is now "<<value<<", updating edges";
	
	MindSpaceGraphEdgeData data = m_graphLinks[link];
	
	foreach(MindSpaceGraphEdge *edge, data.edges)
	{
		edge->setWeight(value * EDGE_WEIGHT_FACTOR);
		edge->setLabel(QString("%1 %2").arg(link->type().name()).arg(value!=1.0?QString::number(value):""));
	}
}

void MindSpaceGraphWidget::removeLink(MLink *link)
{
	if(!m_graphLinks.contains(link))
		return;
	
	disconnect(link, 0, this, 0);
		
	MindSpaceGraphEdgeData data = m_graphLinks[link];
	foreach(MindSpaceGraphEdge *edge, data.edges)
		scene()->removeItem(edge);
	qDeleteAll(data.edges);
	
	if(data.node)
	{
		scene()->removeItem(data.node);
		//data.node->deleteLater();
		delete data.node;
	}
	
	m_graphLinks.remove(link);
}

void MindSpaceGraphWidget::clearScene()
{
	m_layoutStopped = false;
	m_linksProcessed.clear();
		
	foreach(MLink *link, m_graphLinks.keys())
		removeLink(link);
	
	foreach(MNode *node, m_graphNodes.keys())
		removeNode(node);
		
	m_graphNodes.clear();
	m_graphNodesReverse.clear();
	m_graphLinks.clear();
}

void MindSpaceGraphWidget::setMindSpace(MSpace *space)
{
	clearScene();
	
	if(m_mindSpace)
		disconnect(m_mindSpace, 0, this, 0);
	
	m_mindSpace = space;
	
	if(m_mindSpace)
	{
		
		const QList<MNode*> & nodes = space->nodes();
		qDebug() << "MindSpaceGraphWidget::setMindSpace: Creating graph nodes for "<<nodes.size()<<" nodes";
		
		
		foreach(MNode *node, nodes)
			addNode(node);
			//m_centerNode = graphNode; // for use in addTestItem()
		
		connect(space, SIGNAL(nodeAdded(MNode*)), this, SLOT(addNode(MNode*)));
		connect(space, SIGNAL(nodeRemoved(MNode*)), this, SLOT(removeNode(MNode*)));
		
		const QList<MLink*> & links = space->links();
		qDebug() << "MindSpaceGraphWidget::setMindSpace: Creating graph nodes for "<<links.size()<<" links";
		
		foreach(MLink *link, links)
			addLink(link);
			
		connect(space, SIGNAL(linkAdded(MLink*)), this, SLOT(addLink(MLink*)));
		connect(space, SIGNAL(linkRemoved(MLink*)), this, SLOT(removeLink(MLink*)));
		
		qDebug() << "MindSpaceGraphWidget::setMindSpace: Done setting up";
	}
}

void MindSpaceGraphWidget::mapNode(MNode *node, int levels, int currentLevel)
{
	if(m_mindSpace)
		setMindSpace(0);
	
	if(!node)
	{
		//qDebug() << "MindSpaceGraphWidget::mapNode: Null Node, not mapping";
		return;
	}
	
	//qDebug() << "MindSpaceGraphWidget::mapNode: level:"<<currentLevel<<"/"<<levels<<": (size: "<<m_graphNodes.keys().size()<<"nodes/"<<m_graphLinks.keys().size()<<"links) node:"<<node;
	
	addNode(node);
	
	if(currentLevel >= levels)
	{
		//qDebug() << "MindSpaceGraphWidget::mapNode: Reached end, not mapping links";
		return;
	}
	
	const QList<MLink*> & links = node->links();
	foreach(MLink *link, links)
	{
		if(link->node1() != node)
			continue;
			
		if(m_linksProcessed.contains(link))
			continue;
			
		m_linksProcessed << link;
		
		QList<MNode*> args = link->arguments();
		if(!args.isEmpty())
		{
			foreach(MNode *node2, args)
				mapNode(node2, levels, currentLevel+1);	
		}
		else
			mapNode(link->node2(), levels, currentLevel+1);
			
		// Add link AFTER mapping the node so that the destination node exists
		addLink(link);
	}
}


// void MindSpaceGraphWidget::addTestItem()
// {
// 	MindSpaceGraphNode *graphNode;
// 	graphNode = new MindSpaceGraphNode(this, "** TEST **");
// 	graphNode->setPos(rand() % 400 - 200, rand() % 400 - 200);
// 	scene()->addItem(graphNode);
// 	
// 	
// 	scene()->addItem(new MindSpaceGraphEdge(graphNode, m_centerNode));
// }

void MindSpaceGraphWidget::itemMoved(MindSpaceGraphNode *node)
{
	if (!m_timerId)
	{
		if(m_layoutStopped)
			return;
		
		m_timerId = startTimer(1000 / 25);
		emit layoutStarted();
	}
		
	// Store position
	if(node)
	{
		if(MNode *mnode = m_graphNodesReverse[node])
		{
			if(node->property("_type_node").toBool())
				mnode->setProperty("_type_node_pos", node->pos());
			else
				mnode->setProperty("_graph_pos", node->pos());
		}
	}
}

void MindSpaceGraphWidget::keyPressEvent(QKeyEvent *event)
{
    	switch (event->key()) 
    	{
// 	case Qt::Key_Up:
// 		m_centerNode->moveBy(0, -20);
// 		break;
// 	case Qt::Key_Down:
// 		m_centerNode->moveBy(0, 20);
// 		break;
// 	case Qt::Key_Left:
// 		m_centerNode->moveBy(-20, 0);
// 		break;
// 	case Qt::Key_Right:
// 		m_centerNode->moveBy(20, 0);
// 		break;
	case Qt::Key_Plus:
		scaleView(qreal(1.2));
		break;
	case Qt::Key_Minus:
		scaleView(1 / qreal(1.2));
		break;
	case Qt::Key_Space:
	case Qt::Key_Enter:
		foreach (QGraphicsItem *item, scene()->items()) {
		if (qgraphicsitem_cast<MindSpaceGraphNode *>(item))
			item->setPos(-150 + qrand() % 300, -150 + qrand() % 300);
		}
		break;
	default:
		QGraphicsView::keyPressEvent(event);
	}
}

void MindSpaceGraphWidget::timerEvent(QTimerEvent *event)
{
	Q_UNUSED(event);
	
	stepLayout();
}

void MindSpaceGraphWidget::stepLayout()
{
	QList<MindSpaceGraphNode *> nodes;
	foreach (QGraphicsItem *item, scene()->items()) 
	{
		if (MindSpaceGraphNode *node = qgraphicsitem_cast<MindSpaceGraphNode *>(item))
			nodes << node;
	}
	
	bool itemsMoved = true;
	
	static int timeCount =0;
	//qDebug() << "TimerEvent #: " << (++timeCount);
	
	QTime t;
	t.start();
	while(itemsMoved && t.elapsed() < 1000)
	{
		//qDebug() << "\t time "<<t.elapsed()<<"ms";
		itemsMoved = false;
		foreach (MindSpaceGraphNode *node, nodes)
			node->calculateForces();
		
		foreach (MindSpaceGraphNode *node, nodes) 
			if (node->advance())
				itemsMoved = true;
	}
	
	foreach (MindSpaceGraphNode *node, nodes) 
	{
		// Store position
		if(MNode *mnode = m_graphNodesReverse[node])
		{
			if(node->property("_type_node").toBool())
				mnode->setProperty("_type_node_pos", node->pos());
			else
				mnode->setProperty("_graph_pos", node->pos());
		}
	}
	
	emit layoutStep();
	
	if (!itemsMoved) 
	{
		killTimer(m_timerId);
		m_timerId = 0;
		timeCount = 0;
		emit layoutStopped();	
	}
	
	fitInView(scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
	
	//qDebug() << "TimerEvent end";
}

void MindSpaceGraphWidget::stopLayout()
{
	emit layoutStopped();
	killTimer(m_timerId);
	m_timerId = 0;
	fitInView(scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
	m_layoutStopped = true;
}

void MindSpaceGraphWidget::wheelEvent(QWheelEvent *event)
{
	scaleView(pow((double)2, event->delta() / 240.0));
}

void MindSpaceGraphWidget::drawBackground(QPainter *painter, const QRectF &rect)
{
	Q_UNUSED(rect);
	
// 	QRectF sceneRect = this->sceneRect();
// 	QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
// 	gradient.setColorAt(0, Qt::white);
// 	gradient.setColorAt(1, Qt::lightGray);
// 	painter->fillRect(rect.intersect(sceneRect), gradient);
// 	painter->setBrush(Qt::NoBrush);
// 	painter->drawRect(sceneRect);
}

void MindSpaceGraphWidget::scaleView(qreal scaleFactor)
{
	qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
	//qDebug() << "Scale factor:" <<factor;
	if (factor < 0.001 || factor > 100)
		return;
	
	scale(scaleFactor, scaleFactor);
}
