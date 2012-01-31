#include "MindLink.h"
#include <QUuid>

namespace MindSpace
{

/* Our static hash containers for UUID->Link types */
QHash<QString,MLinkType> MindSpace::MLinkType::s_linkTypes;

QDebug operator<<(QDebug dbg, const MLinkType& type)
{
	dbg.nospace() << "MLinkType(name:" << type.name() <<", uuid:" << type.uuid() << ")";
	return dbg.space();
}

QDebug operator<<(QDebug dbg, const MTruthValue& value)
{
	dbg.nospace() << "MTruthValue(type:" << (value.type() == MTruthValue::SimpleTruth ? "SimpleTruth" : "RangeTruth") <<", value:" << value.value()<<", rangeA:"<<value.rangeA()<<", rangeB:"<<value.rangeB() << ")";
	return dbg.space();
}

QDebug operator<<(QDebug dbg, const MLink* link) 
{
	return dbg.space() << qPrintable(MLink::toString(link));
}

QString MLink::toString(const MLink *link, const MNode *from)
{
	if(!link)
		return "MLink(0x0)";
		
	if(from)
	{
		//type:" << type() <<", node1:" << node1() <<", node2:" << node2() << ", uuid:" << uuid() << "
		QStringList out = QStringList() << " +-- " << link->type().name();
		if(link->type().hasList())
		{
			if(link->node1() == from)
				out << " --> [ ";
			else
				out << " <-- " << MNode::toSimpleString(link->node1(), false) << " = [ ";
			QStringList sublist;
			foreach(MNode *node, link->arguments())
			 	sublist << MNode ::toSimpleString(node, false);
			 out << sublist.join(" , ") << " ]";
		}
		else
		{
			if(link->node1() == from)
				out << " --> " << MNode::toSimpleString(link->node2(), false);
			else
				out << " <-- " << MNode::toSimpleString(link->node1(), false);
		}
		
		double value = link->truthValue().value();
		if(value < 1.0)
		{
			out << " (Tv:" << QString::number(value) << ")";
		}
		
		return out.join("");
	}
	else
	{
		QStringList out = QStringList() << "type:\"" << link->type().name() <<"\", node1:" << MNode::toString(link->node1()) <<", node2:" << MNode::toString(link->node2()) << ", uuid:" << link->uuid();
		return "MLink(" + out.join("") + ")";
	}
}

/*******************/
/** Creates an empty (null) MLink with the predefined link type 'PartOfLink'. \sa isNull */
MLink::MLink()
	: QStorableObject()
	, m_node1(0)
	, m_node2(0)
	, m_linkType(MLinkType::PartOfLink())
	, m_truthValue()
{
	createUuid();
	if(MSpace *mind = MSpace::activeSpace())
		mind->addLink(this);
}


/** Creates a new MLink from \a node1 to \a node2 with the givne \a linkType and \a truth value */
MLink::MLink(MNode* node1, MNode* node2, MLinkType linkType, MTruthValue truth)
	: QStorableObject()
	, m_node1(0)
	, m_node2(0)
	, m_linkType(linkType)
	, m_truthValue(truth)
{
	createUuid();
	setNode1(node1);
	setNode2(node2);
	
/*	if(MSpace *mind = MSpace::activeSpace())
		mind->addLink(this);*/
}

/** Creates a new MLink from \a node1 to the \a argumentList with the givne \a linkType and \a truth value */
MLink::MLink(MNode* node1, QList<MNode*> argumentList, MLinkType linkType, MTruthValue truth)
	: QStorableObject()
	, m_node1(0)
	, m_node2(0)
	, m_args()
	, m_linkType(linkType)
	, m_truthValue(truth)
{
	createUuid();
	setNode1(node1);
	setArguments(argumentList);
	
/*	if(MSpace *mind = MSpace::activeSpace())
		mind->addLink(this);*/
}

MLink::~MLink()
{
	setNode1(0);
	setNode2(0);
	setArguments(QList<MNode*>());
	
/*	if(MSpace *mind = MSpace::activeSpace())
		mind->removeLink(this);*/
}

/** \return true if BOTH node1() and node2() are NULL. \sa node1, node2 */
bool MLink::isNull()
{
	if(!m_node1 && !m_node2)
		return true;
	return false;
}

/** Set the MLinkType of this link to \a type. \sa type */
void MLink::setType(MindSpace::MLinkType type) { m_linkType = type; }
/** Set the first MNode of this link to \a node1. \sa node1 */ 
void MLink::setNode1(MindSpace::MNode* node1)
{ 
	if(m_node1)
		m_node1->removeLink(this);
	
	m_node1 = node1;
	 
	if(node1)
		node1->addLink(this);
}

/** Set the second MNode of this link to \a node2. \sa node2 */
void MLink::setNode2(MindSpace::MNode* node2)
{
	if(m_node2)
		m_node2->removeLink(this);
	
	m_node2 = node2;
	
	if(node2)
		node2->addLink(this);
}

/** Set the MTruthValue of this link to \a value. \sa truthValue */
void MLink::setTruthValue(MindSpace::MTruthValue value) { m_truthValue = value; }

/** Set the list of argument nodes to \a arguments. \sa arguments */
void MLink::setArguments(QList<MNode*> arguments)
{
	foreach(MNode *node, m_args)
		node->removeLink(this);
		
	m_args = arguments;
	
	foreach(MNode *node, arguments)
		node->addLink(this);
}

/** Creates a new UUId for this link only if no UUId already assigned. \sa uuid */
void MLink::createUuid()
{
	if(!m_uuid.isEmpty())
		return;
	m_uuid = QUuid::createUuid().toString();
}

// From QStorableObject, used to return storable versions of the relevant properties (all except 'uuid' need to be sanitized by these functions)
QVariant MLink::storableProperty(QString name)
{
	if(name == "node1")
		return node1()->uuid();
	else
	if(name == "node2")
		return node2() ? node2()->uuid() : QString();
	else
	if(name == "arguments")
	{
		QVariantList list;
		foreach(MNode *node, arguments())
			list << node->uuid();
		return list;
	}
	else
	if(name == "truthValue")
		return truthValue().toVariant();
	else
	if(name == "type")
		return type().toVariant();
	else
		return property(qPrintable(name));
}

void MLink::setStoredProperty(QString name, QVariant value)
{
	//qDebug() << "MLink::setStoredProperty: "<<name<<value;
	if(name == "node1")
	{
		//qDebug() << "MLink::setStoredProperty: node1: "<<value.toString();
		setNode1(MSpace::activeSpace()->uuidToNode(value.toString()));
	}
	else
	if(name == "node2")
		setNode2(MSpace::activeSpace()->uuidToNode(value.toString()));
	else
	if(name == "arguments")
	{
		QVariantList list = value.toList();
		QList<MNode*> nodeList;
		foreach(QVariant value, list)
			nodeList << MSpace::activeSpace()->uuidToNode(value.toString());
		setArguments(nodeList);
	}
	else
	if(name == "truthValue")
		setTruthValue(MTruthValue::fromVariant(value));
	else
	if(name == "type")
		setType(MLinkType::fromVariant(value));
	else
	if(name == "uuid")
		// Our Q_PROPERTY defenition doesn't provide a setter for uuid (rightfully so)
		m_uuid = value.toString();
	else
		setProperty(qPrintable(name), value);
}


}; // end namespace
