#include "SimpleBotAgent.h"
#include "SimpleBotEnv.h"
#include "AgentSubsystems.h"

#include <math.h>

bool operator==(SimpleBotAgent::StateInfo a, SimpleBotAgent::StateInfo b) { return a.id==b.id; } 
bool operator!=(SimpleBotAgent::StateInfo a, SimpleBotAgent::StateInfo b) { return a.id!=b.id; }
bool operator!(SimpleBotAgent::StateInfo a) { return a.isNull(); }

SimpleBotAgent::InfoDisplay::InfoDisplay(SimpleBotAgent *agent)
{
	m_agent = agent;
	m_rect  = QRect(10,10,160,80);
	setPos(0,0);
	setZValue(150);
}
 
void SimpleBotAgent::InfoDisplay::paint(QPainter *p, const QStyleOptionGraphicsItem */*option*/, QWidget *)
{
	SimpleBotAgent *agent = m_agent;
	if(!agent)
		return;
	
	QRect rect = m_rect;
	
	p->setPen(Qt::black);
	p->fillRect(rect, QColor(0,0,0,150));
	p->drawRect(rect);
	
	//QString stateName = agent->state().name;
	QString stateName = agent->currentAction() ? agent->currentAction()->content() : "(?)";
	 
	AgentSubsystem *bioPtr = m_agent->subsystem(AgentBioSystem::className());
	AgentBioSystem *bio = dynamic_cast<AgentBioSystem*>(bioPtr);
	if(!bio)
	{
		qDebug() << "SimpleBotAgent::InfoDisplay::: Agent doesn't have a bio system, are we dead??";
		return;
	}
	
	AgentSubsystem *movePtr = m_agent->subsystem(AgentMovementSystem::className());
	AgentMovementSystem *move = dynamic_cast<AgentMovementSystem*>(movePtr);
	if(!move)
	{
		qDebug() << "SimpleBotAgent::InfoDisplay::: Agent doesn't have a movement system, are we dead??";
		return;
	}
	
	
	int fontSize = 10;
	int margin = fontSize/2;
	int y = margin;
	
	p->save();
	p->setPen(Qt::white);
	p->setFont(QFont("Monospace", fontSize, QFont::Bold));
	if(m_agent->m_currentGoal)
		p->drawText(rect.topLeft() + QPoint(margin, y += fontSize), QString( "Goal:   %1"  ).arg(m_agent->m_currentGoal->content()));
	p->drawText(rect.topLeft() + QPoint(margin, y += fontSize), QString( "State:  %1"  ).arg(stateName));
	p->drawText(rect.topLeft() + QPoint(margin, y += fontSize), QString( "Hunger: %1%" ).arg(((int)(bio->hunger() * 100.))));
	p->drawText(rect.topLeft() + QPoint(margin, y += fontSize), QString( "Energy: %1%" ).arg(((int)(bio->energy() * 100.))));
	p->drawText(rect.topLeft() + QPoint(margin, y += fontSize), QString( "Speed:  %1%" ).arg(((int)(move->speed() * 100.))));
	p->drawText(rect.topLeft() + QPoint(margin, y += fontSize), QString( "Angle:  %1"  ).arg(move->angle()));
	p->drawText(rect.topLeft() + QPoint(margin, y += fontSize), QString( "Rest/Eat: %1/%2" ).arg(move->restTime()).arg(bio->eatTime()));
	p->restore();
}

SimpleBotAgent::InfoDisplay *SimpleBotAgent::infoItem()
{
	if(!m_hud)
		m_hud = new SimpleBotAgent::InfoDisplay(this);
	
	return m_hud;
}

SimpleBotAgent::SimpleBotAgent(MSpace *ms)
	: QObject()
	, QGraphicsItem()
	, m_env(0)
	, m_mspace(ms)
	, m_label("")
	, m_color(Qt::black)
	, m_hud(0)
	, m_currentGoal(0)
	, m_currentAction(0)
	, m_node(0)
	, m_currentGoalMemory(0)
{
	if(!m_mspace)
		m_mspace = new MSpace();
		
	m_hunger = 1.0;
	m_energy = 1.0;

	// QGraphicsItem setup
	setFlag(ItemIsMovable);
	setFlag(ItemSendsGeometryChanges);
	setCacheMode(DeviceCoordinateCache);
	setZValue(99);
	
	// Timer setup
	connect(&m_advanceTimer, SIGNAL(timeout()), this, SLOT(advance()));
	m_advanceTimer.setInterval( 1000 / 60 );
	
	m_timer.start();
	m_state = StateUnknown;
	
	m_node = ms->node("Agent", MNodeType::SpecificEntityNode());
	
	initSubsystems();
	
	initGoals();
	
	chooseCurrentGoal();
	
}

void SimpleBotAgent::setEnv(SimpleBotEnv *env)
{
	m_env = env;
	if(env)
		env->addItem(this);
}
/*
void SimpleBotAgent::setMindSpace(MSpace *ms)
{
	m_mspace = ms;
}*/

void SimpleBotAgent::initSubsystems()
{
	addSubsystem(new AgentBioSystem(this));
	addSubsystem(new AgentMovementSystem(this));
	//addSubsystem(new AgentTouchSystem(this));
}

void SimpleBotAgent::addSubsystem(AgentSubsystem *sys)
{
	sys->initMindSpace();
	m_subsystems.append(sys);
	m_subsysHash[sys->className()]  = sys;
}

