#include "MindNode.h"
#include <QUuid>

namespace MindSpace
{
/* Our static hash containers for UUID->Link types */
QHash<QString,MNodeType> MindSpace::MNodeType::s_nodeTypes;
/* Map node names to nodes */
QHash<QString,MNode*> MindSpace::MNode::s_nodes;


QDebug operator<<(QDebug dbg, const MNodeType& type)
{
	dbg.nospace() << "MNodeType(name:" << type.name() <<", uuid:" << type.uuid() << ")";
	return dbg.space();
}

QDebug operator<<(QDebug dbg, const MNode* node) 
{
	dbg.nospace() << qPrintable(MNode::toSimpleString(node));
	return dbg.space();
}

QString MNode::toSimpleString(const MNode* node, bool renderLinks)
{
	if(!node)
	{
		return "(NULL)";
	}
	else
	{
		QStringList out;
		out << QString("#%1(%2)").arg(node->content()).arg(node->type().name());
		if(renderLinks)
		{
			const QList<MLink*> & links = node->links();
			if(!links.isEmpty())
				out << "\n";
				
			foreach(MLink *link, links)
			{
				out << "\t" << link->toString(link, node) << "\n";
			}
		}
		return out.join("");
	
	}
}

QString MNode::toString(const MNode* node, bool renderLinks)
{
	if(!node)
		return "MNode(0x0)";
		
	QStringList out;
	out << "MNode(content:" << node->content() <<", type:\"" << node->type().name() <<"\", uuid:" << node->uuid() << ")";
	if(renderLinks)
	{
		const QList<MLink*> & links = node->links();
		if(!links.isEmpty())
			out << "\n";
			
		foreach(MLink *link, links)
		{
			out << "\t" << link->toString(link, node) << "\n";
		}
	}
	
	return out.join("");
}


MNode *MNode::clone(MNode *node, int levels)
{
	return node->_clone(0, levels);
}

MNode *MNode::_clone(int curLevel, int levels)
{
	if(curLevel > levels)
		return 0;
	 
	MNode *newNode = new MNode();
	newNode->fromVariantMap(node->toVariantMap());
	
	foreach(MLink *links, m_links)
	{
		if(links->node1() == this)
		{
			MNode *node2 = links->node2()->clone(curLevel+1, levels);
			newNode->addLink(node2);
		}
	}
	
	return newNode;
}

MNode *MNode::clone()
{
	return clone(this);
}

/** Create an empty MNode with type set to MindSpace::ConceptNode and LTI/STI to 1.0 each */
MNode::MNode()
	: QStorableObject()
	, m_type(MNodeType::ConceptNode())
	, m_content("")
	, m_longTermImportance(1.0)
	, m_shortTermImportance(1.0)
	, m_links()
{
	/* just to thwart gcc's warnings about MindSpace::staticMetaObject being unused */
	Q_UNUSED(MindSpace::staticMetaObject);
	
	createUuid();
	
/*	if(MSpace *mind = MSpace::activeSpace())
		mind->addNode(this);*/
}

MNode::~MNode()
{
	qDeleteAll(m_links);
/*	if(MSpace *mind = MSpace::activeSpace())
		mind->removeNode(this);*/
}

/** Creates a new MNode with the given \a content and \a type (defaults to MindSpace::ConceptNode) */
MNode::MNode( const QString& content, MindSpace::MNodeType type )
	: QStorableObject()
	, m_type(type)
	, m_content(content)
	, m_longTermImportance(1.0)
	, m_shortTermImportance(1.0)
	, m_links()
{
	createUuid();
	setContent(content);
	
/*	if(MSpace *mind = MSpace::activeSpace())
		mind->addNode(this);*/
}

/** \return the MNode* object for the given node name if the node exists. If the node does not exist, and type is NOT a null type (See MNodetype::isNull()), then it creates a new node and returns that node object. Returns 0 (null) in all other cases */
MNode *MNode::node(const QString& name, MindSpace::MNodeType type, MSpace *mspace)
{
	if(s_nodes.contains(name))
		return s_nodes.value(name);
		
	if(!type.isNull())
	{
		MNode *node = new MNode(name, type);
		
 		if(mspace)
 			mspace->addNode(node);
		
		return node;
	}
	
	return 0;
}

/** Set the MNodeType of this node to \a type. \sa type */
void MNode::setType(MNodeType type)
{
	m_type = type;
	emit nodeTypeChanged(type);
}

/** Set the content of this node to \a content. \sa content */
void MNode::setContent(const QString& content)
{
	QString oldContent = m_content;
	if(s_nodes.contains(m_content))
		s_nodes.remove(m_content);
	
	m_content = content; 
	s_nodes.insert(content, this);
	
	emit contentChanged(oldContent, content);
}

/** Set the long term importance of this node to \a imp. \sa longTermImportance */
void MNode::setLongTermImportance(double imp)
{
	m_longTermImportance = imp;
	emit importanceChanged(m_longTermImportance, m_shortTermImportance);
}

/** Set the short term importance of this node to \a imp. \sa shortTermImportance */
void MNode::setShortTermImportance(double imp)
{
	m_shortTermImportance = imp;
	emit importanceChanged(m_longTermImportance, m_shortTermImportance);
}

/** Set the list of links for this node to \a links. \sa links */
void MNode::setLinks(const QList<MLink*>& links)
{
	m_links = links;
	emit linksListChanged();
}

/** Add link \a link to the internal list of links. \a emits linkAdded */ 
void MNode::addLink(MLink *link)
{
	if(m_links.contains(link))
		return;
		
	m_links.append(link);
	emit linkAdded(link);
}

/** Remove link \a link from the internal list of links. \a emits linkRemoved */
void MNode::removeLink(MLink *link)
{
	if(m_links.removeAll(link) > 0)
		emit linkRemoved(link);
}

/** Set the 'data' property for this link to the value of \a data. emits dataChanged() */
void MNode::setData(QVariant data)
{
	m_data = data;
	emit dataChanged();
}

/** Creates a new UUID for this node only if no UUID already assigned. \sa uuid */
void MNode::createUuid()
{
	if(!m_uuid.isEmpty())
		return;
	m_uuid = QUuid::createUuid().toString();
}

// From QStorableObject, used to return storable versions of the 'links' and 'type' properties
QVariant MNode::storableProperty(QString name)
{
	if(name == "links")
		return QVariant(); // links are handled by MindSpace
	else
	if(name == "type")
		return type().toVariant();
	else
		return property(qPrintable(name));
}

void MNode::setStoredProperty(QString name, QVariant value)
{
	if(name == "links")
		return; // links are handled by MindSpace
	else
	if(name == "type")
		setType(MNodeType::fromVariant(value));
	else
	if(name == "uuid")
		// Our Q_PROPERTY defenition doesn't provide a setter for uuid (rightfully so)
		m_uuid = value.toString();
	else
		setProperty(qPrintable(name), value);
}


QList<MNode *> MNode::linkedNode(const QString& content, bool first)
{
	QList<MNode *> nodes;
	foreach(MLink *link, m_links)
	{
		MNode *node = 0;
		if(link->node1() == this &&
		   link->node2()->content() == content)
		   node = link->node2();
		else
		if(link->node2() == this &&
		   link->node1()->content() == content)
		   node = link->node1();
		
		if(node)
		{
			nodes.append(node);
			if(first)
				break;
		}
	}
	
	return nodes;
}

QList<MNode *> MNode::linkedNode(MindSpace::MNodeType type, bool first)
{
	QList<MNode *> nodes;
	foreach(MLink *link, m_links)
	{
		MNode *node = 0;
		if(link->node1() == this &&
		   link->node2()->type() == type)
		   node = link->node2();
		else
		if(link->node2() == this &&
		   link->node1()->type() == type)
		   node = link->node1();
		
		if(node)
		{
			nodes.append(node);
			if(first)
				break;
		}
	}
	
	return nodes;
}

MNode *MNode::firstLinkedNode(const QString& content)
{
	QList<MNode*> list = linkedNode(content, true);
	return list.isEmpty() ? 0 : list.first();
}

MNode *MNode::firstLinkedNode(MindSpace::MNodeType type)
{
	QList<MNode*> list = linkedNode(type, true);
	return list.isEmpty() ? 0 : list.first();
}


}; // end namespace
