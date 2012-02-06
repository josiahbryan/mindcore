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
	m_rect  = QRect(10,10,150,50);
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
	
	
	int fontSize = 10;
	int margin = fontSize/2;
	int y = margin;
	
	p->save();
	p->setPen(Qt::white);
	p->setFont(QFont("Monospace", fontSize, QFont::Bold));
	p->drawText(rect.topLeft() + QPoint(margin, y += fontSize), QString( "State:  %1" ).arg(stateName));
	p->drawText(rect.topLeft() + QPoint(margin, y += fontSize), QString( "Hunger: %1" ).arg(bio->hunger()));
	p->drawText(rect.topLeft() + QPoint(margin, y += fontSize), QString( "Energy: %1" ).arg(bui->energy()));
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
		MNode *hungerGoal = m_mspace->addNode("HungerGoal", MNodeType::GoalNode());
		
		// NOTE Idea: Perhaps the link weight (Tv) could be the way to rank the goals...
		// NOTE Revision: The lti/sti would be a better ranking...
		m_mspace->link(agentGoals, hungerGoal, MLinkType::OrderedLink());
		
		QVariantList goalData;
		{	
			QVariantList goalRow;
			goalRow << "MIN" << "HungerValue";
			
			goalData << goalRow;
		}
		
		hungerGoal->setData(goalData);
		
		MNode *hungerVar = m_mspace->node("HungerValue", MNodeType::VariableNode());
		hungerVar->setData(1.0);
		
		m_mspace->link(hungerGoal, hungerVar, MLinkType::GoalVariableLink());
	}
	m_goals << hungerGoal;
	
	MNode *energyGoal = m_mspace->node("EnergyGoal");
	if(!energyGoal)
	{
		MNode *energyGoal = m_mspace->addNode("EnergyGoal", MNodeType::GoalNode());
		m_mspace->link(agentGoals, energyGoal, MLinkType::OrderedLink());
		
		QVariantList goalData;
		{	
			QVariantList goalRow;
			goalRow << "MAX" << "EnergyValue";
			
			goalData << goalRow;
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
	return false if !n1 || !n2;
	return n1->shortTermImportance() < n2->shortTermImportance();
}


void SimpleBotAgent::chooseCurrentGoal()
{
	qSort(m_goals.begin(), m_goals.end(), SimpleBotAgent_goalSort);
	MNode *goal = m_goals.first();
	
	if(goal != m_currentGoal)
	{
		qDebug() << "SimpleBotAgent::chooseCurrentGoal: Current goal changed, new goal: "<<goal->content();
		chooseAction();
	}
}

double SimpleBotAgent::calcGoalActionProb(MNode *action)
{
	/// TODO
	return ((double)(rand() % 100) / 100.;
}

void SimpleBotAgent::chooseAction()
{
	AgentSubsystem::ActionInfo maxInfo;
	double maxProb = 0.0;
	foreach(AgentSubsystem *subsys, m_subsystems)
	{
		QList<AgentSubsystem::ActionInfo> actions = subsys->actions();
		foreach(AgentSubsystem::ActionInfo info, actions)
		{
			double p = calcGoalActionProb(info.node);
			if(p >= maxProb)
			{
				maxProb = p;
				maxInfo = info;
			}
		}
	}
	
	if(m_currentAction != maxInfo.node)
	{
		// clone clones first level links and nodes by default
		m_currentAction = maxInfo.node->clone();
		
		QList<MNode*> vars = m_currentAction->linkedNode(MNodeType::VariableNode());
		
		/// TODO DON'T CHEAT
		/// FIXME CHEATING
		
		foreach(MNode *node, vars)
		{
			if(node->content() == "EatTime" ||
			   node->content() == "RestTime")
			{
				node->setData( rand() % 5000 );
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
		}
		
		foreach(AgentSubsystem *subsys, m_subsystems)
		{
			if(!subsys->executeAction(m_currentAction))
			{
				qDebug() << "SimpleBotAgent::chooseAction: "<<subsys<<" - subsys didnt want action "<<m_currentAction->content(); 
			}
		}
	}
	
	qDebug() << "SimpleBotAgent::chooseAction: Chose new action: "<<m_currentAction;
}

void SimpleBotAgent::actionCompleted(MNode *currentAction)
{
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
	
	QPointF center(0,0);
	QLineF line(center, m_vec + center);
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
	// Set initial state
	if(m_state == StateUnknown)
		setState(StateSearching);
		
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
	
	
	// Update bio variables
	updateHungerEnergyState();
	
	// m_hunger increases as m_energy decreases
	// m_energy increases when we eat or when we rest
	// m_energy decreases as time progresses in any state other than resting or eating
	
	//m_label = QString("%1/%2").arg(m_hunger).arg(m_energy);
	
	// Process current state
	if(m_state == StateResting)
	{
		processResting();
	}
	else
	if(m_state == StateSearching)
	{
		processSearching();
	}
	else
	if(m_state == StateEating)
	{
		processEating();
	}
	else
	if(m_state == StateAsking)
	{
		//
	}
	
	// Check to see if state change required
	evaulateStateChangeRequired();
		
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
