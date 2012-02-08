#include "SimpleBotAgent.h"
#include "AgentSubsystems.h"

void AgentSubsystem::raiseException(const QString& message)
{
	m_agent->actionException(m_currentAction, message);
}

void AgentSubsystem::actionCompleted()
{
	m_agent->actionCompleted(m_currentAction);
}

///

void AgentBioSystem::initMindSpace()
{
	// Create the system node
	MSpace *ms = m_agent->mindSpace();
	m_node = ms->node("BioSystem", MNodeType::ConceptNode());
	ms->link(m_agent->node(), m_node, MLinkType::PartOf());
	
	// Create the hunger/energy variables
	m_hungerVar = ms->node("HungerValue", MNodeType::VariableNode());
	m_energyVar = ms->node("EnergyValue", MNodeType::VariableNode());
	
	// Setup intial values
	m_hungerVar->setData(1.0);
	m_energyVar->setData(1.0);
	
	// Link the variables to the system node
	ms->link(m_node, m_hungerVar, MLinkType::PartOf());
	ms->link(m_node, m_energyVar, MLinkType::PartOf());
	
	// Add an action to the system node
	MNode *act = ms->node("EatAction", MNodeType::ActionNode());
	ms->link(m_node, act, MLinkType::PartOf());
	
	// Variable for the eat action
	MNode *time = ms->node("EatTime", MNodeType::VariableNode());
	time->setData(100); // milliseconds
	ms->link(act, time, MLinkType::PartOf());
	
	m_actions << act;
	
	// Setup internal variables
	m_isEating = false;
	m_eatLength = -1;
}

AgentSubsystem::ActionInfo::ActionInfo(MNode *n)
{
	node = n;
	if(node)
	{
		name = node->content();
		vars = node->linkedNode(MNodeType::VariableNode());
		
		foreach(MNode *var, vars)
			varInfo[var->content()] = var->data().type();
	}
}

void AgentBioSystem::advance()
{
	if(m_isEating)
	{
		if(m_eatTimer.elapsed() >= m_eatLength)
		{
			m_isEating = false;
			//advance(); // re-enter because we're done resting;
			
			actionCompleted();
			
			return;
		}
		
		int time = m_eatFrameTimer.restart();
		
		// 5 seconds to 100% full
		m_hungerVar->setData( qMax(0., m_hungerVar->data().toDouble() - 0.1 * (((double)time) / 500.)) );
		
		if(m_hungerVar->data().toDouble() <= 0.0)
		{
			m_isEating = false;
			actionCompleted();
		}
	}
	
}

bool AgentBioSystem::executeAction(MNode *node)
{
	m_isEating = false;
	
	if(node->type() != MNodeType::ActionNode())
	{
		qDebug() << "AgentBioSystem::executeAction: Node given is not an ActionNode: "<<node;
		return false;
	}
	
	m_currentAction = node;
	
	if(node->content() == "EatAction")
	{
		// Get length of time
		MNode *time = node->firstLinkedNode("EatTime");
		
		// Error checking
		if(!time)
		{
			qDebug() << "AgentBioSystem::executeAction: Problem execting 'EatAction': Missing 'EatTime' variable: "<< node;
			return false;
		}
		
		// Setup timing values
		setupEatingState(time);
		
		return true;
	}
	else
	{
		qDebug() << "AgentBioSystem::executeAction: Unknown action: " << node->content(); 
	}
	
	return false;
}

void AgentBioSystem::setupEatingState(MNode *timeNode)
{
	if(!timeNode)
	{
		qDebug() << "AgentBioSystem::setupEatingState: Null input node";
		return;
	}
	
	int time = timeNode->data().toInt();
	
	m_isEating  = true;
	m_eatLength = time;
	m_eatTimer.restart();
	m_eatFrameTimer.restart();
}

