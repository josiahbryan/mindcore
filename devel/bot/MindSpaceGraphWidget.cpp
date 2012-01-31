#include "MindSpaceGraphWidget.h"
#include "MindSpaceGraphEdge.h"
#include "MindSpaceGraphNode.h"

#include <QDebug>
#include <QGraphicsScene>
#include <QWheelEvent>

#include <QGLWidget>

#include <math.h>

#include "MindSpace.h"
using namespace MindSpace;

MindSpaceGraphWidget::MindSpaceGraphWidget()
	: m_timerId(0)
	, m_mindSpace(0)
{
	QGraphicsScene *scene = new QGraphicsScene(this);
	scene->setItemIndexMethod(QGraphicsScene::NoIndex);
	//scene->setSceneRect(-1000000, -1000000, 2000000, 2000000);
	setScene(scene);
	
	setCacheMode(CacheBackground);
	//setViewportUpdateMode(BoundingRectViewportUpdate);
	//setRenderHint(QPainter::Antialiasing);
	setTransformationAnchor(AnchorUnderMouse);
	setResizeAnchor(AnchorViewCenter);
	setDragMode(QGraphicsView::ScrollHandDrag);
	
	//setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );
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

	
// 	MindSpaceGraphNode *node1 = new MindSpaceGraphNode(this);
// 	MindSpaceGraphNode *node2 = new MindSpaceGraphNode(this);
// 	MindSpaceGraphNode *node3 = new MindSpaceGraphNode(this);
// 	MindSpaceGraphNode *node4 = new MindSpaceGraphNode(this);
// 	m_centerNode = new MindSpaceGraphNode(this);
// 	
// 	MindSpaceGraphNode *node12 = new MindSpaceGraphNode(this);
// 	MindSpaceGraphNode *node22 = new MindSpaceGraphNode(this);
// 	MindSpaceGraphNode *node32 = new MindSpaceGraphNode(this);
// 	MindSpaceGraphNode *node42 = new MindSpaceGraphNode(this);
// 	MindSpaceGraphNode *centerNode2 = new MindSpaceGraphNode(this);
// 	
// 	scene->addItem(node1);
// 	scene->addItem(node2);
// 	scene->addItem(node3);
// 	scene->addItem(node4);
// 	scene->addItem(m_centerNode);
// 	
// 	scene->addItem(node12);
// 	scene->addItem(node22);
// 	scene->addItem(node32);
// 	scene->addItem(node42);
// 	scene->addItem(centerNode2);
// 	
// 	scene->addItem(new MindSpaceGraphEdge(node1, m_centerNode));
// 	scene->addItem(new MindSpaceGraphEdge(node2, m_centerNode));
// 	scene->addItem(new MindSpaceGraphEdge(node3, m_centerNode));
// 	scene->addItem(new MindSpaceGraphEdge(node4, m_centerNode));
// 	
// 	scene->addItem(new MindSpaceGraphEdge(node12, centerNode2));
// 	scene->addItem(new MindSpaceGraphEdge(node22, centerNode2));
// 	scene->addItem(new MindSpaceGraphEdge(node32, centerNode2));
// 	scene->addItem(new MindSpaceGraphEdge(node42, centerNode2));
// 	
// 	scene->addItem(new MindSpaceGraphEdge(m_centerNode, centerNode2));
// 	
// 	node1->setPos(-50, -50);
// 	node2->setPos(0, -50);
// 	node3->setPos(50, -50);
// 	node4->setPos(60, -50);
// 	m_centerNode->setPos(0, 0);
// 	
// 	node12->setPos(-50, 50);
// 	node22->setPos(0, 50);
// 	node32->setPos(50, 50);
// 	node42->setPos(60, 50);
// 	centerNode2->setPos(0, 50);
	
	//srand((unsigned)time(0));
	srand(3729);
	
	
// 	qDebug() << _node("Socrates");
// 	qDebug() << _node("man");
// 	qDebug() << _node("woman");
// 	qDebug() << _node("mortal");
// 	qDebug() << _node("air");
// 	qDebug() << _node("water");
// 	qDebug() << _node("breath");
	
	
	
	// MindSpaceGraphNode *node4 = new MindSpaceGraphNode(this);
	// 	scene->addItem(new MindSpaceGraphEdge(node32, centerNode2));
	
	//     MindSpaceGraphNode *node1 = new MindSpaceGraphNode(this);
	//     MindSpaceGraphNode *node2 = new MindSpaceGraphNode(this);
	//     MindSpaceGraphNode *node3 = new MindSpaceGraphNode(this);
	//     MindSpaceGraphNode *node4 = new MindSpaceGraphNode(this);
	//     m_centerNode = new MindSpaceGraphNode(this);
	//     MindSpaceGraphNode *node6 = new MindSpaceGraphNode(this);
	//     MindSpaceGraphNode *node7 = new MindSpaceGraphNode(this);
	//     MindSpaceGraphNode *node8 = new MindSpaceGraphNode(this);
	//     MindSpaceGraphNode *node9 = new MindSpaceGraphNode(this);
	//     scene->addItem(node1);
	//     scene->addItem(node2);
	//     scene->addItem(node3);
	//     scene->addItem(node4);
	//     scene->addItem(m_centerNode);
	//     scene->addItem(node6);
	//     scene->addItem(node7);
	//     scene->addItem(node8);
	//     scene->addItem(node9);
	//     scene->addItem(new MindSpaceGraphEdge(node1, node2));
	//     scene->addItem(new MindSpaceGraphEdge(node2, node3));
	//     scene->addItem(new MindSpaceGraphEdge(node2, m_centerNode));
	//     scene->addItem(new MindSpaceGraphEdge(node3, node6));
	//     scene->addItem(new MindSpaceGraphEdge(node4, node1));
	//     scene->addItem(new MindSpaceGraphEdge(node4, m_centerNode));
	//     scene->addItem(new MindSpaceGraphEdge(m_centerNode, node6));
	//     scene->addItem(new MindSpaceGraphEdge(m_centerNode, node8));
	//     scene->addItem(new MindSpaceGraphEdge(node6, node9));
	//     scene->addItem(new MindSpaceGraphEdge(node7, node4));
	//     scene->addItem(new MindSpaceGraphEdge(node8, node7));
	//     scene->addItem(new MindSpaceGraphEdge(node9, node8));
	// 
	// //     node1->setPos(-50, -50);
	// //     node2->setPos(0, -50);
	// //     node3->setPos(50, -50);
	// //     node4->setPos(-50, 0);
	// //     m_centerNode->setPos(0, 0);
	// //     node6->setPos(50, 0);
	// //     node7->setPos(-50, 50);
	// //     node8->setPos(0, 50);
	// //     node9->setPos(50, 50);
	// 
	// // node1->setPos(50, 50);
	// //     node2->setPos(50, 50);
	// //     node3->setPos(350, 5032);
	// //     node4->setPos(50, 51240);
	// //     m_centerNode->setPos(1, 1);
	// //     node6->setPos(52, 50);
	// //     node7->setPos(50, 530);
	// //     node8->setPos(5725, 50);
	// //     node9->setPos(50, 520);
	//     
	//     node2->setWeight(5);
	//     node9->setWeight(10);
	//     node7->setWeight(2);
	
	scale(qreal(0.8), qreal(0.8));
	setMinimumSize(400, 400);
	setWindowTitle(tr("Elastic Nodes"));
}

