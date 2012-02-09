#include "MindSpace.h"

#include <QUuid>
#include <QStringList>
#include <QFile>

namespace MindSpace
{
 
/* Our predefined types */
#define DefineNodeType(name, uuid) \
	static MNodeType MNodeType_##name ( #name, uuid); \
	const MNodeType & MNodeType:: name() { return MNodeType_##name; }

#define DefineLinkType(name, uuid) \
	static MLinkType MLinkType_##name ( #name, uuid); \
	const MLinkType & MLinkType:: name() { return MLinkType_##name; }

#define DefineLinkTypeList(name, uuid) \
	static MLinkType MLinkType_##name ( #name, uuid, true); \
	const MLinkType & MLinkType:: name() { return MLinkType_##name; }

// Generated by mindspace-types.pl from MindSpace.types
#include "mindspace-types.definitions"

MSpace *MSpace::s_activeSpace = 0;

MSpace::MSpace()
{
	s_activeSpace = this;
}

/** Destroy the MSpace **/
MSpace::~MSpace() 
{
	qDeleteAll(m_nodes);
	qDeleteAll(m_links);
	
	m_nodes.clear();
	m_links.clear();
	
	if(s_activeSpace == this)
		s_activeSpace = 0;
}

void MSpace::makeActive()
{
	s_activeSpace = this;
}

void MSpace::addNode(MNode *node)
{
	if(m_nodes.contains(node))
		return;
	
	node->setMindSpace(this);
	
	m_nodes << node;
	m_uuidToNode[node->uuid()] = node;
	m_contentToNode[node->content()] = node;
	
	connect(node, SIGNAL(contentChanged(QString,QString)),  this, SLOT(nodeContentChanged(QString,QString)));
	
	connect(node, SIGNAL(contentChanged(QString,QString)),  this, SLOT(nodeUpdated()));
	connect(node, SIGNAL(importanceChanged(double,double)), this, SLOT(nodeUpdated()));
	connect(node, SIGNAL(nodeTypeChanged(MNodeType)),       this, SLOT(nodeUpdated()));
	
	
	emit nodeAdded(node);
}

MNode *MSpace::addNode(const QString& content, MNodeType type)
{
	MNode *node = new MNode(content,type);
	addNode(node);
	return node;
}

MNode *MSpace::node(const QString& name, MNodeType type)
{
	if(m_contentToNode.contains(name))
		return m_contentToNode[name];
	
	if(!type.isNull())
		return addNode(name,type);
		
	return 0;
}

void MSpace::nodeContentChanged(QString ov, QString nv)
{
	MNode *node = dynamic_cast<MNode*>(sender());
	if(!node)
		return;
	
	m_contentToNode.remove(ov);
	m_contentToNode[nv] = node;
}


void MSpace::nodeUpdated()
{
	MNode *node = dynamic_cast<MNode*>(sender());
	if(!node)
		return;
	
	emit nodeUpdated(node);
}

void MSpace::removeNode(MNode *node)
{
	if(!m_nodes.contains(node))
		return;
	
	node->setMindSpace(0);
	
	m_nodes.removeAll(node);
	m_uuidToNode.remove(node->uuid());
	
	disconnect(node, 0, this, 0);
	
	emit nodeRemoved(node);
}

void MSpace::addLink(MLink *link)
{
	if(m_links.contains(link))
		return;
	
	m_links << link;
	m_uuidToLink[link->uuid()] = link;
	
	connect(link, SIGNAL(truthValueChanged(MTruthValue)), this, SLOT(linkUpdated()));
	
	emit linkAdded(link);
}

void MSpace::linkUpdated()
{
	MLink *link = dynamic_cast<MLink*>(sender());
	if(!link)
		return;
	
	emit linkUpdated(link);
}

MLink *MSpace::addLink(MNode *node1, MNode *node2, MLinkType type)
{
	MLink *link = new MLink(node1, node2, type);
	addLink(link);
	return link;
}

MLink *MSpace::link(MNode *node1, MNode *node2, MLinkType type)
{
	if(!node1 || !node2)
		return 0;
		
	const QList<MLink *> & links = node1->links();
	foreach(MLink *link, links)
		if(link->node2() == node2)
			return link;
	
	if(!type.isNull())
		return addLink(node1, node2, type);
	
	return 0;
}

void MSpace::removeLink(MLink *link)
{
	if(!m_links.contains(link))
		return;
	
	m_links.removeAll(link);
	m_uuidToLink.remove(link->uuid());
	
	disconnect(link, 0, this, 0);
	
	emit linkRemoved(link);
}


bool MSpace::fromVariantMap(const QVariantMap& map, bool onlyApplyIfChanged)
{
	bool flag = QStorableObject::fromVariantMap(map, onlyApplyIfChanged);
	
	QVariantList nodes = map["nodes"].toList();
	QVariantList links = map["links"].toList();
	
	foreach(QVariant data, nodes)
	{
		QVariantMap map = data.toMap();
		MNode *node = new MNode();
		node->fromVariantMap(map);
		//qDebug() << "MSpace::fromVariantMap: Loaded node:" <<node;
		addNode(node);
	}

	foreach(QVariant data, links)
	{
		QVariantMap map = data.toMap();
		MLink *link= new MLink();
		link->fromVariantMap(map);
		//qDebug() << "MSpace::fromVariantMap: Loaded link:" <<link;
		addLink(link);
	}
	
	return flag;
}

QVariantMap MSpace::toVariantMap()
{
	QVariantMap map = QStorableObject::toVariantMap();
		
	QVariantList nodes;
	foreach(MNode *node, m_nodes)
		nodes << node->toVariantMap();
	map["nodes"] = nodes;
	
	QVariantList links;
	foreach(MLink *link, m_links)
		links << link->toVariantMap();
	map["links"] = links;
	
	return map;
}

		
bool MSpace::importConceptNet2File(const QString& filename, double freqFactor, bool verbose)
{
	// my ($linktype, $arg1, $arg2, $freq, $infer) = ($line =~ /^\(([^\s]+)\s+("[^"]*")\s+("[^"]*")\s+"f=(\d+);i=(\d+);"\)/);
	// \(([^\s]+)\s+("[^"]*")\s+("[^"]*")\s+"f=(\d+);i=(\d+);"\)
	
	QRegExp rx("\\(([^\\s]+)\\s+\"([^\"]*)\"\\s+\"([^\"]*)\"\\s+\"f=(\\d+);i=(\\d+);\"\\)");
	
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "MSpace::importConceptNet2File(): Error opening "<<filename<<".";
		return false;
	}
	
	int maxFreq = 0;
	int maxInfer = 0;
	int freqSum = 0;
	int inferSum = 0;
	
	int counter = 0;
	while (!file.atEnd()) 
	{
		QByteArray lineBytes = file.readLine();
		QString line(lineBytes);
		
		/*int idx = */rx.indexIn(line);
		QStringList values = rx.capturedTexts();
		
		QString linkType = values[1];
		QString arg1	= values[2];
		QString arg2	= values[3];
		int freq	= values[4].toInt();
		int infer	= values[5].toInt(); // not used yet...
		
		if(verbose)
		{
			if(freq > maxFreq)
				maxFreq = freq;
			if(infer > maxInfer)
				maxInfer = infer;
			
			freqSum += freq;
			inferSum += infer;
			
			qDebug() << ++counter << "Loading: "<<arg1<<" -> "<<linkType<<" -> "<<arg2;
		}
		
		MNode *node1 = node(arg1, MNodeType::ConceptNode());
		MNode *node2 = node(arg2, MNodeType::ConceptNode());
			
		addLink(new MLink(node1, node2, MLinkType::findLinkType(linkType), MTruthValue(((double)freq) / freqFactor)));
			
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
	
	
	if(verbose)
	{
		int avgFreq = freqSum / counter;
		int avgInfer = inferSum / counter;
		qDebug() << "Debug: avgFreq:"<<avgFreq<<", maxFreq:"<<maxFreq<<", avgInfer:"<<avgInfer<<", maxInfer:"<<maxInfer;
	}
	
	return true;
}
		
bool MSpace::loadFromFile(const QString& filename)
{
	QFile dataFile(filename);
	if (!dataFile.open(QIODevice::ReadOnly))
	{
		qDebug() << "MSpace::loadFromFile(): Error opening "<<filename<<"."; 
		return false;
	}
	
	QByteArray array = dataFile.readAll();
	fromByteArray(array);
	dataFile.close();
	
	return true;
}

bool MSpace::writeToFile(const QString& filename)
{
	QFile dataFile(filename);
	if(!dataFile.open(QIODevice::WriteOnly))
	{
		qDebug() << "MSpace::writeToFile(): Error opening "<<filename<<".";
		return false;
	}
		
	dataFile.write(toByteArray());
	dataFile.close();
	
	return true;
}

}; /* namespace MindSpace */

