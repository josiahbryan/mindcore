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

signals:
	void nodeDoubleClicked(MNode*);
	
	void layoutStarted();
	void layoutStep();
	void layoutStopped();
	
public slots:
	void scaleView(qreal scaleFactor);

	void addNode(MNode*);
	void removeNode(MNode*);
	
	void addLink(MLink*);
	void removeLink(MLink*);
	
	void clearScene();
	
	void stopLayout();
	
	void stepLayout();
	
private slots:
	void graphNodeDoubleClicked(MindSpaceGraphNode *);

	void linkTruthValueChanged(MTruthValue);

protected:
	void keyPressEvent(QKeyEvent *);
	void timerEvent(QTimerEvent *);
	void wheelEvent(QWheelEvent *);
	void drawBackground(QPainter *, const QRectF &);
	
	

private:
	
	int m_timerId;
	MindSpaceGraphNode *m_centerNode;
	MSpace *m_mindSpace;
	QHash<MNode*,MindSpaceGraphNode*> m_graphNodes;
	QHash<MindSpaceGraphNode*,MNode*> m_graphNodesReverse;
	QHash<MLink*,MindSpaceGraphEdgeData> m_graphLinks;
	QList<MLink*> m_linksProcessed;
	bool m_layoutStopped;
};

#endif
