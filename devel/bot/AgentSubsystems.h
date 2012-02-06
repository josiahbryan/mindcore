#ifndef AgentSubsystems_H
#define AgentSubsystems_H

#include <QObject>
#include <QTime>
#include <QPointF>

#include "MindSpace.h"
using namespace MindSpace;

class SimpleBotAgent;

class AgentSubsystem : public QObject 
{
	Q_OBJECT
public:
	
	AgentSubsystem(SimpleBotAgent *agent) 
		: QObject()
		, m_agent(agent)
		, m_node(0) 
		{ }
		
	virtual ~AgentSubsystem() {}
	
	SimpleBotAgent *agent() { return m_agent; }
	
	// Return the classname of the current object
	QString className() { return metaObject()->className(); }
	
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
	
	class ActionInfo {
	public:
		ActionInfo(MNode *node=0);
		
		MNode *node; 
		QList<MNode*> vars; // Note: content() is the variable name and data().type() is variable type
		QString name;
		QHash<QString,QVariant::Type> varInfo;
	};
	
	virtual QList<AgentSubsystem::ActionInfo> actions() { return m_actions; }
	
	
protected:
	void raiseException(const QString& message="");
	void actionCompleted();

	SimpleBotAgent *m_agent;
	MNode *m_node;
	QList<AgentSubsystem::ActionInfo> m_actions;
	MNode *m_currentAction;
	
};

class AgentBioSystem : public AgentSubsystem
{
	Q_OBJECT
public:
	AgentBioSystem(SimpleBotAgent *agent) : AgentSubsystem(agent) {}
	 
	static QString className() { return staticMetaObject.className(); }
	
	QString name() { return "Biological"; }
	void initMindSpace();
	void advance();
	bool executeAction(MNode *);
	
	// Bio system specific
	bool useEnergy(double speed); // 0-1 (0%-100%)
	
	// Rest the system (increase energy)
	void notifyResting(int ms); // ms = milliseconds rested
	
	bool isEating() { return m_isEating; }
	
	double hunger() { return m_hungerVar->data().toDouble(); }
	double energy() { return m_energyVar->data().toDouble(); }
	
protected:
	MNode *m_hungerVar;
	MNode *m_energyVar;
	
	void setupEatingState(MNode *time);
	
	QTime m_eatTimer;
	QTime m_eatFrameTimer;
	bool m_isEating;
	int m_eatLength;
};

class AgentMovementSystem : public AgentSubsystem
{
	Q_OBJECT
public:
	AgentMovementSystem(SimpleBotAgent *agent) : AgentSubsystem(agent) {}
	
	static QString className() { return staticMetaObject.className(); }
	
	QString name() { return "Movement"; }
	void initMindSpace();
	bool executeAction(MNode *);
	
	// Actually move the agent
	void advance();
	
protected:
	void setupMovementVector(MNode *speed, MNode *dir);
	void setupRestingState(MNode *time);
	void changeAgentPosition();
	
	QPointF m_vec;
	double m_speed;
	double m_angle;
	
	QTime m_restTimer;
	QTime m_restFrameTimer;
	bool m_isResting;
	int m_restLength;
};

// class AgentTouchSystem : public AgentSubsystem
// {
// 	Q_OBJECT
// public:
// 	AgentTouchSystem(SimpleBotAgent *agent) : AgentSubsystem(agent) {}
// 	
// 	static QString className() { return staticMetaObject.className(); }
// 	
// 	QString name() { return "Touch"; }
// 	void initMindSpace();
// 	bool executeAction(MNode *);
// 	
// 	bool isTouchingSomething(); 
// };

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
