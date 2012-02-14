#include "MindSpace.h"
using namespace MindSpace;

#include "BotWindow.h"

#include "MindSpaceGraphWidget.h"

#include <QGraphicsPolygonItem>

#define SETTINGS_FILE "simplebot.dat"

#include "MindSpace.h"
#include "MSpaceViewerWidget.h"
using namespace MindSpace;

#include "SimpleBotAgent.h"
#include "SimpleBotEnv.h"

#include "FoodItem.h"

TweakedGraphicsView::TweakedGraphicsView()
	: QGraphicsView()
{
	setCacheMode(CacheBackground);
	setViewportUpdateMode(BoundingRectViewportUpdate);
	//setRenderHint(QPainter::Antialiasing);
	setTransformationAnchor(AnchorUnderMouse);
	setResizeAnchor(AnchorViewCenter);
	setDragMode(QGraphicsView::ScrollHandDrag);
	
	setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );
	// if there are ever graphic glitches to be found, remove this again
	setOptimizationFlags(QGraphicsView::DontAdjustForAntialiasing | QGraphicsView::DontClipPainter | QGraphicsView::DontSavePainterState);

	//setCacheMode(QGraphicsView::CacheBackground);
	//setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
	setOptimizationFlags(QGraphicsView::DontSavePainterState);
	
// 	// Magic numbers - just fitting approx to view
	scaleView(1.41421);
	scaleView(1.41421);
}


void TweakedGraphicsView::wheelEvent(QWheelEvent *event)
{
	scaleView(pow((double)2, event->delta() / 240.0));
}

void TweakedGraphicsView::drawBackground(QPainter *painter, const QRectF &rect)
{
	Q_UNUSED(rect);
	
	QRectF sceneRect = this->sceneRect();
	QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
	gradient.setColorAt(0, Qt::white);
	gradient.setColorAt(1, Qt::lightGray);
	painter->fillRect(rect.intersect(sceneRect), gradient);
	painter->setBrush(Qt::NoBrush);
	painter->drawRect(sceneRect);
}

void TweakedGraphicsView::scaleView(qreal scaleFactor)
{
	//qDebug() << "TweakedGraphicsView::scaleView: "<<scaleFactor;
	
	qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
	//qDebug() << "Scale factor:" <<factor;
	if (factor < 0.001 || factor > 100)
		return;
	
	scale(scaleFactor, scaleFactor);
}


BotWindow::BotWindow()
	: QWidget()
{
	m_mspace = new MindSpace::MSpace();
	if(!m_mspace->loadFromFile(SETTINGS_FILE))
		m_mspace->writeToFile(SETTINGS_FILE); // write blank file if didn't load one

	SimpleBotEnv *env = new SimpleBotEnv();
	
	SimpleBotAgent *bot = new SimpleBotAgent(m_mspace);
	bot->setEnv(env);
	//bot->setMindSpace(m_mspace);
	
	env->addItem(bot->infoItem());
	
	FoodItem *food = new FoodItem();
	env->addItem(food);
	
	food->setPos(rand() % 200, rand() % 200); 
	
	m_gv = new TweakedGraphicsView();
	m_gv->setScene(env);
	m_gv->scaleView(0.707107);
	
	env->setItemIndexMethod(QGraphicsScene::NoIndex);
	env->setSceneRect(0, 0, 200, 200);
	
	bot->setPos( env->sceneRect().center() );
	
	
	
	QVBoxLayout *vbox = new QVBoxLayout(this);
	
	QSplitter *split = new QSplitter(this);
	split->setOrientation(Qt::Vertical);
	//vbox->addWidget(m_gv);
	vbox->addWidget(split);
	
	split->addWidget(m_gv);
	split->setStretchFactor(0, 2);
	
	m_mspaceViewer = new MSpaceViewerWidget();
	m_mspaceViewer->layout()->setContentsMargins(0,0,0,0);
	//vbox->addWidget(m_mspaceViewer);
	split->addWidget(m_mspaceViewer);
	
	m_mspaceViewer->setMindSpace(m_mspace, 
		// Create node type filter list
		QList<MNodeType>() 
		<< MNodeType::ActionMemoryNode()
		<< MNodeType::VariableSnapshotNode()
	);
	
	resize(1024,768);
	//m_gv->fitInView(env->itemsBoundingRect(), Qt::KeepAspectRatio);
	
	//bot->setGoal("foobar"); 
	bot->start();
}


