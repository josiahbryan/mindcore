#include "MindSpace.h"
using namespace MindSpace;

#include "BotWindow.h"

#include "MindSpaceGraphWidget.h"

#include <QGraphicsPolygonItem>

#define SETTINGS_FILE "nonkline.dat"
#define GetSettingsObject() QSettings settings(SETTINGS_FILE,QSettings::IniFormat);

#include "MindSpace.h"
using namespace MindSpace;


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
	
	qDebug() << "Loading "<<SETTINGS_FILE<<" ...";
	if (!mind->loadFromFile(SETTINGS_FILE)) 
	{
		mind->importConceptNet2File();
		mind->writeToFile(SETTINGS_FILE);
	}
	
	qDebug() << "Done loading.";

	MindSpaceGraphWidget *gw = new MindSpaceGraphWidget();
	connect(gw, SIGNAL(nodeDoubleClicked(MNode*)), this, SLOT(nodeDoubleClicked(MNode*)));
	//gw->setMindSpace(mind);
	
	m_gw = gw;
	vbox->addWidget(gw);
	
	
	QHBoxLayout *hbox = new QHBoxLayout();
	
	QPushButton *btn = new QPushButton("< Back");
	connect(btn, SIGNAL(clicked()), this, SLOT(backBtnClicked()));
	hbox->addWidget(btn);
	m_backBtn = btn;
	m_backBtn->setEnabled(false);
	
	m_textBox = new QLineEdit(this);
	hbox->addWidget(m_textBox);
	
	btn = new QPushButton("Search");
	connect(btn, SIGNAL(clicked()), this, SLOT(searchBtnClicked()));
	hbox->addWidget(btn);
	
	btn = new QPushButton("Stop Layout");
	connect(btn, SIGNAL(clicked()), m_gw, SLOT(stopLayout()));
	hbox->addWidget(btn);
	
// 	btn->hide();
// 	
// 	connect(m_gw, SIGNAL(layoutStarted()), btn, SLOT(show()));
// 	connect(m_gw, SIGNAL(layoutStopped()), btn, SLOT(hide()));
	
	btn = new QPushButton("Auto-Adjust");
	connect(btn, SIGNAL(clicked()), m_gw, SLOT(stepLayout()));
	hbox->addWidget(btn);
	connect(m_gw, SIGNAL(layoutStarted()), btn, SLOT(hide()));
	connect(m_gw, SIGNAL(layoutStopped()), btn, SLOT(show()));
	
	vbox->addLayout(hbox);
	
	search("eat");
}


void BotWindow::closeEvent(QCloseEvent*)
{
	MSpace *mind = MSpace::activeSpace();
	mind->writeToFile(SETTINGS_FILE);
}

void BotWindow::nodeDoubleClicked(MNode* node)
{
	search(node->content());
}

void BotWindow::searchBtnClicked()
{
	search(m_textBox->text());
}

void BotWindow::backBtnClicked()
{
	if(m_history.isEmpty())
		return;
	QString lastItem = m_history.takeLast();
	//qDebug() << "BotWindow::backBtnClicked(): lastItem:"<<lastItem;
	search(lastItem, false); // false = dont add to history
	
	//qDebug() << "\t History: "<<m_history<<", m_lastSearch:"<<m_lastSearch;
	
}

void BotWindow::search(const QString& text, bool addToHistory)
{
	qDebug() << "BotWindow::search(): Searching for "<<text;
	if(addToHistory)
	{
		if(!m_lastSearch.isEmpty())
		{
			//qDebug() << "\t Adding to history: "<<m_lastSearch;
			m_history << m_lastSearch;
			m_backBtn->setEnabled(true);
			
		}
	}
	
	m_lastSearch = text;

	//qDebug() << "\t History: "<<m_history<<", m_lastSearch:"<<m_lastSearch;

	m_gw->clearScene();
	//m_gw->scaleView(0.5);
	//m_gw->scale(0.5,0.5);
	m_gw->mapNode(_node(text), 2);
	m_textBox->setText(text);
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
		