void SimpleBotAgent::initGoals()
{
	AgentSubsystem *bioPtr = subsystem(AgentBioSystem::className());
	
	
	MNode *agentGoals = m_node->linkedNode("AgentGoals", MNodeType::GoalNode());
	{
		MNode *hungerGoal = agentGoals->firstLinkedNode("HungerGoal");
		if(!hungerGoal)
		{
			hungerGoal = agentGoals->linkedNode("HungerGoal", MNodeType::GoalNode(), MLinkType::UnorderedLink());
			
			// NOTE Idea: Perhaps the link weight (Tv) could be the way to rank the goals...
			// NOTE Revision: The lti/sti would be a better ranking...
			
			QVariantList goalData;
			{	
				QVariantList goalRow;
				goalRow << "MIN" << "HungerValue" << 0.0;
				
				goalData.append(QVariant(goalRow));
			}
			
			hungerGoal->setData(goalData);
			
			//qDebug() << "SimpleBotAgent::initGoals: Debug: HUNGER goal goalData: "<<goalData;
			
			// Link this goal with it's associated variable node in the Bio system
			MNode *hungerVar = bioPtr->node()->firstLinkedNode("HungerValue");
			if(hungerVar)
			{
				m_mspace->link(hungerGoal, hungerVar, MLinkType::GoalVariableLink());
			}
			else
			{
				qDebug() << "SimpleBotAgent::initGoals: Bio system didn't have a linked 'HungerValue' - we're probably going to mess up and crash later.";
			}
		}
		m_goals << hungerGoal;
		
		MNode *energyGoal = agentGoals->firstLinkedNode("EnergyGoal");
		if(!energyGoal)
		{
			energyGoal = agentGoals->linkedNode("EnergyGoal", MNodeType::GoalNode(), MLinkType::UnorderedLink());
			
			QVariantList goalData;
			{	
				QVariantList goalRow;
				goalRow << "MAX" << "EnergyValue" << 1.0;
				
				goalData.append(QVariant(goalRow));
			}
			
			energyGoal->setData(goalData);
			
			// Link this goal with it's associated variable node in the Bio system
			MNode *energyVar = bioPtr->node()->firstLinkedNode("EnergyValue");
			if(energyVar)
			{
				m_mspace->link(energyGoal, energyVar, MLinkType::GoalVariableLink());
			}
			else
			{
				qDebug() << "SimpleBotAgent::initGoals: Bio system didn't have a linked 'EnergyValue' - we're probably going to mess up and crash later.";
			}
		}
		m_goals << energyGoal;
	}
}

static bool SimpleBotAgent_goalSort(MNode *n1, MNode *n2)
{
	if(!n1 || !n2)
		return false;
		
	double delta = n1->shortTermImportance() - n2->shortTermImportance();
	double rv = ((double)(rand() % 10) - 5.) / 100.; // Add a random +/- 5% to the value to prevent deadlocking
	delta += rv;
	
	return delta > 0;
}


// QList<MLink*> SimpleBotAgent::siblingGoals(MNode *goal)
// {
// 	if(!goal)
// 	{
// 		MNode *agentGoals = m_node->linkedNode("AgentGoals", MNodeType::GoalNode());
// 		return agentGoals->linkedNodes(MNodeType::GoalNode()); // only returns first level
// 	}
// 	else
// 	{
// 		QList<MLink*> incoming = goal->incomingLinks();
// 		
// 		QList<MNode*> node;	
// 	}
// }

MNode *SimpleBotAgent::evaulateSiblingGoalLinks(MNode *currentNode)
{
	QList<MLink*> incoming = currentNode->incomingLinks();
	
	// Only consider first incoming link
	MLink *firstLink = incoming.first();
	MNode *parentNode = firstLink->node1(); // node1 is the source node
		
	// Use outgoingLinks() instead of linkedNode(<goal node type>) becase we need to consider if the links are OrderedLinks
	QList<MLink*> siblingLinks = parentNode->outgoingLinks();
	
	// Only consider first sibling link to check for OrderedLinks
	firstLink = siblingLinks.first();
	
	// Grab a list of goal nodes so we dont have to filter it ourselves
	QList<MNode*> curLevelGoals = parentNode->linkedNode(MNodeType::GoalNode());
	
	// pointer to return
	MNode *goal = 0;
	
	// If links are ordered, find where the current goal resides in the list of link and take the next link as current
	// If current goal is last in list, assend to parent and re-evaulate parent in the smae manner (STI or OrderedLink)
	if(firstLink->type() == MLinkType::OrderedLink())
	{
		int idx = curLevelGoals.indexOf(currentNode);
		if(idx == curLevelGoals.size() - 1) 
		{
			// current node is at the end, assend to parent
			goal = evaulateSiblingGoalLinks(parentNode);
			qDebug() << "SimpleBotAgent::evaulateSiblingGoalLinks: currentNode:"<<currentNode->content()<<": Chose goal based on parentNode:"<<parentNode->content()<<", new goal: "<<goal->content();
		}
		else
		{
			goal = curLevelGoals.at(idx + 1);
			qDebug() << "SimpleBotAgent::evaulateSiblingGoalLinks: currentNode:"<<currentNode->content()<<": Chose goal based on OrderedLink idx:"<<idx<<", new goal: "<<goal->content();
		}
	}
	else
	{
		qSort(curLevelGoals.begin(), curLevelGoals.end(), SimpleBotAgent_goalSort);
		goal = curLevelGoals.first();
		
		qDebug() << "SimpleBotAgent::evaulateSiblingGoalLinks: currentNode:"<<currentNode->content()<<": Chose goal based on STI: "<<goal->content();
	}
	
	return goal;
}

void SimpleBotAgent::chooseCurrentGoal()
{
// 	if(m_currentGoal && !m_goalStack.isEmpty())
// 		m_goalStack.takeLast(); // current goal
// 	
	MNode *goal;
	
	MNode *agentGoals  = m_node->linkedNode("AgentGoals", MNodeType::GoalNode());
	MNode *goalPtrNode = agentGoals->linkedNode("GoalStack", MNodeType::MemoryNode());
	
	QList<MLink*> links = goalPtrNode->outgoingLinks();
	
	if(!links.isEmpty())
	{
		MLink *curGoalLink = links.takeLast();
		goalPtrNode->removeLink(curGoalLink);
		
		m_mspace->removeLink(curGoalLink);
		delete curGoalLink;
	}
	
	if(!links.isEmpty())
	{
		MLink *prevGoalLink = links.takeLast();
		goal = prevGoalLink->node2();
		
		goalPtrNode->removeLink(prevGoalLink);
		qDebug() << "SimpleBotAgent::chooseCurrentGoal: Popped goal off stack: "<<goal->content();
		
		m_mspace->removeLink(prevGoalLink);
		delete prevGoalLink;
	}
	else
	{
		//QList<Node*> curLevelGoals; // = siblingGoals(m_currentGoal);
		if(!m_currentGoal)
		{
			MNode *agentGoals = m_node->linkedNode("AgentGoals", MNodeType::GoalNode());
			QList<MNode*> curLevelGoals = agentGoals->linkedNode(MNodeType::GoalNode()); // only returns first level
			
			// Assume top-level goals are Unordered, so sort by STI instead of relying on link order 
			qSort(curLevelGoals.begin(), curLevelGoals.end(), SimpleBotAgent_goalSort);
			goal = m_goals.first();
		}
		else
		{
			goal = evaulateSiblingGoalLinks(m_currentGoal);
		}
	}
	
	//qDebug() << "SimpleBotAgent::chooseCurrentGoal: first goal after sort: "<<goal->content();
	
	if(goal != m_currentGoal)
	{
		setCurrentGoal(goal);
		
		chooseAction();
	}
}

