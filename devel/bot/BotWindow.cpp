#include "MindSpace.h"
using namespace MindSpace;

#include "BotWindow.h"

#include "MindSpaceGraphWidget.h"

#include <QGraphicsPolygonItem>

#define SETTINGS_FILE "test.ini"
#define GetSettingsObject() QSettings settings(SETTINGS_FILE,QSettings::IniFormat);


BotWindow::BotWindow()
	: QWidget()
{
// 	QGraphicsView *gv = new QGraphicsView();
// 	
// 	QGraphicsScene *scene = new QGraphicsScene();
// 	
// 	QPolygonF polygon = QPolygonF()
// 		<< QPointF(0., 10.)
// 		<< QPointF(10.,10.)
// 		<< QPointF( 5., 0.);
// 	
// 	QGraphicsPolygonItem *polyItem = new QGraphicsPolygonItem(polygon);
// 		
// 	scene->addItem(polyItem);
// 	
// 	gv->setScene(scene);
	
	
	QVBoxLayout *vbox = new QVBoxLayout(this);
	//vbox->addWidget(gv);
	
	MindSpace::MSpace *mind = new MindSpace::MSpace();
	//mind->makeActive(); // ctor auto-calls makeActive
	
	GetSettingsObject();
	QVariantMap map = settings.value("mind").toMap();
	if(map.isEmpty())
	{ 
		
		// MNode ctor auto-adds nodes to the active MSpace
		mind->addNode(new MNode("human",    MNodeType::ConceptNode()));
		mind->addNode(new MNode("man",      MNodeType::ConceptNode()));
		mind->addNode(new MNode("woman",    MNodeType::ConceptNode()));
		mind->addNode(new MNode("mortal",   MNodeType::ConceptNode()));
		mind->addNode(new MNode("air",      MNodeType::ConceptNode()));
		mind->addNode(new MNode("water",    MNodeType::ConceptNode()));
		mind->addNode(new MNode("Socrates", MNodeType::SpecificEntityNode()));
		mind->addNode(new MNode("Josiah",   MNodeType::SpecificEntityNode()));
		mind->addNode(new MNode("Ashley",   MNodeType::SpecificEntityNode()));
		mind->addNode(new MNode("breath",   MNodeType::PredicateNode()));	// predicate nodes expect an incoming non-preciate node link and an outgoing non-precicate node link
		mind->addNode(new MNode("love",     MNodeType::PredicateNode()));
		
		// MLink ctor auto-adds links to the active MSpace
		mind->addLink(new MLink(_node("Socrates"), _node("man"),    MLinkType::InheritsFromLink()));
		mind->addLink(new MLink(_node("Josiah"),   _node("man"),    MLinkType::InheritsFromLink()));
		mind->addLink(new MLink(_node("Ashley"),   _node("woman"),  MLinkType::InheritsFromLink()));
		mind->addLink(new MLink(_node("man"),      _node("human"),  MLinkType::InheritsFromLink()));
		mind->addLink(new MLink(_node("woman"),    _node("human"),  MLinkType::InheritsFromLink()));
		mind->addLink(new MLink(_node("human"),    _node("mortal"), MLinkType::InheritsFromLink()));
	// 	new MLink(_node("man"),      _node("breath"), MLinkType::PredicateSubjectLink());
	// 	new MLink(_node("breath"),   _node("air"),    MLinkType::PredicateObjectLink());
	// 	new MLink(_node("breath"),   _node("water"),  MLinkType::PredicateObjectLink(), MTruthValue(0.1));
	// 	MLink *link = new MLink(_node("breath"),   _node("air"),    MLinkType::EvaluationLink());
	// 	link->setArguments( QList<MNode*>() << _node("man") << _node("air") );
	
		//new MLink(_node("breath"),   QList<MNode*>() << _node("man") << _node("air"),    MLinkType::EvaluationLink());
		//new MLink(_node("breath"),   QList<MNode*>() << _node("man") << _node("water"),  MLinkType::EvaluationLink(), MTruthValue(0.1));
		
		mind->addLink(new MLink(_node("human"),   QList<MNode*>() << _node("breath") << _node("air"),    MLinkType::PredicateLink()));
		mind->addLink(new MLink(_node("human"),   QList<MNode*>() << _node("breath") << _node("water"),  MLinkType::PredicateLink(), MTruthValue(0.1)));
		mind->addLink(new MLink(_node("Josiah"),   QList<MNode*>() << _node("love") << _node("Ashley"),  MLinkType::PredicateLink()));
		
		settings.setValue("mind", mind->toVariantMap());
	}
	else
	{
		//qDebug() << "Loading MindSpace from variant map...";
		mind->fromVariantMap(map);
	}
	

	MindSpaceGraphWidget *gw = new MindSpaceGraphWidget();
	vbox->addWidget(gw);
	
	gw->setMindSpace(mind);
	
	QPushButton *testBtn = new QPushButton("Test Button");
	connect(testBtn, SIGNAL(clicked()), this, SLOT(addTestItem()));
	vbox->addWidget(testBtn);

}

void BotWindow::addTestItem()
{
	MSpace *mind = MSpace::activeSpace();
	
	mind->addNode(_node("child",    MNodeType::ConceptNode()));
	mind->addLink(new MLink(_node("child"),      _node("human"),  MLinkType::InheritsFromLink()));
	
	mind->addNode(new MNode("Tanner",    MNodeType::SpecificEntityNode()));
	mind->addLink(new MLink(_node("Tanner"),      _node("child"),  MLinkType::InheritsFromLink()));
	
}

void BotWindow::closeEvent(QCloseEvent*)
{
	MSpace *mind = MSpace::activeSpace();
	
	GetSettingsObject();
	settings.setValue("mind", mind->toVariantMap());
}
