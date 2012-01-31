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
}

MNode::~MNode()
{
	qDeleteAll(m_links);
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
}

/** \return the MNode* object for the given node name if the node exists. If the node does not exist, and type is NOT a null type (See MNodetype::isNull()), then it creates a new node and returns that node object. Returns 0 (null) in all other cases */
MNode *MNode::node(const QString& name, MindSpace::MNodeType type)
{
	if(s_nodes.contains(name))
		return s_nodes.value(name);
	if(!type.isNull())
		return new MNode(name, type);
	return 0;
}

/** Set the MNodeType of this node to \a type. \sa type */
void MNode::setType(MNodeType type) { m_type = type; }

/** Set the content of this node to \a content. \sa content */
void MNode::setContent(const QString& content)
{
	if(s_nodes.contains(m_content))
		s_nodes.remove(m_content);
	
	m_content = content; 
	s_nodes.insert(content, this);
}

/** Set the long term importance of this node to \a imp. \sa longTermImportance */
void MNode::setLongTermImportance(double imp) { m_longTermImportance = imp; }

/** Set the short term importance of this node to \a imp. \sa shortTermImportance */
void MNode::setShortTermImportance(double imp) { m_shortTermImportance = imp; }

/** Set the list of links for this node to \a links. \sa links */
void MNode::setLinks(const QList<MLink*>& links) { m_links = links; }

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
		return QVariant();
	else
	if(name == "type")
		return type().toVariant();
	else
		return property(qPrintable(name));
}

void MNode::setStoredProperty(QString name, QVariant value)
{
	if(name == "links")
		return;
	else
	if(name == "type")
		setType(MNodeType::fromVariant(value));
	else
		setProperty(qPrintable(name), value);
}



}; // end namespace
