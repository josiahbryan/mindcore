#include "MindSpace.h"
using namespace MindSpace;

#include "MSpaceViewerWidget.h"
#include "MindSpaceGraphWidget.h"

#include <QGraphicsPolygonItem>

#include "MindSpace.h"
using namespace MindSpace;


MSpaceViewerWidget::MSpaceViewerWidget()
	: QWidget()
{
	QVBoxLayout *vbox = new QVBoxLayout(this);
	
	m_mind = 0;
	//mind->makeActive(); // ctor auto-calls makeActive
	

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
}


void MSpaceViewerWidget::closeEvent(QCloseEvent*)
{
	//MSpace *mind = MSpace::activeSpace();
	//mind->writeToFile(SETTINGS_FILE);
}

void MSpaceViewerWidget::nodeDoubleClicked(MNode* node)
{
	search(node->content());
}

void MSpaceViewerWidget::searchBtnClicked()
{
	search(m_textBox->text());
}

void MSpaceViewerWidget::backBtnClicked()
{
	if(m_history.isEmpty())
		return;
	QString lastItem = m_history.takeLast();
	//qDebug() << "MSpaceViewerWidget::backBtnClicked(): lastItem:"<<lastItem;
	search(lastItem, false); // false = dont add to history
	
	//qDebug() << "\t History: "<<m_history<<", m_lastSearch:"<<m_lastSearch;
	
}

void MSpaceViewerWidget::search(const QString& text, bool addToHistory)
{
	_search(text, addToHistory);
}

void MSpaceViewerWidget::_search(const QString& text, bool addToHistory, bool reallyMap)
{
	qDebug() << "MSpaceViewerWidget::search(): Searching for "<<text;
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
	if(reallyMap)
		m_gw->mapNode(_node(text), 2);
		
	m_textBox->setText(text);
}


/// Interface methods


void MSpaceViewerWidget::loadConceptNetDemo()
{
	if(m_mind)
		delete m_mind;
	
	MindSpace::MSpace *mind = new MindSpace::MSpace();
	m_mind = mind;
	
	QString dataFile = "nonkline.dat";
	QString conceptNetFile = "/opt/mindcore/data/src/conceptnet2/predicates_concise_nonkline.txt";
	
	qDebug() << "Loading "<<dataFile<<" ...";
	if(!mind->loadFromFile(dataFile)) 
	{
		mind->importConceptNet2File(conceptNetFile);
		mind->writeToFile(dataFile);
	}
	
	qDebug() << "Done loading.";
	
	search("eat");
}

void MSpaceViewerWidget::loadFile(QString file)
{
	if(m_mind)
		delete m_mind;
	
	MindSpace::MSpace *mind = new MindSpace::MSpace();
	m_mind = mind;
	
	m_mind->loadFromFile(file);
	const QList<MNode*> & list = m_mind->nodes();
	if(!list.isEmpty())
		search(list.first()->content());
	else
		qDebug() << "MSpaceViewerWidget::loadFile(): File "<<file<<" has no nodes defined"; 
}

void MSpaceViewerWidget::setMindSpace(MSpace *mspace)
{
	if(m_mind)
		delete m_mind;
	
	// NOTE: DON'T set m_mind=mspace because it will get deleted if one of the above methods is called
	
	// This will graph the entire mspace
	m_gw->setMindSpace(mspace);
}

void MSpaceViewerWidget::mapNode(MNode *node, int levels)
{
	m_history.clear();
	m_backBtn->setEnabled(false);
	
	// Update UI only, last false tells it not to call m_gw->mapNode()
	_search(node->content(), true, false);
	
	m_gw->mapNode(node, levels);
	
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
		


