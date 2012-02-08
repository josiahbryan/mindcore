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
	m_rect  = QRect(10,10,160,70);
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
	p->fillRect(rect, QColor(0,0,0,187));
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
	m_advanceTimer.setInterval( 1000 / 10 );
	
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
	MNode *agentGoals = m_mspace->node("AgentGoals", MNodeType::GoalNode());
	m_mspace->link(m_node, agentGoals, MLinkType::PartOf());
	
	MNode *hungerGoal = m_mspace->node("HungerGoal");
	if(!hungerGoal)
	{
		hungerGoal = m_mspace->addNode("HungerGoal", MNodeType::GoalNode());
		
		// NOTE Idea: Perhaps the link weight (Tv) could be the way to rank the goals...
		// NOTE Revision: The lti/sti would be a better ranking...
		m_mspace->link(agentGoals, hungerGoal, MLinkType::OrderedLink());
		
		QVariantList goalData;
		{	
			QVariantList goalRow;
			goalRow << "MIN" << "HungerValue" << 0.0;
			
			goalData.append(QVariant(goalRow));
		}
		
		hungerGoal->setData(goalData);
		
		//qDebug() << "SimpleBotAgent::initGoals: Debug: HUNGER goal goalData: "<<goalData;
		
		MNode *hungerVar = m_mspace->node("HungerValue", MNodeType::VariableNode());
		hungerVar->setData(1.0);
		
		m_mspace->link(hungerGoal, hungerVar, MLinkType::GoalVariableLink());
	}
	m_goals << hungerGoal;
	
	MNode *energyGoal = m_mspace->node("EnergyGoal");
	if(!energyGoal)
	{
		energyGoal = m_mspace->addNode("EnergyGoal", MNodeType::GoalNode());
		m_mspace->link(agentGoals, energyGoal, MLinkType::OrderedLink());
		
		QVariantList goalData;
		{	
			QVariantList goalRow;
			goalRow << "MAX" << "EnergyValue" << 1.0;
			
			goalData.append(QVariant(goalRow));
		}
		
		energyGoal->setData(goalData);
		
		MNode *energyVar = m_mspace->node("EnergyValue", MNodeType::VariableNode());
		energyVar->setData(1.0);
		
		m_mspace->link(energyGoal, energyVar, MLinkType::GoalVariableLink());
	}
	m_goals << energyGoal;
}

static bool SimpleBotAgent_goalSort(MNode *n1, MNode *n2)
{
	if(!n1 || !n2)
		return false;
		
	double delta = n1->shortTermImportance() - n2->shortTermImportance();
	double rv = ((double)(rand() % 10) - 5.) / 100.;
	delta += rv;
	
	return delta > 0;
}


void SimpleBotAgent::chooseCurrentGoal()
{
	qSort(m_goals.begin(), m_goals.end(), SimpleBotAgent_goalSort);
	MNode *goal = m_goals.first();
	
	//qDebug() << "SimpleBotAgent::chooseCurrentGoal: first goal after sort: "<<goal->content();
	
	if(goal != m_currentGoal)
	{
		m_currentGoal = goal;
		
		qDebug() << "SimpleBotAgent::chooseCurrentGoal: Current goal changed, new goal: "<<goal->content();
		chooseAction();
		
	}
}

double SimpleBotAgent::calcGoalActionProb(MNode *action)
{
	/// TODO
	//return ((double)(rand() % 100)) / 100.;
	double lti = action->longTermImportance();
// 	if(lti == 1.0)
// 	{
// 		lti *= ((double)(rand() % 100)) / 100.;
// 	}
	
	double rv = ((double)(rand() % 10) - 5.) / 100.;
	lti += rv;
	
	return lti;
}

