#ifndef SimpleBotAgent_H
#define SimpleBotAgent_H

#include <QtGui>

#include "MindSpace.h"
using namespace MindSpace;

class SimpleBotEnv;
class SimpleBotAgent;

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
	
	// Aux feature: Info display item
	class InfoDisplay : public QGraphicsItem
	{
	protected:
		friend class SimpleBotAgent;
		InfoDisplay(SimpleBotAgent *agent);
		
		SimpleBotAgent *m_agent;
		QRect m_rect; 
		
	public:
		QRectF boundingRect() const { return m_rect; }
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	};
	
	// Retrieve the info display item
	InfoDisplay *infoItem();
	
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
	friend class InfoDisplay;
	
	SimpleBotEnv *m_env;
	MSpace *m_mspace;
	
	QString m_label;
	QColor m_color;
	
	QTimer m_advanceTimer;
	
	/* States:
	   - Ident
	   - Name
	   - How to "do" it
	   - Where to go from here
	   - What triggers transition
	*/
	
	class StateInfo {
	public:
		int x;
	};
	
	
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
	
	InfoDisplay *m_hud;
};

#endif