double SimpleBotAgent::calcGoalActionProb(MNode *goal, MNode *action)
{
	MNode *goalActNode = 0;
	
	// Find/create agent's memory node
	MNode *memory = m_node->linkedNode("AgentMemory", MNodeType::MemoryNode());
	{
		// Find/create memory node for this goal
		QString goalMemoryKey = goal->content();
		MNode *goalMemory = memory->linkedNode(goalMemoryKey, MNodeType::GoalMemoryNode());
		{
			// Get the action node inside the goal
			QString actMemoryKey = action->content();
			goalActNode = goalMemory->linkedNode(actMemoryKey, MNodeType::ActionTryNode());
		}
	}
	
	
	//return ((double)(rand() % 100)) / 100.;
	//double lti = action->longTermImportance();
	double lti = goalActNode->longTermImportance();
	
	double rv = ((double)(rand() % 10) - 5.) / 100.; // Add a random +/- 5% to the value to prevent deadlocking
	lti += rv;
	
	return lti;
}

void SimpleBotAgent::chooseAction()
{
	bool debug = false;
	
	// debug must be true for any of these to output anything:
	bool debugActionProg = true;
	bool debugLtiChange  = true;
	bool debugVarValues  = true;
	bool debugBasic      = true;
	
	AgentSubsystem::ActionInfo maxInfo;
	double maxProb = -65536.0;
	static bool isSetup = false;
	foreach(AgentSubsystem *subsys, m_subsystems)
	{
		QList<AgentSubsystem::ActionInfo> actions = subsys->actions();
		foreach(AgentSubsystem::ActionInfo info, actions)
		{
			if(!isSetup)
			{
				//info.node->setLongTermImportance(1.0);
			}
			
			double p = calcGoalActionProb(m_currentGoal, info.node);
			if(debug && debugActionProg)
				qDebug() << "SimpleBotAgent::chooseAction: p:"<<p<<", info.node:" << info.node;
			if(p >= maxProb)
			{
				maxProb = p;
				maxInfo = info;
			}
		}
	}
	
	isSetup = true;

	AgentSubsystem *bioPtr = subsystem(AgentBioSystem::className());
	AgentBioSystem *bio = dynamic_cast<AgentBioSystem*>(bioPtr);

	// clone clones first level links and nodes by default
	//m_currentAction = maxInfo.node->clone();
	
	// If we just finished an action (m_currentAction not null), update the LTI of the action based on the progress made toward the goal
	if(m_currentAction)
	{
		/// Create/locate a memory snapshot of goal/action/variable combo
		
		// Store goalTryNode inside the block below for updating the LTI of it as we proceede
		//MNode *goalTryNode = 0;
		MNode *goalActNode = 0;
		
		// Find/create agent's memory node
		MNode *memory = m_node->linkedNode("AgentMemory", MNodeType::MemoryNode());
		{
			// Find/create memory node for this goal
			QString goalMemoryKey = m_currentGoal->content();
			MNode *goalMemory = memory->linkedNode(goalMemoryKey, MNodeType::GoalMemoryNode());
			{
				// This is just theory - but store the uuid in case we need to definitively associate this GoalMemoryNode
				// with the specific GoalNode - because the goalMemoryKey (based on content()) might conceivably have the
				// same key (and, therefore, clash) as another goal, but the UUIDs would (should) never clash 
				if(!goalMemory->data().isValid())
					goalMemory->setData(m_currentGoal->uuid());
				
				// chooseCurrentGoal() resets m_currentGoalMemory to NULL when the goal changges,
				// so start out the m_currentGoalMemory ptr at a GoalTryNode using a node name
				// constructed sequentially based on the number of times we've attempted this goal
				/*
				if(!m_currentGoalMemory)
				{
					// Store the "try counter" on the goal itself so it automatically gets preserved across program runs
					int curGoalMemoryCounter = m_currentGoal->property("_goal_memory_counter").toInt();
					if(curGoalMemoryCounter < 0)
						curGoalMemoryCounter = 0;
					curGoalMemoryCounter ++;
					m_currentGoal->setProperty("_goal_memory_counter", curGoalMemoryCounter); 
					
					// Create the node using a sequential name based on the number of times we've attempted this goal
					QString memKey = tr("GoalTry%1").arg(curGoalMemoryCounter);
					m_currentGoalMemory = goalMemory->linkedNode(memKey, MNodeType::GoalTryNode());
					
					// Store the time we started this "try" on the memory node for later determining how long it took us to "finish" this goal
					m_currentGoalMemory->setData(QTime::currentTime());
					
					goalTryNode = m_currentGoalMemory;
				}
				else
				{
					int curGoalMemoryCounter = m_currentGoal->property("_goal_memory_counter").toInt();
					QString memKey = tr("GoalTry%1").arg(curGoalMemoryCounter);
					
					goalTryNode = goalMemory->linkedNode(memKey, MNodeType::GoalTryNode());
				}
				*/
					
				// Create memory node for this action
				QString actMemoryKey = m_currentAction->content();
				/*
				MNode *actMemory = m_mspace->addNode(actMemoryKey, MNodeType::ActionMemoryNode());
				actMemory->setData(m_currentAction->uuid());
				
				// Clone the variables associated with the current action - e.g. the action just "completed"
				// and store them "on" the current ActionMemoryNode
				QList<MNode*> vars = m_currentAction->linkedNode(MNodeType::VariableNode());
				foreach(MNode *node, vars)
				{
					MNode *nodeClone = node->clone(0); // 0 = dont clone any links to this node
					nodeClone->setType(MNodeType::VariableSnapshotNode());
					m_mspace->addNode(nodeClone);
					m_mspace->addLink(actMemory, nodeClone, MLinkType::MemoryLink());
				}
				
				// Link the current ActionMemoryNode to the m_currentGoalMemory - then set the 
				// m_currentGoalMemory to the current ActionMemoryNode, essentially creating a simple 'linked list'
				m_mspace->addLink(m_currentGoalMemory, actMemory, MLinkType::NextItemLink());
				m_currentGoalMemory = actMemory;
				*/
				// Note that we'll also use the m_currentGoalMemory ptr to set the LTI of this action memory item based on the contribution toward the persuit of the goal, below.
					
				// Also create a link directly from goalMemory to a node for the current action (not inside the goalTry loop, for LTI adjustment)
				goalActNode = goalMemory->linkedNode(actMemoryKey, MNodeType::ActionTryNode());
				if(!goalActNode->data().isValid())
					goalActNode->setData(m_currentAction->uuid());
			}
		}
		
		
		// eval goal value change and reward STI/LTI of action accordingly
		
		
		
		// Loop thru the evaulation information contained in the goal's data() function, find the variables
		// and compare them to the values the variables had at the start of the action (stored in another block below in m_goalVarSnapshot)
		// and adjust the longTermImportance of the m_currentAction based on whether the change in the goal variable data was 'good' (matched the goal) or 'bad' (against the goal) 
		double goalDeltaSum = 0.0;
		
		QVariantList goalData = m_currentGoal->data().toList();
		int rowCount = 0;
		foreach(QVariant rowData, goalData)
		{	
			rowCount ++ ;
			
			QVariantList goalRow = rowData.toList();
			if(goalRow.size() < 2)
			{
				qDebug() << "SimpleBotAgent::chooseAction: Error evaulating current goal "<<m_currentGoal->content()<<": row "<<rowCount<<" has less than 2 data elements: "<<goalRow;
				qDebug() << "Exiting app.";
				exit(-1);
			}
			
			QString evalFunc = goalRow[0].toString();
			QString varId    = goalRow[1].toString(); // can be name or UUID
			QVariant goalVal = goalRow.size() >= 3 ? goalRow[2] : QVariant();
			
			// Find the node referenced as the variable
			MNode *goalVar = m_currentGoal->linkedNodeUuid(varId); // check by uuid first
			if(!goalVar)
				goalVar = m_currentGoal->firstLinkedNode(varId); // check by content if uuid not matched
			
			if(!goalVar)
			{
				qDebug() << "SimpleBotAgent::chooseAction: Error evaulating current goal "<<m_currentGoal->content()<<": Unable to find linked var:" << varId;
				qDebug() << "Exiting app.";
				exit(-1);
			}
			
			// Grab the data value from the when we chose this action and where it is now
			QVariant lastValue = m_goalVarSnapshot[goalVar];
			QVariant thisValue = goalVar->data();
			QVariant::Type dataType = thisValue.type();
			
			// Figure out the evaulation function
			double expectedChangeSign = evalFunc.toUpper() == "MIN" ? -1 : 
						    evalFunc.toUpper() == "MAX" ? +1 :
						    0;
				
			if(expectedChangeSign == 0)
			{
				qDebug() << "SimpleBotAgent::chooseAction: Error evaulating current goal "<<m_currentGoal->content()<<": Linked var:" << varId<<": unknown eval function: "<<evalFunc;
				qDebug() << "Exiting app.";
				exit(-1); 
			}
			
			// Eval the change in the variable
			if(dataType == QVariant::Double ||
			   dataType == QVariant::Int    ||
			   dataType == QVariant::Bool   ||
			   dataType == QVariant::Int    ||
			   dataType == QVariant::LongLong)
			{
				double valueDelta = thisValue.toDouble() - lastValue.toDouble();
				//double changeSign = valueDelta < 0 ? -1 : +1;
				
				// Make sure ZERO change is marked as BAD
				if(valueDelta == 0.0)
					valueDelta = -0.07 * expectedChangeSign;
					
				/// TODO 0.1 = magic number, should it be configurable ?
				double propLtiChange = 0.1 * valueDelta * expectedChangeSign; // Basically, for things that should be minimized 
											      // (MIN eval func, -1 change sign), invert the Lti 
											      // change because the "good" delta will be going negative
				
				// Adjust the LTI of the system's action node
				m_currentAction->setLongTermImportance( m_currentAction->longTermImportance() + propLtiChange );
				
				// Also adjust the LTI of this goal->action combo
				goalActNode->setLongTermImportance( goalActNode->longTermImportance() + propLtiChange );
				MLink *link = goalActNode->links().at(0);
				if(link)
				{
					double tv = goalActNode->longTermImportance();
					if(tv < 0)
						tv = 0.001;
					//if(tv > 10.0)
					//	tv = tv/10.;
					tv *= 1.5;
					link->setTruthValue(MTruthValue( tv ));
				}
				
				if(debug && debugLtiChange)
					qDebug() << "SimpleBotAgent::chooseAction: " << m_currentGoal->content()<</*", new STI: "<< m_currentAction->longTermImportance()<<*/", new goalActNode STI: "<<goalActNode->longTermImportance()<<", action was: "<<m_currentAction->content()<<" (prop lti change: "<<propLtiChange<<", thisValue:" <<thisValue.toDouble()<<", lastValue:"<<lastValue.toDouble()<<", change:"<<valueDelta<<")";
				
				// Use the goalVal to compare the change so we can have something to use to update the STI of the goal
				if(goalVal.isValid())
				{
					// convert to abs 0-1 range (see note below on view of goalDeltaSum)
					double goalNum = goalVal.toDouble();
					double goalDelta = fabs(goalNum - thisValue.toDouble()) / (goalNum == 0.0 ? 1.0 : goalNum);
					goalDeltaSum += goalDelta;
				}
				
			}
			else
			{
				qDebug() << "SimpleBotAgent::chooseAction: Error evaulating current goal "<<m_currentGoal->content()<<": Linked var:" << varId<<": don't know how to process dataType: "<<dataType;
				qDebug() << "Exiting app.";
				exit(-1);
			}
		}
		
		// If we view the goalDeltaSum as the distance from the goal, then we can convert the goalDeltaSum to an abs 0-1 value and store as the sti of the goal
		//double propAbsSti = qAbs(goalDeltaSum) / goalVal.toDouble(); // convert to abs 0-1 value
		double avgGoalDelta = goalDeltaSum / ((double)rowCount);
		m_currentGoal->setShortTermImportance(avgGoalDelta);
		if(debug && debugLtiChange)
			qDebug() << "SimpleBotAgent::chooseAction: " << m_currentGoal->content()<<": goalDeltaSum: "<<goalDeltaSum<<", avgGoalDelta: "<<avgGoalDelta;
		
		
		// TODO: We stored the goalTryNode and we have the m_currentGoalMemory as the memory node of the last completed action -
		// we need to update the goalTryNode LTI somehow - really, we need a variety of criteria for assessing the LTI of the "try", such as:
		// - Time it took
		// - "effort" .. ?
		// - other variables...?
	}


	// Store a snapshot of the goal variable's data at the start of this action assignment
	QVariantList goalData = m_currentGoal->data().toList();
	//qDebug() << "SimpleBotAgent::chooseAction: Debug: Starting to store data for goal "<<m_currentGoal->content()<<", goalData: "<<goalData;
	
	int rowCount = 0;
	foreach(QVariant rowData, goalData)
	{	
		rowCount ++;
		
		QVariantList goalRow = rowData.toList();
		if(goalRow.size() < 2)
		{
			qDebug() << "SimpleBotAgent::chooseAction: Error storing data for goal "<<m_currentGoal->content()<<": row "<<rowCount<<" has less than 2 data elements: "<<goalRow;
			qDebug() << "Exiting app.";
			exit(-1);
		}
		
		QString evalFunc = goalRow[0].toString();
		QString varId    = goalRow[1].toString(); // can be name or UUID
		
		MNode *goalVar = m_currentGoal->linkedNodeUuid(varId); // check by uuid first
		if(!goalVar)
			goalVar = m_currentGoal->firstLinkedNode(varId); // check by content if uuid not matched
		
		if(!goalVar)
		{
			qDebug() << "SimpleBotAgent::chooseAction: Error storing data for goal "<<m_currentGoal->content()<<": Unable to find linked var:" << varId;
			qDebug() << "Exiting app.";
			exit(-1);
		}
		
		m_goalVarSnapshot[goalVar] = goalVar->data();
	}
	
	
	// Why clone... ?
	m_currentAction = maxInfo.node;//->clone();
	
	if(debug && debugBasic)
		qDebug() << "SimpleBotAgent::chooseAction: Current Goal: "<<m_currentGoal->content()<<", Chose new action: "<<m_currentAction;

	//qDebug() << "Action changed, cloned action node. Debug info: orig node:"<<maxInfo.node<<", orig node type: "<<maxInfo.node->type()<<", cloned type:" <<m_currentAction->type();
//	const QList<MLink*> & links = m_currentAction->links();
// 	qDebug() << "Debug: links on current action: ";
// 	foreach(MLink *link, links)
// 		qDebug() << "\t" << link << "\n\n";
	
	QList<MNode*> vars = m_currentAction->linkedNode(MNodeType::VariableNode());
	
	/// TODO DON'T CHEAT
	// Need to come up with a way for the agent to "choose" its own rest/eat times and speed/direction based on its "personality" and "learned behaviours"
	/// FIXME CHEATING
	
	
	foreach(MNode *node, vars)
	{
		if(node->content() == "EatTime" ||
		   node->content() == "RestTime")
		{
			node->setData( rand() % 500 );
		}
		else
		if(node->content() == "MoveSpeed")
		{
			node->setData( ((double)(rand() % 100)) / 100.); 
		}
		else
		if(node->content() == "MoveDirection")
		{
			node->setData( rand() % 359 );
		}
		if(debug && debugVarValues)
			qDebug() << " \t Variable: "<<node->content()<<", Data: "<<node->data();
	}
	
	foreach(AgentSubsystem *subsys, m_subsystems)
	{
		if(!subsys->executeAction(m_currentAction))
		{
			//qDebug() << "SimpleBotAgent::chooseAction: "<<subsys<<" - subsys didnt want action "<<m_currentAction->content(); 
		}
		else
		{
			if(debug)
				qDebug() << "SimpleBotAgent::chooseAction: "<<subsys<<" subsystem accepted "<<m_currentAction->content();
		}
	}

// 	if((m_currentAction->content() != "MoveAction" && vars.size() > 1) ||
// 	   (m_currentAction->content() == "MoveAction" && vars.size() > 3))
// 	{
// 		qDebug() << "Your up creek without a paddle, buddy.";
// 		exit(-1);
// 	}
	
	//qDebug("\n\n");

	update();
	
	m_exceptionVar = 0;
}

