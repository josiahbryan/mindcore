#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>

#include <QDebug>

#include "MindSpaceGraphEdge.h"
#include "MindSpaceGraphNode.h"
#include "MindSpaceGraphWidget.h"

MindSpaceGraphNode::MindSpaceGraphNode(MindSpaceGraphWidget *graphWidget, const QString &label)
	: QGraphicsItem()
	, m_graph(graphWidget)
	, m_weight(1.)
	, m_label(label)
	, m_color(Qt::green)
{
	setFlag(ItemIsMovable);
	setFlag(ItemSendsGeometryChanges);
	setCacheMode(DeviceCoordinateCache);
	setZValue(-1);
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
	
		QLineF line(mapFromItem(node, 0, 0), QPointF(0, 0));
		qreal dx = line.dx();
		qreal dy = line.dy();
		double l = 2.0 * (dx * dx + dy * dy);
		if (l > 0) 
		{
			// TODO Why 150? Magic number...
			xvel += (dx * 150.0) / l;
			yvel += (dy * 150.0) / l;
		}
	}
	
	// Now subtract all forces pulling items together
	// TODO Why 3.? Magic number...
	double weight = (double)(m_edgeList.size() + 1) * 5. * m_weight;
	//qDebug() << this << "Weight: "<<weight;

	foreach (MindSpaceGraphEdge *edge, m_edgeList) 
	{
		QPointF pos;
		if (edge->sourceNode() == this)
			pos = mapFromItem(edge->destNode(), 0, 0);
		else
			pos = mapFromItem(edge->sourceNode(), 0, 0);
		
		xvel += pos.x() / weight;
		yvel += pos.y() / weight;
	}
	
	if (qAbs(xvel) < 1 && qAbs(yvel) < 1)
		xvel = yvel = 0;
	
	// Keep inside the sceneRect()
	QRectF sceneRect = scene()->sceneRect();
	m_newPos = pos() + QPointF(xvel, yvel);
	m_newPos.setX(qMin(qMax(m_newPos.x(), sceneRect.left() + 10), sceneRect.right()  - 10));
	m_newPos.setY(qMin(qMax(m_newPos.y(), sceneRect.top()  + 10), sceneRect.bottom() - 10));
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
	painter->setPen(Qt::NoPen);
	painter->setBrush(QColor(0,0,0,100)); //Qt::darkGray);
	painter->drawEllipse(-9.5, -9.5, 20, 20);
	
	QRadialGradient gradient(-3, -3, 10);
	if (option->state & QStyle::State_Sunken) 
	{
		gradient.setCenter(3, 3);
		gradient.setFocalPoint(3, 3);
		gradient.setColorAt(1, m_color.darker(220));
		gradient.setColorAt(0, m_color.lighter(420));
	}
	else 
	{
		gradient.setColorAt(0, m_color.lighter(420));
		gradient.setColorAt(1, m_color.darker(120));
	}
	
	painter->setBrush(gradient);
	painter->setPen(QPen(Qt::black, 0));
	painter->drawEllipse(-10, -10, 20, 20);
	
	if(!m_label.isEmpty())
	{
		QFont font("",5);
		painter->setFont(font);
		//painter->setPen(Qt::gray);
		//painter->drawText(textRect.translated(2, 2), message);
		painter->setPen(Qt::black);
		//painter->drawText(textRect, message);

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
			m_graph->itemMoved();
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
