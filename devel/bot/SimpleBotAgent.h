#ifndef SimpleBotAgent_H
#define SimpleBotAgent_H

#include <QtGui>

#include "MindSpace.h"
using namespace MindSpace;

class SimpleBotEnv;
class SimpleBotAgent;
class AgentSubsystem;

#define StateUnknown   SimpleBotAgent::StateInfo(0x001, "unknown")
#define StateResting   SimpleBotAgent::StateInfo(0x002, "resting")
#define StateSearching SimpleBotAgent::StateInfo(0x003, "searching")
#define StateEating    SimpleBotAgent::StateInfo(0x004, "eating")
#define StateAsking    SimpleBotAgent::StateInfo(0x005, "asking")

class SimpleBotAgent : public QObject, 
		       public QGraphicsItem
{
	Q_OBJECT
public:
	SimpleBotAgent(MSpace *mspace=0);
	
	void setEnv(SimpleBotEnv *env);
	//void setMindSpace(MSpace *mspace);
	MSpace *mindSpace() { return m_mspace; }
	
	MNode *node() { return m_node; }
	
	AgentSubsystem *subsystem(const QString& name) { return m_subsysHash[name]; }
	
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
	
	
	void actionException(MNode *currentAction, MNode *exceptionVar, QVariant targetVal, const QString& message);
	
	MNode *currentAction() { return m_currentAction; }
	
	void actionCompleted(MNode *currentAction);
	
	
	
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
	
	
	InfoDisplay *m_hud;
	
	QList<AgentSubsystem*> m_subsystems;
	QHash<QString,AgentSubsystem*> m_subsysHash;
	
	void initSubsystems();
	void addSubsystem(AgentSubsystem*);
	void initGoals();
	
	// Choose an MNode* to be current goal
	void chooseCurrentGoal();
	MNode *m_currentGoal;
	
	// called by chooseCurrentGoal(),
	// based on goal, find set up actions which give the highest probability of achieving current goal
	void chooseAction();
	// chooseAction() could also be triggered by actionException() above
	MNode *m_currentAction;
	
	// Calculate the probability that the MNode *action will achieve/help toward  m_currentGoal 
	double calcGoalActionProb(MNode *goal, MNode *action);
	
	double m_lastHunger;
	double m_lastEnergy;
	
	QList<MNode*> m_goals;
	
	QHash<MNode*,QVariant> m_goalVarSnapshot;
	
	MNode *m_node;
	
	MNode *m_currentGoalMemory;
	
	MNode *m_exceptionVar;
	
	QList<MNode*> m_goalStack;
	
	void setCurrentGoal(MNode *goal);
	//QList<MLink*> siblingGoals(MNode *goal);
	MNode *evaulateSiblingGoalLinks(MNode *currentNode);
	
	MNode *m_contextNode;
	void initContextNode(); 
	
};

#endif