void SimpleBotAgent::actionCompleted(MNode *currentAction)
{
	//qDebug() << "SimpleBotAgent::actionCompleted: "<<currentAction;
	chooseAction();
}

//MNode *m_currentGoal;

void SimpleBotAgent::actionException(MNode *currentAction, MNode *exceptionVar, QVariant targetVal,  const QString& message)
{
	//m_exceptionVar = exceptionVar;
	
	//qDebug() << "SimpleBotAgent::actionException: Unandled: "<<message<<", current action: "<<currentAction;
	//exit(-1);
	qDebug() << "SimpleBotAgent::actionException: "<<message<<", current action: "<<currentAction<<", exceptionVar: "<<exceptionVar<<", targetVal: "<<targetVal;
	
	// Has exception var, create faux goal with MAX/MIN based on delta (targetVal-exceptionVar.data())
	/*
	if(exceptionVar)
	{
		double targetNum = targetVal.toDouble();
		double currentNum = exceptionVar->data().toDouble();
		double delta = targetNum - currentNum;
		QString func = delta >= 0 ? "MAX" : "MIN";
		QString varName = exceptionVar->content();
		
		qDebug() << "\t delta:"<<delta<<", func:"<<func<<", varName:"<<varName;
		
		MNode *fauxGoal = 0;
		foreach(MNode *goal, m_goals)
		{
			// TODO This block below attempts to match the goal data to the exception var/target val
			// However, it's possible that goals may have more than one variable - in which case,
			// this would match the last goal that it findds that matches - regardless of the other
			// variables. That may not be the right action...
			QVariantList goalData = goal->data().toList();
			int rowCount = 0;
			foreach(QVariant rowData, goalData)
			{	
				rowCount ++ ;
				
				QVariantList goalRow = rowData.toList();
				if(goalRow.size() < 2)
				{
					qDebug() << "SimpleBotAgent::actionException: Error searching goal "<<goal->content()<<": row "<<rowCount<<" has less than 2 data elements: "<<goalRow;
					continue;
				}
				
				QString evalFunc = goalRow[0].toString();
				QString varId    = goalRow[1].toString(); // can be name or UUID
				QVariant goalVal = goalRow.size() >= 3 ? goalRow[2] : QVariant();
				
				// Find the node referenced as the variable
				MNode *goalVar = goal->linkedNodeUuid(varId); // check by uuid first
				if(!goalVar)
					goalVar = goal->firstLinkedNode(varId); // check by content if uuid not matched
				
				if(!goalVar)
				{
					qDebug() << "SimpleBotAgent::actionException: Error searching goal "<<goal->content()<<": Unable to find linked var:" << varId;
					continue;
				}
				
				if(goalVar->uuid() == exceptionVar->uuid() &&
				   goalVal.toDouble() == targetNum)
				{
					fauxGoal = goal;
					qDebug() << "\t g: "<<goal->content()<<" matches, goalVar: "<<varId<<goalVal.toDouble()<<", ex data:"<<exceptionVar->content()<<targetNum<<"("<<goalVar->uuid()<<"="<<exceptionVar->uuid()<<")";
					break;
				}
				else
				{
					qDebug() << "\t g: !"<<goal->content()<<" goalVar:"<<goalVar->content()<<"!="<<exceptionVar->content()<<", or goalVal:"<<goalVal.toDouble()<<"!="<<targetNum;
				}
			}
		}
		
		// If we didnt find a goal that matched the exception/var,
		// attempt to create it (or find a goal node linked to AgentGoals with the same name)
		if(!fauxGoal)
		{
			MNode *agentGoals = m_node->linkedNode("AgentGoals", MNodeType::GoalNode());
			
			fauxGoal = agentGoals->linkedNode(tr("%1(%2)").arg(func).arg(varName), MNodeType::GoalNode(), MLinkType::PartOf());
				
			QVariantList goalData;
			{	
				QVariantList goalRow;
				goalRow << func << varName << targetVal;
				
				goalData.append(QVariant(goalRow));
			}
			
			fauxGoal->setData(goalData);
			
			if(!m_goals.contains(fauxGoal))
				m_goals << fauxGoal;
			
			m_mspace->link(fauxGoal, exceptionVar, MLinkType::GoalVariableLink());
			qDebug() << "SimpleBotAgent::actionException: Created faux goal: "<<fauxGoal->content()<<" for exception data: var:"<<exceptionVar->content()<<", varName:"<<varName<<", target:"<<targetVal.toDouble();
		}
		
		// This routine here attempts to prevent unnecessary groth of the goal stack
		// by intelligently removing/appending the current goal to the stack.
		if(!m_goalStack.isEmpty())
		{	
			//qDebug() << "\t lastGoal: "<<lastGoal<<", currentGoal:"<<m_currentGoal;
			MNode *lastGoal = m_goalStack.last();
			
			if(lastGoal == fauxGoal)
			{
				// take off last goal, put on current goal
				m_goalStack.takeLast();
				m_goalStack.append(m_currentGoal);
			}
			else
			if(m_currentGoal != lastGoal &&
			   m_currentGoal != fauxGoal)
			{
				m_goalStack.append(m_currentGoal);
			}
			
			
			// Print out some debugging info so we can ensure its functioning correctly
			int row = 0;
			foreach(MNode *goal, m_goalStack)
			{
				qDebug() << "\t goalStack "<<row<<": "<<goal->content(); 
				row++;
			}
			
			qDebug() << "\t lastGoal: "<<lastGoal<<", currentGoal:"<<m_currentGoal<<", fauxGoal:"<<fauxGoal;
			
			//qDebug() << "\t ------";
		}
		else
		{
			// Start off the stack with the current goal
			m_goalStack.append(m_currentGoal);
		}
		
		
		// Output more debug info
		qDebug() << "SimpleBotAgent::actionException: Current goal changed, new goal: "<<fauxGoal->content()<<", was:"<<m_currentGoal->content()<<", m_goalStack.size:"<<m_goalStack.size();
		
		setCurrentGoal(fauxGoal);
	}
	*/
	chooseAction();
}

