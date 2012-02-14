#ifndef MindSpaceNode_H
#define MindSpaceNode_H

#include "MindSpace.h"
#include "MindNodeType.h"
#include "MindLinkType.h"

namespace MindSpace 
{
	class MLink;
	class MSpace;
	
	
	/*! \brief A MNode represents a single node in MindSpace. 
	 	Nodes are joined together by MLink objects 
	 */ 	
	 
	class MNode : public QStorableObject 
	{
		Q_OBJECT
		
		/** The UUID of this MNode */
		Q_PROPERTY(QString uuid READ uuid);
		
		/** The MNodeType of this node */
		Q_PROPERTY(MNodeType type READ type WRITE setType);
		
		/** The Content of this node */ 
		Q_PROPERTY(QString content READ content WRITE setContent);
		
		/** The long term importance of this node */
		Q_PROPERTY(double longTermImportance READ longTermImportance WRITE setLongTermImportance);
		
		/** The short term importance of this node */
		Q_PROPERTY(double shortTermImportance READ shortTermImportance WRITE setShortTermImportance);
		
		/** The list of links from and to this MNode */
		Q_PROPERTY(QList<MLink*> links READ links WRITE setLinks);
		
		/** The data attribute for this MNode */
		Q_PROPERTY(QVariant data READ data WRITE setData);
		
		
	public:
		MNode();
		MNode( const QString& content, MindSpace::MNodeType type = MNodeType::ConceptNode() );
		virtual ~MNode();
		
		QString uuid() const			{ return m_uuid; }			/*!< \return the UUID of this node. 			\sa uuid */
		MNodeType type() const			{ return m_type; }			/*!< \return the MNodeType of this node. 		\sa type */
		QString content() const			{ return m_content; }			/*!< \return the content of this node. 			\sa content */
		
		double longTermImportance() const	{ return m_longTermImportance; }	/*!< \return the Long Term Importance of this node.	\sa longTermImportance */
		double shortTermImportance() const	{ return m_shortTermImportance; }	/*!< \return the short term importance of this node. 	\sa shortTermImportance */
		
		/* const so that they don't modify our list externally - TODO should they be able to? */
		const QList<MLink*> & links() const 	{ return m_links; }			/*!< \return the links pointing from and to this node.	\sa links */
		QList<MLink*> outgoingLinks() const;						/*!< \return only the links outgoing from this node.	\sa links */
		QList<MLink*> incomingLinks() const;						/*!< \return only the links incoming to this node.	\sa links */
		
		const QVariant & data() const		{ return m_data; }			/*!< \return the 'data' property variant.		\sa data */
		
		QList<MNode *> linkedNode(const QString& content,    bool first=false);		/*!< \return all linked nodes matching \a content - return only the first matching node if \a first is true. */
		QList<MNode *> linkedNode(MindSpace::MNodeType type, bool first=false);		/*!< \return all linked nodes matching \a type - return only the first matching node if \a first is true. */
		
		MNode *linkedNode(const QString& content, MindSpace::MNodeType type, MindSpace::MLinkType linkType=MLinkType::PartOf(), QVariant initialData=QVariant()); /*!< \return a linked MNode matching content/type - it finds or creates the node if it doesn't exist */
		
		MNode *firstLinkedNode(const QString& content);					/*!< \return the first linked node matching \a content - convenience function, calls linkedNode(content,true) internally */ 
		MNode *firstLinkedNode(MindSpace::MNodeType type);				/*!< \return the first linked node matching \a type - convenience function, calls linkedNode(content,true) internally */
		
		MNode *linkedNodeUuid(const QString& uuid);					/*!< \return the node linked to this node matching \a uuid - returns NULL if not found */
		
		MSpace *mindSpace() { return m_mspace; }					/*!< \return the MSpace this node belongs to */
		
		MNode *clone(int levels=1);							/*!< clone this node using toVariantMap()/fromVariantMap(), \return the new node */
		static MNode* clone(MNode *node, int levels=1);					/*!< \return a cloned copy of the \a node */ 
		
		static QString toString(const MNode* node=0, bool renderLinks = true);		/*!< \return a string suitable for textual output for debugging purposes */
		static QString toSimpleString(const MNode* node=0, bool renderLinks = true);	/*!< \return a short format string suitable for textual output for debugging purposes */
		
		static MNode *node(const QString& name, MindSpace::MNodeType type = MNodeType(), MSpace *mspace=0);
		
		static QHash<QString,MNode*> nodes() { return s_nodes; }
		
		// From QStorableObject, used to return storable versions of the 'links' and 'type' properties
		virtual QVariant storableProperty(QString name); 
		virtual void setStoredProperty(QString name, QVariant value);
		
	public slots:
		void setType(MNodeType type);
		void setContent(const QString& content);
		void setLongTermImportance(double imp);
		void setShortTermImportance(double imp);
		
		void setLinks(const QList<MLink*>& links);
		void addLink(MLink *link);
		void removeLink(MLink *link);
		MLink *addLink(MNode *, MindSpace::MLinkType linkType=MLinkType::PartOf());
		
		void setData(QVariant);
	
	signals:
		/** Emitted when a link is added to this node */
		void linkAdded(MLink *link);
		
		/** Emitted when a link is removed from this node */
		void linkRemoved(MLink *link);
		
		/// Node type has changed to \a type
		void nodeTypeChanged(MNodeType type);
		
		/// Content changed from \a oldContent to \a newContent
		void contentChanged(QString oldContent, QString newContent);
		
		/// Importance values changed
		void importanceChanged(double longTerm, double shortTerm);
		
		/// setLinks() was called
		void linksListChanged();
		
		/// The 'data' property was changed
		void dataChanged();
		
	protected:
		friend class MSpace;
		void setMindSpace(MSpace *ms);// { m_mspace=ms; }
		
	protected:
		/** \brief Creates a new UUID for this node
		  */
		void createUuid();
		
		/// The UUID of this node
		QString m_uuid;
		
		/// The MNodeType of this node
		MNodeType m_type;
		
		/// The content of this node
		QString m_content;
		
		/// The long term importance of this node
		double m_longTermImportance;
		
		/// The short term importance of this node
		double m_shortTermImportance;
		
		/// The links from and to this node 
		QList<MLink*> m_links;
		
		/// The data attribute for this node
		QVariant m_data;
		
		/// MSpace object to which this has been added
		MSpace *m_mspace;
		
		/// Static map of Content to MNode objects
		static QHash<QString,MNode*> s_nodes;
		
		
		MNode *_clone(int curLeve=0, int levels=1);
	
	};
	
	/** Shortcut for MNode::node(...) **/
	static MNode *_node(const QString& name, MindSpace::MNodeType type = MNodeType(), MSpace *mspace=0 ) { return MNode::node(name, type, mspace); }
	
	QDebug operator<<(QDebug dbg, const MNode* node);
	
};

#endif
