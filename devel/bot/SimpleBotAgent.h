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
	
	class StateInfo {
	public:
		StateInfo(int _id=-1, QString _name="") 
			: id(_id)
			, name(_name)
			{}
		bool isNull() { return id<0; }
		
		int id;
		QString name;
	};
	
	StateInfo state() { return m_state; }
	
	
	
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
	
// 	typedef enum {
// 		S_Unknown = 0,
// 		S_Resting,
// 		S_Searching,
// 		S_Eating,
// 		S_AskForMore,
// 	} StateType;
// 	
// 	StateType m_state;

	StateInfo m_state;
	
	//QString nameForState(StateType state);
	void setState(StateInfo state);
	double chooseVector();
	
	void updateHungerEnergyState();
	double m_decayRate;
	
	void processSearching();
	void processResting();
	void processEating();
	
	void evaulateStateChangeRequired();
	
	double m_hunger;
	double m_energy;
	
	QTime m_timer;
	QTime m_stateTimer;
	
	QPointF m_vec;
	
	InfoDisplay *m_hud;
	
	QList<AgentSubsystem*> m_subsystems;
	QHash<QString,AgentSubsystem*> m_subsysHash;
	
	void setupSubsystems();
	void addSubsystem(AgentSubsystem*);
	void initGoals();
	
	QList<MNode*> m_goals;
	
	MNode *m_node;
	
};

bool operator==(SimpleBotAgent::StateInfo a, SimpleBotAgent::StateInfo b); 
bool operator!=(SimpleBotAgent::StateInfo a, SimpleBotAgent::StateInfo b);
bool operator!(SimpleBotAgent::StateInfo a);
//QDebug operator<<(QDebug dbg, SimpleBotAgent::StateInfo state);
	

class AgentSubsystem : public QObject 
{
	Q_OBJECT
public:
	
	AgentSubsystem(SimpleBotAgent *agent) 
		: QObject(agent)
		, m_agent(agent)
		, m_node(0) 
		{ }
		
	virtual ~AgentSubsystem() {}
	
	SimpleBotAgent *agent() { return m_agent; }
	
	// Unique name of this subsystem
	virtual QString name() { return "?"; }
	
	// Setup the mindspace of the agent (agent->mindSpace()) with any action nodes and a node represeting this system linked to the agent
	// TODO: Shouldn't this be implicitly called by ctor?
	virtual void initMindSpace() {}
	
	// Clock tick
	virtual void advance() {}
	
	// Return the node for this system
	// TODO: Is this even needed outside the subsystem?
	virtual MNode *node() { return m_node; }
	
	// Execute the action described by the MNode given
	// May return immediately and execute the action over time.
	// May return false if the subsystem cannot handle the given action
	// Must never return true and ingore the action
	virtual bool executeAction(MNode *) { return false; }
	
	// The subsystem has access to the agents mindspace outside of executeAction(), e.g. in a slot for updating the mindspace asyncronously
	
protected:
	SimpleBotAgent *m_agent;
	MNode *m_node;
	
};

class AgentBioSystem : public AgentSubsystem
{
	Q_OBJECT
public:
	AgentBioSystem(SimpleBotAgent *agent) : AgentSubsystem(agent) {}
	 
	QString name() { return "Biological"; }
	void initMindSpace();
	void advance();
	bool executeAction(MNode *);
	
protected:
	MNode *m_hungerVar;
	MNode *m_energyVar;
};

class AgentMovementSystem : public AgentSubsystem
{
	Q_OBJECT
public:
	AgentMovementSystem(SimpleBotAgent *agent) : AgentSubsystem(agent) {}
	
	QString name() { return "Movement"; }
	void initMindSpace();
	bool executeAction(MNode *);
};

class AgentTouchSystem : public AgentSubsystem
{
	Q_OBJECT
public:
	AgentTouchSystem(SimpleBotAgent *agent) : AgentSubsystem(agent) {}
	
	QString name() { return "Touch"; }
	void initMindSpace();
	bool executeAction(MNode *);
};

/*
class AgentHearingSystem : public AgnetSusbsystem
{
	Q_OBJECT
public:
	QString name() { return "Hearing"; }
	void initMindSpace();
	bool executeAction(MNode *);
};
*/

/*
class AgentSpeechSystem : public AgnetSusbsystem
{
	Q_OBJECT
public:
	QString name() { return "Speech"; }
	void initMindSpace();
	bool executeAction(MNode *);
};
*/


#endif
