#ifndef MindSpaceGraphWidget_H
#define MindSpaceGraphWidget_H

#include <QtGui/QGraphicsView>

class MindSpaceGraphNode;
class MindSpaceGraphEdge;
// namespace MindSpace
// {
// class MSpace;
// class MLink;
// class MNode;
// };
#include "MindSpace.h"
using namespace MindSpace;

class MindSpaceGraphEdgeData
{
public:
	MindSpaceGraphEdgeData() { node=0; }
	MindSpaceGraphNode *node;
	QList<MindSpaceGraphEdge *> edges;
};
	
	
class MindSpaceGraphWidget : public QGraphicsView
{
    Q_OBJECT

public:
	MindSpaceGraphWidget();
	
	void itemMoved(MindSpaceGraphNode *node=0);
	
	void setMindSpace(MSpace*);
	MSpace *mindSpace() { return m_mindSpace; }
	
	void mapNode(MNode *node, int levels=5, int currentLevel=0);

private slots:
	void addNode(MNode*);
	void removeNode(MNode*);
	
	void addLink(MLink*);
	void removeLink(MLink*);
	
	void clearScene();


protected:
	void keyPressEvent(QKeyEvent *);
	void timerEvent(QTimerEvent *);
	void wheelEvent(QWheelEvent *);
	void drawBackground(QPainter *, const QRectF &);
	
	void scaleView(qreal scaleFactor);

private:
	
	int m_timerId;
	MindSpaceGraphNode *m_centerNode;
	MSpace *m_mindSpace;
	QHash<MNode*,MindSpaceGraphNode*> m_graphNodes;
	QHash<MindSpaceGraphNode*,MNode*> m_graphNodesReverse;
	QHash<MLink*,MindSpaceGraphEdgeData> m_graphLinks;
	QList<MLink*> m_linksProcessed;
};

#endif