bool AgentBioSystem::useEnergy(double speed) // 0-1
{
	// at 100% speed, deplete energy by 2.5%
	double amt = 0.025 * speed;
	double energy = m_energyVar->data().toDouble();
	if(energy < amt)
		return false;
		
	m_energyVar->setData( qMax(0., energy - amt) );
	
	// at 100% speed, hunger increased by 5%
	double hungerAmt = 0.05 * speed;
	double hunger = m_hungerVar->data().toDouble();
	m_hungerVar->setData( qMin(1., hunger + hungerAmt) );
	
	if(m_hungerVar->data().toDouble() >= 1.0)
		return false; // can't use more energy if too hungry
	
	return true; 
}

void AgentBioSystem::notifyResting(int ms) // milliseconds
{
	// 5 seconds to 100% energy
	m_energyVar->setData( qMin(1., m_energyVar->data().toDouble() + 0.1 * (((double)ms) / 500.)) );
	
	//if(m_energyVar->data().toDouble() >= 1.0)
	//	actionCompleted();
}

///

void AgentMovementSystem::initMindSpace()
{
	//qDebug() << "AgentMovementSystem::initMindSpace(): Bio ptr: "<< m_agent->subsystem(AgentBioSystem::className());
	
	// Create the system node
	MSpace *ms = m_agent->mindSpace();
	m_node = ms->node("MovementSystem", MNodeType::ConceptNode());
	ms->link(m_agent->node(), m_node, MLinkType::PartOf());
	
	// Create the variable nodes
	MNode *speed = ms->node("MoveSpeed", MNodeType::VariableNode());
	MNode *dir   = ms->node("MoveDirection", MNodeType::VariableNode());
	
	speed->setData(1.0); // 0-1
	dir->setData(90); // 0-359
	
	
	// Link the vars to the node
//	ms->link(m_node, speed, MLinkType::PartOf());
//	ms->link(m_node, dir, MLinkType::PartOf());
	
	// Create the action and link the vars to the act, and act to the system node
	MNode *act = ms->node("MoveAction", MNodeType::ActionNode());
	ms->link(act,    speed, MLinkType::PartOf());
	ms->link(act,    dir,   MLinkType::PartOf());
	ms->link(m_node, act,   MLinkType::PartOf());
	m_actions << act;
	
	// Create the resting action
	act = ms->node("RestAction", MNodeType::ActionNode());
	ms->link(m_node, act, MLinkType::PartOf());
	
	//qDebug() << "AgentMovementSystem::initMindSpace(): creating RestAction, act:"<<act<<", type:"<<act->type();
	
	// Variable for the rest action
	MNode *time = ms->node("RestTime", MNodeType::VariableNode());
	time->setData(100); // ms
	ms->link(act, time, MLinkType::PartOf());
	
	m_actions << act;
	
	// Setup our internal state
	m_vec = QPointF(1.,1.);
	m_isResting = false;
	m_restLength = -1;

	m_isMoving = false;
}

bool AgentMovementSystem::executeAction(MNode *node)
{
	m_isMoving = false;
	
	if(node->type() != MNodeType::ActionNode())
	{
		qDebug() << "AgentMovementSystem::executeAction: Node given is not an ActionNode: "<<node;
		return false;
	}
	
	m_currentAction = node;
	
	if(node->content() == "MoveAction")
	{
		// Get two variables - MoveSpeed and MoveDirection
		MNode *speed = node->firstLinkedNode("MoveSpeed");
		MNode *dir   = node->firstLinkedNode("MoveDirection");
		
		// Error checking
		if(!speed || !dir)
		{
			qDebug() << "AgentMovementSystem::executeAction: Problem execting 'MoveAction': One of the variables was missing: "<< qPrintable(MNode::toString(node, true));
			return false;
		}
		
		// Convert to a x/y movement vector
		setupMovementVector(speed, dir);
		
		m_isMoving = true;
		
		return true;
	}
	else
	if(node->content() == "RestAction")
	{
		// Get length of time
		MNode *time = node->firstLinkedNode("RestTime");
		
		// Error checking
		if(!time)
		{
			qDebug() << "AgentMovementSystem::executeAction: Problem execting 'RestAction': Missing 'RestTime' variable: "<< qPrintable(MNode::toString(node, true));
			return false;
		}
		
		// Setup timing values
		setupRestingState(time);
		
		return true;
	}
	else
	{
		qDebug() << "AgentMovementSystem::executeAction: Unknown action: "<<node->content(); 
	}
	
	return false;
}

