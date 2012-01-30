#include "MindSpaceGraphWidget.h"
#include "MindSpaceGraphEdge.h"
#include "MindSpaceGraphNode.h"

#include <QDebug>
#include <QGraphicsScene>
#include <QWheelEvent>

#include <math.h>

#include "MindSpace.h"
using namespace MindSpace;

MindSpaceGraphWidget::MindSpaceGraphWidget()
	: m_timerId(0)
{
	QGraphicsScene *scene = new QGraphicsScene(this);
	scene->setItemIndexMethod(QGraphicsScene::NoIndex);
	scene->setSceneRect(-1000, -1000, 2000, 2000);
	setScene(scene);
	setCacheMode(CacheBackground);
	setViewportUpdateMode(BoundingRectViewportUpdate);
	setRenderHint(QPainter::Antialiasing);
	setTransformationAnchor(AnchorUnderMouse);
	setResizeAnchor(AnchorViewCenter);
	
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
	
	_node("human",    MNodeType::ConceptNode());
	_node("man",      MNodeType::ConceptNode());
	_node("woman",    MNodeType::ConceptNode());
	_node("mortal",   MNodeType::ConceptNode());
	_node("air",      MNodeType::ConceptNode());
	_node("water",    MNodeType::ConceptNode());
	_node("Socrates", MNodeType::SpecificEntityNode());
	_node("Josiah",   MNodeType::SpecificEntityNode());
	_node("Ashley",   MNodeType::SpecificEntityNode());
	_node("breath",   MNodeType::PredicateNode());	// predicate nodes expect an incoming non-preciate node link and an outgoing non-precicate node link
	_node("love",     MNodeType::PredicateNode());
	
	new MLink(_node("Socrates"), _node("man"),    MLinkType::InheritsFromLink());
	new MLink(_node("Josiah"),   _node("man"),    MLinkType::InheritsFromLink());
	new MLink(_node("Ashley"),   _node("woman"),    MLinkType::InheritsFromLink());
	new MLink(_node("man"),      _node("human"), MLinkType::InheritsFromLink());
	new MLink(_node("woman"),    _node("human"), MLinkType::InheritsFromLink());
	new MLink(_node("human"),    _node("mortal"), MLinkType::InheritsFromLink());
// 	new MLink(_node("man"),      _node("breath"), MLinkType::PredicateSubjectLink());
// 	new MLink(_node("breath"),   _node("air"),    MLinkType::PredicateObjectLink());
// 	new MLink(_node("breath"),   _node("water"),  MLinkType::PredicateObjectLink(), MTruthValue(0.1));
// 	MLink *link = new MLink(_node("breath"),   _node("air"),    MLinkType::EvaluationLink());
// 	link->setArguments( QList<MNode*>() << _node("man") << _node("air") );

	//new MLink(_node("breath"),   QList<MNode*>() << _node("man") << _node("air"),    MLinkType::EvaluationLink());
	//new MLink(_node("breath"),   QList<MNode*>() << _node("man") << _node("water"),  MLinkType::EvaluationLink(), MTruthValue(0.1));
	
	new MLink(_node("human"),   QList<MNode*>() << _node("breath") << _node("air"),    MLinkType::PredicateLink());
	new MLink(_node("human"),   QList<MNode*>() << _node("breath") << _node("water"),  MLinkType::PredicateLink(), MTruthValue(0.1));
	new MLink(_node("Josiah"),   QList<MNode*>() << _node("love") << _node("Ashley"),  MLinkType::PredicateLink());
	
// 	qDebug() << _node("Socrates");
// 	qDebug() << _node("man");
// 	qDebug() << _node("woman");
// 	qDebug() << _node("mortal");
// 	qDebug() << _node("air");
// 	qDebug() << _node("water");
// 	qDebug() << _node("breath");
	
	QHash<MNode*,MindSpaceGraphNode*> graphNodes;
	
	QHash<QString,MNode*> nodes = MNode::nodes();
	foreach(MNode *node, nodes)
	{
		MindSpaceGraphNode *graphNode;
		graphNode = new MindSpaceGraphNode(this, node->content());
		graphNode->setPos(rand() % 400 - 200, rand() % 400 - 200);
		scene->addItem(graphNode);
		graphNodes[node] = graphNode;
		
		m_centerNode = graphNode; // for use in addTestItem()
	}
	
	QList<MLink*> linksProcessed;
	foreach(MNode *node, nodes)
	{
		const QList<MLink*> & links = node->links();
		foreach(MLink *link, links)
		{
			if(linksProcessed.contains(link))
				continue;
			
			MindSpaceGraphNode *startNode = graphNodes[link->node1()];
			
			QList<MNode*> args = link->arguments();
			if(!args.isEmpty())
			{
				double value = link->truthValue().value();
			
				MindSpaceGraphNode *typeNode;
				typeNode = new MindSpaceGraphNode(this, QString("%2 %1").arg(value<1.0?QString::number(value):"").arg(link->type().name()));
				//typeNode = new MindSpaceGraphNode(this, QString("%1%2").arg(value<1.0?"!":"").arg(link->type().name()));
				
				typeNode->setPos(rand() % 400 - 200, rand() % 400 - 200);
				typeNode->setColor(QColor(255,150,50)); // orange
				scene->addItem(typeNode);
				
				MindSpaceGraphEdge *edge = new MindSpaceGraphEdge(startNode, typeNode);
				scene->addItem(edge);
				edge->setWeight(value * 2.0);
				//edge->setLabel(link->type().name());
				
				foreach(MNode* node2, args)
				{
					MindSpaceGraphNode *endNode = graphNodes[node2];
					//scene->addItem(new MindSpaceGraphEdge(typeNode, endNode));
					
					MindSpaceGraphEdge *edge = new MindSpaceGraphEdge(typeNode, endNode);
					scene->addItem(edge);
					edge->setWeight(value * 2.0);
				}
			}
			else
			{
				double value = link->truthValue().value();
				
				MindSpaceGraphNode *endNode = graphNodes[link->node2()];
				
				MindSpaceGraphEdge *edge = new MindSpaceGraphEdge(startNode, endNode);
				scene->addItem(edge);
				edge->setWeight(value * 2.0);
				edge->setLabel(QString("%1 %2").arg(link->type().name()).arg(value<1.0?QString::number(value):""));
			}
			
			linksProcessed << link;
		}
	}
	
	
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

void MindSpaceGraphWidget::addTestItem()
{
	MindSpaceGraphNode *graphNode;
	graphNode = new MindSpaceGraphNode(this, "** TEST **");
	graphNode->setPos(rand() % 400 - 200, rand() % 400 - 200);
	scene()->addItem(graphNode);
	
	
	scene()->addItem(new MindSpaceGraphEdge(graphNode, m_centerNode));
}

void MindSpaceGraphWidget::itemMoved()
{
	if (!m_timerId)
		m_timerId = startTimer(1000 / 25);
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
	
	foreach (MindSpaceGraphNode *node, nodes)
		node->calculateForces();
	
	bool itemsMoved = false;
	foreach (MindSpaceGraphNode *node, nodes) 
	{
		if (node->advance())
			itemsMoved = true;
	}
	
	if (!itemsMoved) 
	{
		killTimer(m_timerId);
		m_timerId = 0;
	}
}

void MindSpaceGraphWidget::wheelEvent(QWheelEvent *event)
{
	scaleView(pow((double)2, -event->delta() / 240.0));
}

void MindSpaceGraphWidget::drawBackground(QPainter *painter, const QRectF &rect)
{
	Q_UNUSED(rect);
	
	// Shadow
	QRectF sceneRect = this->sceneRect();
	QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
	QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
	if (rightShadow.intersects(rect) || rightShadow.contains(rect))
		painter->fillRect(rightShadow, Qt::darkGray);
	if (bottomShadow.intersects(rect) || bottomShadow.contains(rect))
		painter->fillRect(bottomShadow, Qt::darkGray);
	
	// Fill
	QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
	gradient.setColorAt(0, Qt::white);
	gradient.setColorAt(1, Qt::lightGray);
	painter->fillRect(rect.intersect(sceneRect), gradient);
	painter->setBrush(Qt::NoBrush);
	painter->drawRect(sceneRect);
	
// 	// Text
// 	QRectF textRect(sceneRect.left() + 4, sceneRect.top() + 4,
// 			sceneRect.width() - 4, sceneRect.height() - 4);
// 	QString message(tr("Click and drag the nodes around, and zoom with the mouse "
// 			"wheel or the '+' and '-' keys"));
// 	
// 	QFont font = painter->font();
// 	font.setBold(true);
// 	font.setPointSize(14);
// 	painter->setFont(font);
// 	painter->setPen(Qt::lightGray);
// 	painter->drawText(textRect.translated(2, 2), message);
// 	painter->setPen(Qt::black);
// 	painter->drawText(textRect, message);
}

void MindSpaceGraphWidget::scaleView(qreal scaleFactor)
{
	qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
	if (factor < 0.07 || factor > 100)
		return;
	
	scale(scaleFactor, scaleFactor);
}