void MindSpaceGraphWidget::addNode(MNode *node)
{
	MindSpaceGraphNode *graphNode;
	graphNode = new MindSpaceGraphNode(this, node->content());
	
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
		
		
		
		typeNode->setColor(QColor(255,150,50)); // orange
		scene()->addItem(typeNode);
		
		data.node = typeNode;
		
		MindSpaceGraphEdge *edge = new MindSpaceGraphEdge(startNode, typeNode);
		scene()->addItem(edge);
		edge->setWeight(value * 2.0);
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
			edge->setWeight(value * 2.0);
			
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
			edge->setWeight(value * 2.0);
			edge->setLabel(QString("%1 %2").arg(link->type().name()).arg(value<1.0?QString::number(value):""));
			
			data.edges << edge;
		}
	}
	
	m_graphLinks[link] = data;	
}

void MindSpaceGraphWidget::removeLink(MLink *link)
{
	if(!m_graphLinks.contains(link))
		return;
		
	MindSpaceGraphEdgeData data = m_graphLinks[link];
	foreach(MindSpaceGraphEdge *edge, data.edges)
		scene()->removeItem(edge);
	qDeleteAll(data.edges);
	
	if(data.node)
	{
		scene()->removeItem(data.node);
		delete data.node;
	}
	
	m_graphLinks.remove(link);
}

void MindSpaceGraphWidget::clearScene()
{
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
	{
		m_linksProcessed.clear();
		setMindSpace(0);
	}
	
	qDebug() << "MindSpaceGraphWidget::mapNode: level:"<<currentLevel<<"/"<<levels<<": (size: "<<m_graphNodes.keys().size()<<"nodes/"<<m_graphLinks.keys().size()<<"links) node:"<<node;
	
	addNode(node);
	
	if(currentLevel >= levels)
	{
		qDebug() << "MindSpaceGraphWidget::mapNode: Reached end, not mapping links";
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
		m_timerId = startTimer(1000 / 10);
		
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
	case Qt::Key_Up:
		m_centerNode->moveBy(0, -20);
		break;
	case Qt::Key_Down:
		m_centerNode->moveBy(0, 20);
		break;
	case Qt::Key_Left:
		m_centerNode->moveBy(-20, 0);
		break;
	case Qt::Key_Right:
		m_centerNode->moveBy(20, 0);
		break;
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
	
	QList<MindSpaceGraphNode *> nodes;
	foreach (QGraphicsItem *item, scene()->items()) 
	{
		if (MindSpaceGraphNode *node = qgraphicsitem_cast<MindSpaceGraphNode *>(item))
			nodes << node;
	}
	
	bool itemsMoved = true;
	
	//qDebug() << "TimerEvent start";
	
	//while(itemsMoved)
	{
		foreach (MindSpaceGraphNode *node, nodes)
			node->calculateForces();
		
		foreach (MindSpaceGraphNode *node, nodes) 
		{
			if (!node->advance())
			{
				itemsMoved = false;
				
	
			}
		}
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
	
	if (!itemsMoved) 
	{
		killTimer(m_timerId);
		m_timerId = 0;
	}
	
	//qDebug() << "TimerEvent end";
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
	qDebug() << "Scale factor:" <<factor;
	if (factor < 0.001 || factor > 100)
		return;
	
	scale(scaleFactor, scaleFactor);
}
