#ifndef SimpleBotAgent_H
#define SimpleBotAgent_H

#include <QtGui>

#include "MindSpace.h"
using namespace MindSpace;

class SimpleBotEnv;

class SimpleBotAgent : public QObject, public QGraphicsItem
{
	Q_OBJECT
public:
	SimpleBotAgent();
	
	void setEnv(SimpleBotEnv *env);
	void setMindSpace(MSpace *mspace);

	// QGraphicsItem::
	enum { Type = UserType + 1 };
	int type() const { return Type; }

	// QGraphicsItem::
	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

signals:
	void doubleClicked(SimpleBotAgent *);

public slots:
	void start();
	
private slots:
	void advance();

protected:

	// QGraphicsItem::
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
	
	// QGraphicsItem::
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	
	// QGraphicsItem::
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);



protected:
	SimpleBotEnv *m_env;
	MSpace *m_mspace;
	
	QString m_label;
	QColor m_color;
	
	//QPainterPath m_painterPath;
	
	QTimer m_advanceTimer;
	
	
	
	typedef enum {
		S_Unknown = 0,
		S_Resting,
		S_Searching,
		S_Eating,
		S_AskForMore,
	} StateType;
	
	StateType m_state;
	
	QString nameForState(StateType state);
	void setState(StateType state);
	double chooseVector();
	
	double m_hunger;
	double m_energy;
	
	QTime m_timer;
	QTime m_stateTimer;
	
	QPointF m_vec;
};

#endif