void SimpleBotAgent::setCurrentGoal(MNode *newGoal)
{
	/*
	if(m_currentGoalMemory && 
	   m_currentGoal)
	{
		// Find/create agent's memory node
		MNode *memory = m_node->linkedNode("AgentMemory", MNodeType::MemoryNode());
		{
			// Find/create memory node for the new goal
			QString goalMemoryKey = goal->content();
			MNode *goalMemory = memory->linkedNode(goalMemoryKey, MNodeType::GoalMemoryNode());
			{
				// Link last item in the linked list of actions to the next goal
				m_mspace->addLink( m_currentGoalMemory, goalMemory, MLinkType::NextItemLink() );
			}
			
			
			// Find the memory node for the old (current) goal
			QString curGoalMemoryKey = m_currentGoal->content();
			MNode *curGoalMemory = memory->linkedNode(curGoalMemoryKey, MNodeType::GoalMemoryNode());
			{
				int curGoalMemoryCounter = m_currentGoal->property("_goal_memory_counter").toInt();
				QString memKey = tr("GoalTry%1").arg(curGoalMemoryCounter);
				
				MNode *goalTryNode = curGoalMemory->linkedNode(memKey, MNodeType::GoalTryNode());
				
				// Update the LTI of the "GoalTry" node based on the time it took to complete this "try"s
				QTime startTime = goalTryNode->data().toTime();
				int delta = startTime.msecsTo(QTime::currentTime());
				goalTryNode->setLongTermImportance(delta);
			
				// TODO: Find better quantifiers of the "importance" of this "try" other than the length of time it took to finish the goal
			}
		}
	}
	*/
	
	qDebug() << "SimpleBotAgent::setCurrentGoal: newGoal: "<<newGoal->content();
		
	MNode *agentGoals  = m_node->linkedNode("AgentGoals", MNodeType::GoalNode());
	MNode *goalPtrNode = agentGoals->linkedNode("GoalStack", MNodeType::MemoryNode());
	
	QList<MLink*> links = goalPtrNode->outgoingLinks();
	
	MNode *prevGoal    = links.size() >= 2 ? links.at(links.size() - 2)->node2() : 0; // node2 is the destination node
	MNode *currentGoal = links.size() >= 1 ? links.at(links.size() - 1)->node2() : 0; // node2 is the destination node
	
	
	if(prevGoal && newGoal && 
	   prevGoal == newGoal)
	{
		MLink *prevGoalLink = links.at(links.size()-2);
		MLink *curGoalLink  = links.at(links.size()-1);
		
		// take it out of 'second to last'
		goalPtrNode->removeLink(prevGoalLink);
		// append it to the end
		goalPtrNode->addLink(prevGoalLink);
		//delete links.last(); // we assume ownership of the link object once removed
		
		// Create the link
		//MLink *link = goalPtrNode->addLink(goal, MLinkType::OrderedLink());
		
		// Hackish method of displaying the 'slot number' this link occupies on the graph (since the graph renders truth values
		prevGoalLink->setTruthValue(MTruthValue( links.size() ));
		curGoalLink ->setTruthValue(MTruthValue( links.size()-1 ));
	}
	else
	if(currentGoal != newGoal)
	{
		// Create the link
		MLink *link = goalPtrNode->addLink(newGoal, MLinkType::OrderedLink());
		//qDebug() << "\t ** ADDING new link for goal: "<<newGoal<<", link: "<< link; 
		
		// Hackish method of displaying the 'slot number' this link occupies on the graph (since the graph renders truth values
		link->setTruthValue(MTruthValue( links.size() ));
	}
	
	// Print out some debugging info so we can ensure its functioning correctly
	QList<MLink*> newStack = goalPtrNode->outgoingLinks();
	int row = 0;
	foreach(MLink *goalLink, newStack)
	{
		MNode *goal = goalLink->node2();
		qDebug() << "\t goalStack "<<row<<": "<<goal->content(); 
		//qDebug() << "\t \t "<<goalLink;
		row++;
	}
	
	qDebug() << "\t prevGoal: "<<prevGoal<<", currentGoal:"<<currentGoal<<", newGoal:"<<newGoal;
	
	
	
	m_currentGoal = newGoal;
	m_currentGoalMemory = 0;
	
	//qDebug() << "SimpleBotAgent::setCurrentGoal: Current goal changed, new goal: "<<newGoal->content();
	
// 	if(newStack.size() >= 2)
// 		exit(-1);
	
	
}

