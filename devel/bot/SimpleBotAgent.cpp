#include "SimpleBotAgent.h"
#include "SimpleBotEnv.h"

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
 
void SimpleBotAgent::InfoDisplay::paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *)
{
	SimpleBotAgent *agent = m_agent;
	if(!agent)
		return;
	
	QRect rect = m_rect;
	
	p->setPen(Qt::black);
	p->fillRect(rect, QColor(0,0,0,187));
	p->drawRect(rect);
	
	QString stateName = agent->state().name;
	
	int fontSize = 10;
	int margin = fontSize/2;
	int y = margin;
	
	p->save();
	p->setPen(Qt::white);
	p->setFont(QFont("Monospace", fontSize, QFont::Bold));
	p->drawText(rect.topLeft() + QPoint(margin, y += fontSize), QString( "State:  %1" ).arg(stateName));
	p->drawText(rect.topLeft() + QPoint(margin, y += fontSize), QString( "Hunger: %1" ).arg(agent->m_hunger));
	p->drawText(rect.topLeft() + QPoint(margin, y += fontSize), QString( "Energy: %1" ).arg(agent->m_energy));
	p->restore();
}

SimpleBotAgent::InfoDisplay *SimpleBotAgent::infoItem()
{
	if(!m_hud)
		m_hud = new SimpleBotAgent::InfoDisplay(this);
	
	return m_hud;
}

SimpleBotAgent::SimpleBotAgent()
	: QObject()
	, QGraphicsItem()
	, m_env(0)
	, m_mspace(0)
	, m_label("")
	, m_color(Qt::black)
	, m_hud(0)
{
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
}

void SimpleBotAgent::setEnv(SimpleBotEnv *env)
{
	m_env = env;
	if(env)
		env->addItem(this);
}

void SimpleBotAgent::setMindSpace(MSpace *ms)
{
	m_mspace = ms;
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

void SimpleBotAgent::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
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
		double angle = chooseVector();
		
		qDebug() << "SimpleBotAgent::advance: S_Searching: new m_vec: "<<m_vec<<", new angle: "<<angle;
	}
	//else
		//qDebug() << "SimpleBotAgent::advance: S_Searching: new pos: "<<newPos;
	
	setPos(newPos);
}

double SimpleBotAgent::chooseVector()
{
	m_vec = QPointF(
		((double)(rand() % 100)) / 10. - 5.,
		((double)(rand() % 100)) / 10. - 5.
	);
	
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
	MNode *currentStateNode = m_mspace->node(stateName, MNodeType::ProceduralNode());
		
	if(lastStateNode)
	{
		MLink *foundLink = m_mspace->link(lastStateNode, currentStateNode);
		
		if(foundLink)
			foundLink->setTruthValue(foundLink->truthValue().value() + 0.05);
		else
			m_mspace->addLink( new MLink(lastStateNode, currentStateNode, MLinkType::EventLink(), MTruthValue(0.1)) );
	}
}

// QString SimpleBotAgent::nameForState(StateType state)
// {
// 	switch(state)
// 	{
// 		case S_Resting:
// 			return "resting";
// 		case S_Searching:
// 			return "searching";
// 		case S_Eating:
// 			return "eating";
// 		case S_AskForMore:
// 			return "asking for more";
// 		default:
// 			break;
// 	};
// 	
// 	return "?";
// }
// 
// 
// 
