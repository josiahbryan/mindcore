#include <QPainter>

#include "MindSpaceGraphEdge.h"
#include "MindSpaceGraphNode.h"

#include <math.h>

static const double Pi = 3.14159265358979323846264338327950288419717;
static double TwoPi = 2.0 * Pi;

MindSpaceGraphEdge::MindSpaceGraphEdge(MindSpaceGraphNode *sourceNode, MindSpaceGraphNode *destNode)
	: m_arrowSize(10)
{
	setAcceptedMouseButtons(0);
	m_source = sourceNode;
	m_dest = destNode;
	m_source->addEdge(this);
	m_dest->addEdge(this);
	adjust();
}

MindSpaceGraphEdge::~MindSpaceGraphEdge()
{
}

MindSpaceGraphNode *MindSpaceGraphEdge::sourceNode() const
{
	return m_source;
}

void MindSpaceGraphEdge::setSourceNode(MindSpaceGraphNode *node)
{
	m_source = node;
	adjust();
}

MindSpaceGraphNode *MindSpaceGraphEdge::destNode() const
{
	return m_dest;
}

void MindSpaceGraphEdge::setDestNode(MindSpaceGraphNode *node)
{
	m_dest = node;
	adjust();
}

void MindSpaceGraphEdge::adjust()
{
	if (!m_source || !m_dest)
		return;
	
	QLineF line(mapFromItem(m_source, 0, 0), mapFromItem(m_dest, 0, 0));
	qreal length = line.length();
	
	prepareGeometryChange();
	
	if (length > qreal(20.)) 
	{
		QPointF edgeOffset((line.dx() * 10) / length, (line.dy() * 10) / length);
		m_sourcePoint = line.p1() + edgeOffset;
		m_destPoint = line.p2() - edgeOffset;
	}
	else 
	{
		m_sourcePoint = m_destPoint = line.p1();
	}
}

QRectF MindSpaceGraphEdge::boundingRect() const
{
	if (!m_source || !m_dest)
		return QRectF();
	
	qreal penWidth = 1;
	qreal extra = (penWidth + m_arrowSize) / 2.0;
	
	return QRectF(m_sourcePoint, QSizeF(m_destPoint.x() - m_sourcePoint.x(),
					    m_destPoint.y() - m_sourcePoint.y()))
		.normalized()
		.adjusted(-extra, -extra, extra, extra);
}

void MindSpaceGraphEdge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	if (!m_source || !m_dest)
		return;
	
	QLineF line(m_sourcePoint, m_destPoint);
	if (qFuzzyCompare(line.length(), qreal(0.)))
		return;
	
	// Draw the line itself
	painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	painter->drawLine(line);
	
	// Draw the arrows
	double angle = ::acos(line.dx() / line.length());
	if (line.dy() >= 0)
		angle = TwoPi - angle;
	
	QPointF sourceArrowP1 = m_sourcePoint + QPointF(sin(angle + Pi / 3) * m_arrowSize,
							cos(angle + Pi / 3) * m_arrowSize);
							
	QPointF sourceArrowP2 = m_sourcePoint + QPointF(sin(angle + Pi - Pi / 3) * m_arrowSize,
							cos(angle + Pi - Pi / 3) * m_arrowSize);
							
	QPointF destArrowP1 = m_destPoint + QPointF(sin(angle - Pi / 3) * m_arrowSize,
						    cos(angle - Pi / 3) * m_arrowSize);

	QPointF destArrowP2 = m_destPoint + QPointF(sin(angle - Pi + Pi / 3) * m_arrowSize,
						    cos(angle - Pi + Pi / 3) * m_arrowSize);
	
	painter->setBrush(Qt::black);
	//painter->drawPolygon(QPolygonF() << line.p1() << sourceArrowP1 << sourceArrowP2);
	painter->drawPolygon(QPolygonF() << line.p2() << destArrowP1 << destArrowP2);
}