QRectF SimpleBotAgent::boundingRect() const
{
	qreal adjust = 2;
	QRectF shapeRect = QRectF(-10 - adjust, -10 - adjust,
		                   20 + adjust,  20 + adjust);

	QFont font("", 5);
	QFontMetrics metrics(font);
	QRectF textRect = QRectF(metrics.boundingRect(m_label));
	textRect.moveTo(5,10);
	
	return shapeRect.united(textRect.adjusted(-adjust,-adjust,+adjust,+adjust));
	
}

QPainterPath SimpleBotAgent::shape() const
{
	QPainterPath path;
	path.addEllipse(-10, -10, 20, 20);
	return path;
}

void SimpleBotAgent::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget *)
{
	painter->setBrush(m_color);
	painter->drawEllipse(-10, -10, 20, 20);
	
	painter->setPen( QPen(Qt::red, 3.0 ) );
	
	AgentSubsystem *movePtr = subsystem(AgentMovementSystem::className());
	AgentMovementSystem *move = dynamic_cast<AgentMovementSystem*>(movePtr);
	if(move)
	{
		QPointF center(0,0);
		QLineF line(center, move->vec() + center);
		painter->drawLine( line );
	}
	
	if(!m_label.isEmpty())
	{
		QFont font("",5);
		painter->setFont(font);
		
		painter->setPen(Qt::white);
		painter->drawText(5,10,m_label);
		painter->setPen(Qt::green);
		painter->drawText(4,9,m_label);
	}
}

