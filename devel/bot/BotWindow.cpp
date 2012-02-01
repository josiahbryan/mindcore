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
	
	QFile dataFile(SETTINGS_FILE);
	qDebug() << "Loading "<<SETTINGS_FILE<<" ...";
	if (!dataFile.open(QIODevice::ReadOnly)) 
	{
		
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
		
		
		QString filename = "/opt/mindcore/data/src/conceptnet2/predicates_concise_nonkline.txt";
		// my ($linktype, $arg1, $arg2, $freq, $infer) = ($line =~ /^\(([^\s]+)\s+("[^"]*")\s+("[^"]*")\s+"f=(\d+);i=(\d+);"\)/);
		
		// \(([^\s]+)\s+("[^"]*")\s+("[^"]*")\s+"f=(\d+);i=(\d+);"\)
		QRegExp rx("\\(([^\\s]+)\\s+\"([^\"]*)\"\\s+\"([^\"]*)\"\\s+\"f=(\\d+);i=(\\d+);\"\\)");
		
		QFile file(filename);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			qDebug() << "Error opening "<<filename<<", exiting.";
			exit(-1);
		}
		
		int counter = 0;
		while (!file.atEnd()) 
		{
			QByteArray lineBytes = file.readLine();
			QString line(lineBytes);
			int idx = rx.indexIn(line);
			QStringList values = rx.capturedTexts();
			
			QString linkType = values[1];
			QString arg1 = values[2];
			QString arg2 = values[3];
			int freq = values[4].toInt(); // not used yet...
			int infer = values[5].toInt(); // not used yet...
			
			qDebug() << ++counter << "Loading: "<<arg1<<" -> "<<linkType<<" -> "<<arg2;
			
			if(!_node(arg1))
				mind->addNode(new MNode(arg1, MNodeType::ConceptNode()));
			if(!_node(arg2))
				mind->addNode(new MNode(arg2, MNodeType::ConceptNode()));
				
			mind->addLink(new MLink(_node(arg1), _node(arg2), MLinkType::findLinkType(linkType)));
			
// 			if(counter > 5000)
// 			{
// 				// stop after X just for testing
// 				break;
// 			}
			
				
// 			(CapableOf "red bicycle" "pump" "f=2;i=1;")
// 			(IsA "spider" "bug" "f=3;i=0;")
// 			(EffectOf "have party" "tire" "f=2;i=0;")
// 			(LocationOf "steer wheel" "in car" "f=2;i=0;")
// 			(LocationOf "waitress" "in restaurant" "f=2;i=0;")

			
		}
		
		
		dataFile.open(QIODevice::WriteOnly);
		dataFile.write(mind->toByteArray());
		dataFile.close();
	}
	else
	{
		//qDebug() << "Loading MindSpace from variant map...";
		QByteArray array = dataFile.readAll();
		mind->fromByteArray(array);
	}
	
	qDebug() << "Done loading.";

	MindSpaceGraphWidget *gw = new MindSpaceGraphWidget();
	vbox->addWidget(gw);
	m_gw = gw;
	
	//gw->mapNode(_node("fun"), 2);
	connect(gw, SIGNAL(nodeDoubleClicked(MNode*)), this, SLOT(nodeDoubleClicked(MNode*)));
	//gw->setMindSpace(mind);
	
// 	QPushButton *testBtn = new QPushButton("Test Button");
// 	connect(testBtn, SIGNAL(clicked()), this, SLOT(addTestItem()));
// 	vbox->addWidget(testBtn);
	
	QHBoxLayout *hbox = new QHBoxLayout();
	
	QPushButton *btn = new QPushButton("< Back");
	connect(btn, SIGNAL(clicked()), this, SLOT(backBtnClicked()));
	hbox->addWidget(btn);
	m_backBtn = btn;
	m_backBtn->setEnabled(false);
	
	m_textBox = new QLineEdit(this);
	//m_textBox->setText("fun");
	hbox->addWidget(m_textBox);
	
	/*QPushButton **/btn = new QPushButton("Search");
	connect(btn, SIGNAL(clicked()), this, SLOT(searchBtnClicked()));
	hbox->addWidget(btn);
	
	btn = new QPushButton("Stop Layout");
	connect(btn, SIGNAL(clicked()), m_gw, SLOT(stopLayout()));
	hbox->addWidget(btn);
	
	btn->hide();
	
	connect(m_gw, SIGNAL(layoutStarted()), btn, SLOT(show()));
	connect(m_gw, SIGNAL(layoutStopped()), btn, SLOT(hide()));
	
	btn = new QPushButton("Auto-Adjust");
	connect(btn, SIGNAL(clicked()), m_gw, SLOT(stepLayout()));
	hbox->addWidget(btn);
	
	//btn->hide();
	
	connect(m_gw, SIGNAL(layoutStarted()), btn, SLOT(hide()));
	connect(m_gw, SIGNAL(layoutStopped()), btn, SLOT(show()));
	
	vbox->addLayout(hbox);
	
	search("fun");
}

// void BotWindow::addTestItem()
// {
// 	MSpace *mind = MSpace::activeSpace();
// 	
// 	mind->addNode(_node("child",    MNodeType::ConceptNode()));
// 	mind->addLink(new MLink(_node("child"),      _node("human"),  MLinkType::InheritsFromLink()));
// 	
// 	mind->addNode(new MNode("Tanner",    MNodeType::SpecificEntityNode()));
// 	mind->addLink(new MLink(_node("Tanner"),      _node("child"),  MLinkType::InheritsFromLink()));
// 	
// }

void BotWindow::closeEvent(QCloseEvent*)
{
	MSpace *mind = MSpace::activeSpace();
	
// 	GetSettingsObject();
// 	settings.setValue("mind", mind->toVariantMap());
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

