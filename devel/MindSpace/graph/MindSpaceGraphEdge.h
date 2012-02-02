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
	
	void setWeight(double w) { m_weight = w; update(); }
	double weight() const { return m_weight; }
	
	void setLabel(const QString& label) { m_label=label; update(); }
	QString label() { return m_label; }
	
	void setColor(const QColor& c) { m_color=c; }
	QColor color() { return m_color; }
	
	
protected:
	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
	MindSpaceGraphNode *m_source, *m_dest;
	
	QPointF m_sourcePoint;
	QPointF m_destPoint;
	qreal m_arrowSize;
	double m_weight;
	QString m_label;
	QColor m_color;
	
};

#endif