void SimpleBotAgent::chooseAction()
{
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
			
			double p = calcGoalActionProb(info.node);
			qDebug() << "SimpleBotAgent::chooseAction: p:"<<p<<", info.node:" << info.node;
			if(p >= maxProb)
			{
				maxProb = p;
				maxInfo = info;
			}
		}
	}
	
	isSetup = true;
	
	//qDebug() << "SimpleBotAgent::chooseAction: [NoOp] maxInfo.node: "<<maxInfo.node;
	//return;
	
	//if(!m_currentAction || m_currentAction->content() != maxInfo.node->content())
	{
		AgentSubsystem *bioPtr = subsystem(AgentBioSystem::className());
		AgentBioSystem *bio = dynamic_cast<AgentBioSystem*>(bioPtr);

		// clone clones first level links and nodes by default
		//m_currentAction = maxInfo.node->clone();
		if(m_currentAction)
		{
			// eval goal value change and reward STI/LTI of action accordingly
			
// 			double hungerChange = bio->hunger() - m_lastHunger;
// 			double energyChange = bio->energy() - m_lastEnergy;
// 			
// 			qDebug() << "SimpleBotAgent::chooseAction: hungerChange: "<<hungerChange<<" ("<<bio->hunger()<<"-"<<m_lastHunger<<"), energyChange: "<<energyChange<<" ("<<bio->energy()<<"-"<<m_lastEnergy<<")";
			
			
			// NOTE We need to store parameters of action in memory and reward LTI/STI for *that* combo, not just the aciton in general
			
			
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
						valueDelta = -0.1 * expectedChangeSign;
						
					/// TODO 0.1 = magic number, should it be configurable ?
					double propLtiChange = 0.1 * valueDelta * expectedChangeSign; // Basically, for things that should be minimized 
												      // (MIN eval func, -1 change sign), invert the Lti 
												      // change because the "good" delta will be going negative
					
					m_currentAction->setLongTermImportance( m_currentAction->longTermImportance() + propLtiChange );
					
					qDebug() << "SimpleBotAgent::chooseAction: " << m_currentGoal->content().toUpper()<<", new STI: "<< m_currentAction->longTermImportance()<<", action was: "<<m_currentAction->content()<<" (prop lti change: "<<propLtiChange<<", thisValue:" <<thisValue.toDouble()<<", lastValue:"<<lastValue.toDouble()<<", change:"<<valueDelta<<")";
					
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
			qDebug() << "SimpleBotAgent::chooseAction: " << m_currentGoal->content().toUpper()<<": goalDeltaSum: "<<goalDeltaSum<<", avgGoalDelta: "<<avgGoalDelta;
			
			
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
		
		qDebug() << "SimpleBotAgent::chooseAction: Chose new action: "<<m_currentAction;
	
		//qDebug() << "Action changed, cloned action node. Debug info: orig node:"<<maxInfo.node<<", orig node type: "<<maxInfo.node->type()<<", cloned type:" <<m_currentAction->type();
		
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
				qDebug() << "SimpleBotAgent::chooseAction: "<<subsys<<" subsystem accepted "<<m_currentAction->content();
			}
		}
	}
	
// 	qDebug() << "Stopping sim, exiting.";
// 	exit(-1);
	qDebug("\n\n");

	update();
}

void SimpleBotAgent::actionCompleted(MNode *currentAction)
{
	qDebug() << "SimpleBotAgent::actionCompleted: "<<currentAction;
	chooseAction();
}

//MNode *m_currentGoal;

void SimpleBotAgent::actionException(MNode *currentAction, const QString& message)
{
	/// TODO
	//qDebug() << "SimpleBotAgent::actionException: Unandled: "<<message<<", current action: "<<currentAction;
	//exit(-1);
	qDebug() << "SimpleBotAgent::actionException: "<<message<<", current action: "<<currentAction;
	
	chooseAction();
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
	
	QPointF center(0,0);
	QLineF line(center, move->vec() + center);
	painter->drawLine( line );
	
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
	MNode *lastStateNode = 0;
	if(m_state != StateUnknown)
		lastStateNode = m_mspace->node(m_state.name);
	
	QString stateName = state.name;
	qDebug() << "SimpleBotAgent::setState: "<<stateName;
	m_state = state;
	
	m_stateTimer.restart();
	
	// Ensure node exists for this state
	MNode *currentStateNode = m_mspace->node(stateName, MNodeType::ProcedureNode());
		
	if(lastStateNode)
	{
		MLink *foundLink = m_mspace->link(lastStateNode, currentStateNode);
		
		if(foundLink)
			foundLink->setTruthValue(foundLink->truthValue().value() + 0.05);
		else
			m_mspace->addLink( new MLink(lastStateNode, currentStateNode, MLinkType::EventLink(), MTruthValue(0.1)) );
	}
}
