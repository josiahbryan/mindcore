#ifndef MindSpaceGraphNode_H
#define MindSpaceGraphNode_H

#include <QGraphicsItem>
#include <QList>

class MindSpaceGraphEdge;
class MindSpaceGraphWidget;
class QGraphicsSceneMouseEvent;

class MindSpaceGraphNode : public QObject, public QGraphicsItem
{
	Q_OBJECT
public:
	MindSpaceGraphNode(MindSpaceGraphWidget *graphWidget, const QString& label="");
	
	void addEdge(MindSpaceGraphEdge *edge);
	QList<MindSpaceGraphEdge *> edges() const;
	
	enum { Type = UserType + 1 };
	int type() const { return Type; }
	
	void calculateForces();
	bool advance();
	
	void setWeight(double w) { m_weight=w; };
	double weight() { return m_weight; }
	
	void setLabel(const QString& label) { m_label=label; }
	QString label() { return m_label; }
	
	void setColor(const QColor& c) { m_color=c; }
	QColor color() { return m_color; }
	
	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
	
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
	QList<MindSpaceGraphEdge *> m_edgeList;
	QPointF m_newPos;
	MindSpaceGraphWidget *m_graph;
	double m_weight;
	QString m_label;
	QColor m_color;
};

#endif
