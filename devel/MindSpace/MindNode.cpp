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
	dbg.nospace() << qPrintable(MNode::toSimpleString(node, false));
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
		out << QString("#%1(%2,%3)").arg(node->content()).arg(node->type().name()).arg(node->uuid());
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
	//qDebug() << "MNode::_clone("<<curLevel<<"/"<<levels<<"): orig node:"<<qPrintable(MNode::toString(this, true));
	if(curLevel > levels)
		return 0;
	 
	MNode *newNode = new MNode();
	newNode->fromVariantMap(toVariantMap());
	
	// Clear uuid because createUuid() won't overwrite an existing uuid
	newNode->m_uuid = "";
	newNode->createUuid();
	
	if(curLevel < levels)
	{
		//qDebug() << "MNode::_clone("<<curLevel<<"/"<<levels<<"): new node: "<<newNode<<", starting to clone "<<m_links.size()<<" links";
		
		foreach(MLink *link, m_links)
		{
			if(link->node1() == this)
			{
				//qDebug() << "MNode::_clone("<<curLevel<<"/"<<levels<<"): ** cloning link: "<<link;
			
				// Create and clone the link (type, truthvalue, etc)
				MLink *link2 = new MLink();
				link2->fromVariantMap(link->toVariantMap());
				
				// Clone the second node of the link
				MNode *node2 = link->node2()->_clone(curLevel+1, levels);
				
				// Set the node 1 & 2 on the link
				link2->setNode1(newNode);
				link2->setNode2(node2);
				
				// Add the new link to the new node
				newNode->addLink(link2);
			}
		}
	}
	
	//qDebug() << "MNode::_clone("<<curLevel<<"/"<<levels<<"): newNode:"<<qPrintable(MNode::toString(newNode, true));
	
	return newNode;
}

MNode *MNode::clone(int levels)
{
	return clone(this, levels);
}

/** Create an empty MNode with type set to MindSpace::ConceptNode and LTI/STI to 1.0 each */
MNode::MNode()
	: QStorableObject()
	, m_type(MNodeType::ConceptNode())
	, m_content("")
	, m_longTermImportance(1.0)
	, m_shortTermImportance(1.0)
	, m_links()
	, m_mspace(0)
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
	, m_mspace(0)
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
	{
		//qDebug() << "MNode::addLink(link): "<<this<<": Node already contains link: "<<link<<", not adding";
		return;
	}
		
	m_links.append(link);
	emit linkAdded(link);
	
// 	if(m_mspace)
// 	{
// 		m_mspace->addLink(link);
// 	}
// 	else
// 	{
// 		qDebug() << "MNode::addLink: "<<link<<": No m_mspace setup yet!";
// 	}
		
	
	//qDebug() << "MNode::addLink(link): "<<this<<": num links: "<<m_links.size();
}

/** Convenience function, equivelant to calling sourceNode->addLink(new MLink(sourceNode,destNode,linkType)) */
MLink *MNode::addLink(MNode *destNode, MindSpace::MLinkType linkType)
{
	//qDebug() << "MNode::addLink(node,type): "<<this<<": link -> "<<destNode;
	foreach(MLink *link, m_links)
	{
		if(link->node1() == this && 
		   link->node2() == destNode && 
		   link->type() == linkType)
		{
			return link;
		}
	}
	
	MLink *link = new MLink(this, destNode, linkType);
	//qDebug() << "MNode::addLink(node,type): "<<this<<": Adding new link, links size:"<<m_links.size();
	//addLink(link); // automatically added to both nodes by MLink() constructor
	
	if(m_mspace)
	{
		m_mspace->addLink(link);
	}
	else
	{
		qDebug() << "MNode::addLink: "<<link<<": No m_mspace setup yet!";
	}

	return link;
}

/** Remove link \a link from the internal list of links. \a emits linkRemoved */
void MNode::removeLink(MLink *link)
{
	if(m_links.removeAll(link) > 0)
	{
		emit linkRemoved(link);
			
		if(link->node1() == this)
			if(link->node2())
				link->node2()->removeLink(link);
			else
			if(link->arguments().size() > 0)
				foreach(MNode *node, link->arguments())
					node->removeLink(link);
		else
			link->node1()->removeLink(link);
		
		
	// 	if(m_mspace)
	// 	{
	// 		m_mspace->removeLink(link);
	// 	}
	// 	else
	// 	{
	// 		qDebug() << "MNode::removeLink: "<<link<<": No m_mspace setup yet!";
	// 	}
	}
		
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
	//qDebug() << "MNode::setStoredProperty: "<<name<<value;
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

MNode * MNode::linkedNodeUuid(const QString& uuid)
{
	foreach(MLink *link, m_links)
	{
		if(link->node1() == this &&
		   link->node2()->uuid() == uuid)
		   return link->node2();
		else
		if(link->node2() == this &&
		   link->node1()->uuid() == uuid)
		   return link->node1();
	}
	
	return 0;
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

void MNode::setMindSpace(MSpace *ms)
{
	m_mspace = ms;
	//qDebug() << "MNode::setMindSpace: "<<this<<" using ptr: "<<ms;
}


MNode *MNode::linkedNode(const QString& content, MindSpace::MNodeType type, MindSpace::MLinkType linkType, QVariant initialData)
{
	MNode *node = firstLinkedNode(content);
	if(!node)
	{
		//qDebug() << "MNode::linkedNode: "<<this<<" -> "<<content<<": creating node and link";
		node = new MNode(content, type);
		
		if(initialData.isValid())
			node->setData(initialData);
		
		MLink *link = new MLink(this, node, linkType);
		
		if(m_mspace)
		{
			m_mspace->addNode(node);
			m_mspace->addLink(link);
		}
		else
		{
			qDebug() << "MNode::linkedNode: "<<this<<" -> "<<content<<": No m_mspace setup yet!";
		}
		
		// Link will call setNode() which calls node->addLink(), which calls m_mspace->addLink()
		//MLink *link = new MLink(this, node, linkType);
	}
	
	return node;
}

QList<MLink*> MNode::outgoingLinks() const
{
	QList<MLink*> out;
	foreach(MLink *link, m_links)
		if(link->node1() == this)
			out.append(link);
	//qDebug() << "MNode::outgoingLinks(): "<<this<<" num raw links:"<<m_links.size()<<", num out links:"<<out.size();
	return out;
}

QList<MLink*> MNode::incomingLinks() const
{
	QList<MLink*> in;
	foreach(MLink *link, m_links)
		if(link->node2() == this)
			in.append(link);
	return in;
}

}; // end namespace
