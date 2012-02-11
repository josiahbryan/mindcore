#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>

#include <QDebug>

#include "MindSpaceGraphEdge.h"
#include "MindSpaceGraphNode.h"
#include "MindSpaceGraphWidget.h"

MindSpaceGraphNode::MindSpaceGraphNode(MindSpaceGraphWidget *graphWidget, const QString &label)
	: QObject(),
	  QGraphicsItem()
	, m_graph(graphWidget)
	, m_weight(1.)
	, m_label(label)
	, m_color(Qt::green)
{
	setFlag(ItemIsMovable);
	setFlag(ItemSendsGeometryChanges);
	setCacheMode(DeviceCoordinateCache);
	setZValue(99);
}

void MindSpaceGraphNode::addEdge(MindSpaceGraphEdge *edge)
{
	m_edgeList << edge;
	edge->adjust();
}

QList<MindSpaceGraphEdge *> MindSpaceGraphNode::edges() const
{
	return m_edgeList;
}

void MindSpaceGraphNode::calculateForces()
{
	if (!scene() || scene()->mouseGrabberItem() == this) 
	{
		m_newPos = pos();
		return;
	}
	
	// Sum up all forces pushing this item away
	qreal xvel = 0;
	qreal yvel = 0;
	foreach (QGraphicsItem *item, scene()->items()) 
	{
		MindSpaceGraphNode *node = qgraphicsitem_cast<MindSpaceGraphNode *>(item);
		if (!node)
			continue;
	
		QLineF line(node->pos(), pos());
		qreal dx = line.dx();
		qreal dy = line.dy();
		double l = 2.0 * (dx * dx + dy * dy); //  TODO is 2.0 a magic number here ...?
		if (l > 0) 
		{
			// TODO Why 150? Magic number here...
			xvel += (dx * 150.0) / l;
			yvel += (dy * 150.0) / l;
		}
	}
	
	// Now subtract all forces pulling items together
	// TODO Magic number...
	double weight = (double)(m_edgeList.size() + 1) * 2. * m_weight;
	//qDebug() << this << "Weight: "<<weight;

	// TODO Should we factor in the weight of the edge? (edge->weight())
	foreach (MindSpaceGraphEdge *edge, m_edgeList) 
	{
		QPointF pos;
		if (edge->sourceNode() == this)
			pos = edge->destNode()->pos() - this->pos();
		else
			pos = edge->sourceNode()->pos() - this->pos();
		
		xvel += pos.x() / weight;
		yvel += pos.y() / weight;
	}
	
// 	// Add an invisible edge pulling toward the center of the scene
// 	QPointF centerVec = scene()->sceneRect().center() - pos();
// 	xvel += centerVec.x() / weight;
// 	yvel += centerVec.y() / weight;
	
	if (qAbs(xvel) < 1. && qAbs(yvel) < 1.)
		xvel = yvel = 0;
	
	m_newPos = pos() + QPointF(xvel, yvel);
}

bool MindSpaceGraphNode::advance()
{
	if (m_newPos == pos())
		return false;
	
	setPos(m_newPos);
	return true;
}

QRectF MindSpaceGraphNode::boundingRect() const
{
	qreal adjust = 2;
	QRectF shapeRect = QRectF(-10 - adjust, -10 - adjust,
		                   23 + adjust,  23 + adjust);

	QFont font("", 5);
	QFontMetrics metrics(font);
	QRectF textRect = QRectF(metrics.boundingRect(m_label));
	textRect.moveTo(-8,2);
	
	return shapeRect.united(textRect.adjusted(-adjust,-adjust,+adjust,+adjust));
	
}

QPainterPath MindSpaceGraphNode::shape() const
{
	QPainterPath path;
	path.addEllipse(-10, -10, 20, 20);
	return path;
}

void MindSpaceGraphNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{	
	painter->setBrush(m_color);
	painter->setPen(QPen(Qt::black, 0));
	painter->drawEllipse(-10, -10, 20, 20);
	//painter->fillRect(-10, -10, 20, 20, m_color);
	
	if(!m_label.isEmpty())
	{
		QFont font("",5);
		painter->setFont(font);
		
		painter->setPen(Qt::white);
		painter->drawText(-7,3,m_label);
		painter->setPen(Qt::black);
		painter->drawText(-8,2,m_label);
	}
}

QVariant MindSpaceGraphNode::itemChange(GraphicsItemChange change, const QVariant &value)
{
	switch (change) 
	{
		case ItemPositionHasChanged:
			foreach (MindSpaceGraphEdge *edge, m_edgeList)
				edge->adjust();
			m_graph->itemMoved(this);
			break;
		default:
			break;
	};
	
	return QGraphicsItem::itemChange(change, value);
}

void MindSpaceGraphNode::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	update();
	QGraphicsItem::mousePressEvent(event);
}

void MindSpaceGraphNode::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	update();
	QGraphicsItem::mouseReleaseEvent(event);
}

void MindSpaceGraphNode::mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event )
{
	emit doubleClicked(this);
	//QGraphicsItem::mouseDoubleClickEvent(event);
}
