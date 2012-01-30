#ifndef MindSpaceGraphEdge_H
#define MindSpaceGraphEdge_H

#include <QGraphicsItem>

class MindSpaceGraphNode;

class MindSpaceGraphEdge : public QGraphicsItem
{
public:
	MindSpaceGraphEdge(MindSpaceGraphNode *sourceNode, MindSpaceGraphNode *destNode);
	~MindSpaceGraphEdge();
	
	MindSpaceGraphNode *sourceNode() const;
	void setSourceNode(MindSpaceGraphNode *node);
	
	MindSpaceGraphNode *destNode() const;
	void setDestNode(MindSpaceGraphNode *node);
	
	void adjust();
	
	enum { Type = UserType + 2 };
	int type() const { return Type; }
	
protected:
	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
	MindSpaceGraphNode *m_source, *m_dest;
	
	QPointF m_sourcePoint;
	QPointF m_destPoint;
	qreal m_arrowSize;
};

#endif
