#include "../MindSpace.h"

#include <QtGui/QApplication>
#include <QDebug>

using namespace MindSpace;

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

// 	qDebug() << "ConceptNode:"<<MNodeType::ConceptNode();
// 	qDebug() << "PartOfLink:"<<MLinkType::PartOfLink();
	
	_node("man",      MNodeType::ConceptNode());
	_node("mortal",   MNodeType::ConceptNode());
	_node("air",      MNodeType::ConceptNode());
	_node("water",    MNodeType::ConceptNode());
	_node("Socrates", MNodeType::SpecificEntityNode());
	_node("breath",   MNodeType::PredicateNode());	// predicate nodes expect an incoming non-preciate node link and an outgoing non-precicate node link
	
	new MLink(_node("Socrates"), _node("man"),    MLinkType::InheritsFromLink());
	new MLink(_node("man"),      _node("mortal"), MLinkType::InheritsFromLink());
// 	new MLink(_node("man"),      _node("breath"), MLinkType::PredicateSubjectLink());
// 	new MLink(_node("breath"),   _node("air"),    MLinkType::PredicateObjectLink());
// 	new MLink(_node("breath"),   _node("water"),  MLinkType::PredicateObjectLink(), MTruthValue(0.1));
// 	MLink *link = new MLink(_node("breath"),   _node("air"),    MLinkType::EvaluationLink());
// 	link->setArguments( QList<MNode*>() << _node("man") << _node("air") );

	//new MLink(_node("breath"),   QList<MNode*>() << _node("man") << _node("air"),    MLinkType::EvaluationLink());
	//new MLink(_node("breath"),   QList<MNode*>() << _node("man") << _node("water"),  MLinkType::EvaluationLink(), MTruthValue(0.1));
	
	new MLink(_node("man"),   QList<MNode*>() << _node("breath") << _node("air"),    MLinkType::PredicateLink());
	new MLink(_node("man"),   QList<MNode*>() << _node("breath") << _node("water"),  MLinkType::PredicateLink(), MTruthValue(0.1));
	
	qDebug() << _node("Socrates");
	qDebug() << _node("man");
	qDebug() << _node("mortal");
	qDebug() << _node("air");
	qDebug() << _node("water");
	qDebug() << _node("breath");
	
	// Calculate probability of truthfulness that Socrates breathes air
	// First, P(socrates = man), defined Truth as 1
	// Next, P(man = breath air) = 1
	// P(SM) * P(MBA) = 1
	// However, for P(socrates breathes water):
	// P(SM) * P (MBW) = 1 * 0.1 = 0.1
	
	
	
}
