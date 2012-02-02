#include "SimpleBotAgent.h"
#include "SimpleBotEnv.h"

SimpleBotAgent::SimpleBotAgent()
	: QObject()
	, QGraphicsItem()
	, m_env(0)
	, m_mspace(0)
	, m_label("")
	, m_color(Qt::black)
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
	m_state = S_Unknown;
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
	QRectF shapeRect = QRectF( 0 - adjust,  0 - adjust,
		                  20 + adjust, 20 + adjust);

	QFont font("", 5);
	QFontMetrics metrics(font);
	QRectF textRect = QRectF(metrics.boundingRect(m_label));
	textRect.moveTo(5,10);
	
	return shapeRect.united(textRect.adjusted(-adjust,-adjust,+adjust,+adjust));
	
}

QPainterPath SimpleBotAgent::shape() const
{
	QPainterPath path;
	//path.addEllipse(-10, -10, 20, 20);
	QPolygonF polygon = QPolygonF()
		<< QPointF(0., 20.)
		<< QPointF(20.,20.)
		<< QPointF(10., 0.);
	path.addPolygon(polygon);
	return path;
}

void SimpleBotAgent::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
// 	painter->setPen(Qt::NoPen);
// 	painter->setBrush(QColor(0,0,0,100)); //Qt::darkGray);
// 	painter->drawEllipse(-9.5, -9.5, 20, 20);
	
// 	QRadialGradient gradient(-3, -3, 10);
// 	if (option->state & QStyle::State_Sunken) 
// 	{
// 		gradient.setCenter(3, 3);
// 		gradient.setFocalPoint(3, 3);
// 		gradient.setColorAt(1, m_color.darker(220));
// 		gradient.setColorAt(0, m_color.lighter(420));
// 	}
// 	else 
// 	{
// 		gradient.setColorAt(0, m_color.lighter(420));
// 		gradient.setColorAt(1, m_color.darker(120));
// 	}
// 	
// 	painter->setBrush(gradient);

	QPolygonF polygon = QPolygonF()
		<< QPointF(0., 10.)
		<< QPointF(10.,10.)
		<< QPointF( 5., 0.);
	
// 	if (option->state & QStyle::State_Sunken)
// 		painter->setBrush(m_color.lighter(400)); 
// 	else
		painter->setBrush(m_color);
		
	//painter->setPen(QPen(Qt::black, 0));
	//painter->drawEllipse(-10, -10, 20, 20);
	painter->drawConvexPolygon(polygon);
	//painter->fillRect(-10, -10, 20, 20, m_color);
	
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
	// Simple state machine here, perhaps
	
	// States:
	// Searching, Eating, Resting, Ask For More
	
	if(m_state == S_Unknown)
	{
		chooseVector();
		setState(S_Searching);
	}
	
	MNode *currentState = m_mspace->node(nameForState(m_state));
	
	int elapsed = m_timer.restart();
	
	double rate = (((double)elapsed) / 100.) * 0.1;
	m_hunger += rate * 0.5;
	m_energy -= rate * 0.7;
	
	// m_hunger and m_energy
	
	// m_hunger increases as m_energy decreases
	// m_energy increases when we eat or when we rest
	// m_energy decreases as time progresses in any state other than resting or eating
	m_label = QString("%1/%2").arg(m_hunger).arg(m_energy);
	
	switch(m_state)
	{
		case S_Resting:
			// What do we do?
			// When are we done?
			m_energy += rate;
			m_hunger -= rate * 0.1;
			
			if(m_stateTimer.elapsed() / 1000  > rand() % 10)
				setState(S_Searching);
			else
				qDebug() << "SimpleBotAgent::advance: S_Resting: m_energy: "<<m_energy<<", m_hunger: "<<m_hunger;
			break;
			
		case S_Searching:
			{
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
				else
					qDebug() << "SimpleBotAgent::advance: S_Searching: new pos: "<<newPos;
				
				setPos(newPos);
				
				if(m_stateTimer.elapsed() / 1000 > rand() % 10)
					setState(rand() % 1 == 0 ? S_Eating : S_Resting);
			}
			break;
			
		case S_Eating:
			m_hunger -= rate * 1.5;
			if(m_stateTimer.elapsed() / 1000  > 1.0)
				setState(S_Searching);
			else
				qDebug() << "SimpleBotAgent::advance: S_Eating: m_hunger: "<<m_hunger;
			break;
			
		case S_AskForMore:
			break;
			
		default:
			break;
	
	}
	
	if(m_hunger < 0)
		m_hunger = 0;
	if(m_hunger > 1)
		m_hunger = 1;
	if(m_energy < 0)
		m_energy = 0;
	if(m_energy > 1)
		m_energy = 1;
	
}

double SimpleBotAgent::chooseVector()
{
	m_vec = QPointF(
		((double)(rand() % 100)) / 10. - 5.,
		((double)(rand() % 100)) / 10. - 5.
	);
	
	QLineF line(QPointF(0,0), m_vec);
	setRotation(line.angle() + 90);
	
	return line.angle();
}

void SimpleBotAgent::setState(StateType state)
{
	MNode *lastState = 0;
	if(m_state != S_Unknown)
		lastState = m_mspace->node(nameForState(m_state));
	
	QString stateName = nameForState(state);
	qDebug() << "SimpleBotAgent::setState: "<<stateName;
	m_state = state;
	
	MNode *currentState = m_mspace->node(stateName, MNodeType::ProceduralNode());
		
	m_stateTimer.restart();
	
	if(lastState)
	{
		const QList<MLink*> & links = lastState->links();
		MLink *foundLink = 0;
		foreach(MLink *link, links)
		{
			if(link->node1() == lastState && link->node2() == currentState)
				foundLink = link;
		}
		
		if(foundLink)
		{	
			foundLink->setTruthValue(foundLink->truthValue().value() + 0.05);
			//qDebug() << "\t Found existing link between last state and this state, updating truth value to: "<<foundLink->truthValue().value();
		}	
		else
		{
			//qDebug() << "\t Link between states not found, creating ...";
			m_mspace->addLink( new MLink(lastState, currentState, MLinkType::EventLink(), MTruthValue(0.1)) );
		}
	}
}

QString SimpleBotAgent::nameForState(StateType state)
{
	switch(state)
	{
		case S_Resting:
			return "resting";
		case S_Searching:
			return "searching";
		case S_Eating:
			return "eating";
		case S_AskForMore:
			return "asking for more";
		default:
			break;
	};
	
	return "?";
}