void AgentMovementSystem::advance()
{
	AgentSubsystem *bioPtr = m_agent->subsystem(AgentBioSystem::className());
	AgentBioSystem *bio = dynamic_cast<AgentBioSystem*>(bioPtr);
	if(!bio)
	{
		qDebug() << "AgentMovementSystem::advance: Agent doesn't have a bio system, are we dead??";
		return;
	}
	
	if(m_isResting)
	{
		if(m_restTimer.elapsed() >= m_restLength)
		{
			m_isResting = false;
			advance(); // re-enter because we're done resting;
			
			actionCompleted();
			
			return;
		}
		
		bio->notifyResting(m_restFrameTimer.restart());
	}
	else
	if(m_isMoving)
	{
		if(!bio->isEating())
		{
			if(bio->useEnergy(m_speed))
			{
				changeAgentPosition();
			}
			else
			{
				qDebug() << "AgentMovementSystem::advance: Out of energy, can't move.";
				raiseException("Out of energy, can't move.");
			}
		}
		else
		{
			qDebug() << "AgentMovementSystem::advance: Eating, not moving.";
		}
	}
}

void AgentMovementSystem::changeAgentPosition()
{
// 	if(m_vec.isNull())
// 		chooseVector();
		
	QRectF sceneRect = m_agent->scene()->sceneRect();
	QPointF newPos = m_agent->pos() + m_vec; //QPointF(xvel, yvel);
	QPointF expect = newPos;
	newPos.setX(qMin(qMax(newPos.x(), sceneRect.left() + 10), sceneRect.right()  - 10));
	newPos.setY(qMin(qMax(newPos.y(), sceneRect.top()  + 10), sceneRect.bottom() - 10));
	
	if(newPos != expect)
	{
		qDebug() << "AgentMovementSystem::changeAgentPosition: Hit wall, can't move any more. Flagging error.";
		raiseException("Hit wall, can't move any more");
		
		//chooseVector();
	}
	
	m_agent->setPos(newPos);
}

void AgentMovementSystem::setupMovementVector(MNode *speedNode, MNode *angleNode)
{
	if(!speedNode || !angleNode)
	{
		qDebug() << "AgentMovementSystem::setupMovementVector: Null input nodes";
		return;
	}
	
	m_speed = speedNode->data().toDouble(); // expected 0-1
	m_angle = angleNode->data().toDouble(); // expected 0-359
	
	// X= R*cos(Theta)
	// Y= R*sin(Theta)
	const double SpeedRange = 9.0;
	double speedValue = m_speed * SpeedRange + 1.0;
	
	double x = speedValue * cos(m_angle);
	double y = speedValue * sin(m_angle);
	
	m_vec = QPointF(x,y);
}

void AgentMovementSystem::setupRestingState(MNode *timeNode)
{
	if(!timeNode)
	{
		qDebug() << "AgentMovementSystem::setupRestingState: Null input node";
		return;
	}
	
	int time = timeNode->data().toInt();
	
	m_isResting  = true;
	m_restLength = time;
	m_restTimer.restart();
	m_restFrameTimer.restart();
}

///

// void AgentTouchSystem::initMindSpace()
// {
// 	// Create the system node
// 	MSpace *ms = m_agent->mindSpace();
// 	m_node = ms->node("Touchsystem", MNodeType::ConceptNode());
// 	ms->link(m_agent->node(), m_node, MLinkType::PartOf());
// 	
// 	// Create and link the "sensor" node (variable) to the system node
// 	MNode *touch = ms->node("TouchSensor", MNodeType::VariableNode());
// 	ms->link(m_node, touch, MLinkType::PartOf());
// }
// 
// bool AgentTouchSystem::executeAction(MNode *node)
// {
// 	m_currentAction = node;
// 	return false;
// }