QVariant SimpleBotAgent::itemChange(GraphicsItemChange change, const QVariant &value)
{
	switch (change) 
	{
		case ItemPositionHasChanged:
// 			foreach (MindSpaceGraphEdge *edge, m_edgeList)
// 				edge->adjust();
// 			m_graph->itemMoved(this);
			break;
		default:
			break;
	};
	
	return QGraphicsItem::itemChange(change, value);
}

void SimpleBotAgent::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	update();
	QGraphicsItem::mousePressEvent(event);
}

void SimpleBotAgent::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	update();
	QGraphicsItem::mouseReleaseEvent(event);
}

void SimpleBotAgent::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	emit doubleClicked(this);
	QGraphicsItem::mouseDoubleClickEvent(event);
}

void SimpleBotAgent::start()
{
	m_advanceTimer.start();
}

void SimpleBotAgent::advance()
{
	// If this goal is no longer important, request a new goal
	if(m_currentGoal->shortTermImportance() <= 0.0)
	{
		qDebug() << "SimpleBotAgent::advance(): STI of current goal is <= 0.0, requesting new goal";
		chooseCurrentGoal();
	}
	
	
	
	// Set initial state
// 	if(m_state == StateUnknown)
// 		setState(StateSearching);

		
	/*
		Based on the new readme notes/thoughts on goals, etc, then we can express our SimpleBotAgent goals in terms of maximizing energy and minimizing hunger.
		Somehow, we have to first determine a format to express the goals, then we have to link them to our agent and rank them in some order.
		 
	
	*/
	
	//MNode *agentNode = m_mspace->node(
	//MNode *hungerGoal = m_mspace->node("HungerGoal", MNodeType::GoalNode());
	//MNode *energyGoal = m_mspace->node("EnergyGoal", MNodeType::GoalNode());
	
	
	/*
	
		Okay, now we've got the goals described and linked to the variables they relate to.
		
		Now, we've got to have some method of evaulating the goals and seing if they're met.
		If not, we need to have some way of meeting those goals (deciding what actions to take).
		Note that any actions we take, we need to evaulate the effect they have on the variable in question
		and then reward/demote the nodes accordingly, linking them back to the action itself, and ultimatly link the action to the goal for future reference.
		 
		The end result will be the goal may have multiple different actions linked to it, each rated differently (or even close to the same).
		So when the goal needs to be fulfilled again in the future, it can choose actions that gave it best results (maybe even store how quickly the actions affect the goal
		e.g. store the "time" it took)
		
		This conceptually implies that since we expressed the goals as functions of variables, if other goals that are "new" (e.g. dont have actions linked),
		the agent /could/ search for other goals that link to those same variables and find out what actions were taken....theoretically.
		
		Well, I'll have to stop here for now...will continue coding as soon as possible.
	*/
	
	/* Coding stopped here 2/3 */
	
	///
	/* More thoughts:
	
		Actions---
			ActionNode
				
	
	*/
	///
	
	
	foreach(AgentSubsystem *subsys, m_subsystems)
		subsys->advance();
	
// 	// Update bio variables
// 	updateHungerEnergyState();
// 	
// 	// m_hunger increases as m_energy decreases
// 	// m_energy increases when we eat or when we rest
// 	// m_energy decreases as time progresses in any state other than resting or eating
// 	
// 	//m_label = QString("%1/%2").arg(m_hunger).arg(m_energy);
// 	
// 	// Process current state
// 	if(m_state == StateResting)
// 	{
// 		processResting();
// 	}
// 	else
// 	if(m_state == StateSearching)
// 	{
// 		processSearching();
// 	}
// 	else
// 	if(m_state == StateEating)
// 	{
// 		processEating();
// 	}
// 	else
// 	if(m_state == StateAsking)
// 	{
// 		//
// 	}
// 	
// 	// Check to see if state change required
// 	evaulateStateChangeRequired();
		
	// Update HUD item if present
	if(m_hud)
		m_hud->update();
	
}