void BotWindow::closeEvent(QCloseEvent*)
{
 	MSpace *mind = MSpace::activeSpace();
 	mind->writeToFile(SETTINGS_FILE);
}



	
// 		// MNode ctor auto-adds nodes to the active MSpace
// 		mind->addNode(new MNode("human",    MNodeType::ConceptNode()));
// 		mind->addNode(new MNode("man",      MNodeType::ConceptNode()));
// 		mind->addNode(new MNode("woman",    MNodeType::ConceptNode()));
// 		mind->addNode(new MNode("mortal",   MNodeType::ConceptNode()));
// 		mind->addNode(new MNode("air",      MNodeType::ConceptNode()));
// 		mind->addNode(new MNode("water",    MNodeType::ConceptNode()));
// 		mind->addNode(new MNode("Socrates", MNodeType::SpecificEntityNode()));
// 		mind->addNode(new MNode("Josiah",   MNodeType::SpecificEntityNode()));
// 		mind->addNode(new MNode("Ashley",   MNodeType::SpecificEntityNode()));
// 		mind->addNode(new MNode("breath",   MNodeType::PredicateNode()));	// predicate nodes expect an incoming non-preciate node link and an outgoing non-precicate node link
// 		mind->addNode(new MNode("love",     MNodeType::PredicateNode()));
// 		
// 		// MLink ctor auto-adds links to the active MSpace
// 		mind->addLink(new MLink(_node("Socrates"), _node("man"),    MLinkType::InheritsFromLink()));
// 		mind->addLink(new MLink(_node("Josiah"),   _node("man"),    MLinkType::InheritsFromLink()));
// 		mind->addLink(new MLink(_node("Ashley"),   _node("woman"),  MLinkType::InheritsFromLink()));
// 		mind->addLink(new MLink(_node("man"),      _node("human"),  MLinkType::InheritsFromLink()));
// 		mind->addLink(new MLink(_node("woman"),    _node("human"),  MLinkType::InheritsFromLink()));
// 		mind->addLink(new MLink(_node("human"),    _node("mortal"), MLinkType::InheritsFromLink()));
// 	// 	new MLink(_node("man"),      _node("breath"), MLinkType::PredicateSubjectLink());
// 	// 	new MLink(_node("breath"),   _node("air"),    MLinkType::PredicateObjectLink());
// 	// 	new MLink(_node("breath"),   _node("water"),  MLinkType::PredicateObjectLink(), MTruthValue(0.1));
// 	// 	MLink *link = new MLink(_node("breath"),   _node("air"),    MLinkType::EvaluationLink());
// 	// 	link->setArguments( QList<MNode*>() << _node("man") << _node("air") );
// 	
// 		//new MLink(_node("breath"),   QList<MNode*>() << _node("man") << _node("air"),    MLinkType::EvaluationLink());
// 		//new MLink(_node("breath"),   QList<MNode*>() << _node("man") << _node("water"),  MLinkType::EvaluationLink(), MTruthValue(0.1));
// 		
// 		mind->addLink(new MLink(_node("human"),   QList<MNode*>() << _node("breath") << _node("air"),    MLinkType::PredicateLink()));
// 		mind->addLink(new MLink(_node("human"),   QList<MNode*>() << _node("breath") << _node("water"),  MLinkType::PredicateLink(), MTruthValue(0.1)));
// 		mind->addLink(new MLink(_node("Josiah"),   QList<MNode*>() << _node("love") << _node("Ashley"),  MLinkType::PredicateLink()));
		