void SimpleBotAgent::evaulateStateChangeRequired()
{
	if(m_hunger < 0)
		m_hunger = 0;
	if(m_hunger > 1)
		m_hunger = 1;
	if(m_energy < 0)
		m_energy = 0;
	if(m_energy > 1)
		m_energy = 1;
		
	if(m_stateTimer.elapsed() / 1000  > rand() % 10)
			setState(StateSearching);
	else
	if(m_stateTimer.elapsed() / 1000 > rand() % 10) // || m_hunger > 0.9 || m_energy < 0.1)
			setState(rand() % 10 > 5 ? StateEating : StateResting);
			
// 	if(m_stateTimer.elapsed() / 1000  > 1.0)
// 			setState(StateSearching);
}

void SimpleBotAgent::updateHungerEnergyState()
{
	int elapsed = m_timer.restart();
	double rate = (((double)elapsed) / 100.) * 0.1;
	
	m_hunger += rate * 0.5;
	m_energy -= rate * 0.7;
	
	if(m_decayRate != rate)
		m_decayRate = rate;
}

void SimpleBotAgent::processResting()
{
	// What do we do?
	// When are we done?
	m_energy += m_decayRate;
	m_hunger -= m_decayRate * 0.1;
}

void SimpleBotAgent::processEating()
{
	m_hunger -= m_decayRate * 1.5;
}

void SimpleBotAgent::processSearching()
{
	if(m_vec.isNull())
		chooseVector();
		
	QRectF sceneRect = scene()->sceneRect();
	QPointF newPos = pos() + m_vec; //QPointF(xvel, yvel);
	QPointF expect = newPos;
	newPos.setX(qMin(qMax(newPos.x(), sceneRect.left() + 10), sceneRect.right()  - 10));
	newPos.setY(qMin(qMax(newPos.y(), sceneRect.top()  + 10), sceneRect.bottom() - 10));
	
	if(newPos != expect)
	{
		chooseVector();
		//double angle = chooseVector();
		//qDebug() << "SimpleBotAgent::advance: S_Searching: new m_vec: "<<m_vec<<", new angle: "<<angle;
	}
	//else
		//qDebug() << "SimpleBotAgent::advance: S_Searching: new pos: "<<newPos;
	
	setPos(newPos);
}

double SimpleBotAgent::chooseVector()
{
// 	m_vec = QPointF(
// 		((double)(rand() % 100)) / 10. - 5.,
// 		((double)(rand() % 100)) / 10. - 5.
// 	);

	double angle = ((double)(rand() % 359));
	double speed = ((double)(rand() %  19)) + 1;
	qDebug() << "SimpleBotAgent::chooseVector(): angle:"<<angle<<", speed:"<<speed;
	
	// X= R*cos(Theta)
	// Y= R*sin(Theta)
	double x = speed * cos(angle);
	double y = speed * sin(angle);
	
	m_vec = QPointF(x,y);
	
	QLineF line(QPointF(0,0), m_vec);
	//setRotation(line.angle());
	
	update();
	
	return line.angle();
}

void SimpleBotAgent::setState(StateInfo state)
{
// 	MNode *lastStateNode = 0;
// 	if(m_state != StateUnknown)
// 		lastStateNode = m_mspace->node(m_state.name);
// 	
// 	QString stateName = state.name;
// 	qDebug() << "SimpleBotAgent::setState: "<<stateName;
// 	m_state = state;
// 	
// 	m_stateTimer.restart();
// 	
// 	// Ensure node exists for this state
// 	MNode *currentStateNode = m_mspace->node(stateName, MNodeType::ProcedureNode());
// 		
// 	if(lastStateNode)
// 	{
// 		MLink *foundLink = m_mspace->link(lastStateNode, currentStateNode);
// 		
// 		if(foundLink)
// 			foundLink->setTruthValue(foundLink->truthValue().value() + 0.05);
// 		else
// 			m_mspace->addLink( new MLink(lastStateNode, currentStateNode, MLinkType::EventLink(), MTruthValue(0.1)) );
// 	}
}
