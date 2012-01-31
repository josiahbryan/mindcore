#ifndef MindSpaceNode_H
#define MindSpaceNode_H

#include "MindSpace.h"

namespace MindSpace 
{
	class MLink;
	
	/** \brief Object representing the type of a node */
	
	class MNodeType 
	{
	public:
		/** Creates a NULL node type */
		MNodeType() {}
		/** Creates a new MNodeType object with the given \a uuid and \a name */
		MNodeType(const QString& name, const QString& uuid)
			: m_uuid(uuid)
			, m_name(name)
		{
			s_nodeTypes.insert(uuid, *this);
		}
		/** Removes this node type from the static map of node types */
		~MNodeType()
		{
			//s_nodeTypes.remove(m_uuid); 
		}
		
		/** \return true if this node type is null (no UUID) */
		bool isNull() { return m_uuid.isEmpty(); }
		
		/** \return the UUID if the node type */
		QString uuid() const { return m_uuid; }
		/** \return the Name of the node type */
		QString name() const { return m_name; }
		
		/** \return A MNodeType object of the given \a uuid if it exists.
		 * If the \a uuid does not exist, and \a defineIfNotExists is empty, then a null link type is returned. 
		 * If the \a uuid does not exist, and \a defineIfNotExists is NOT empty, then a new MNodeType object with the given \a uuid and \a defineIfNotExists as the name is returned.
		 */
		static MNodeType findNodeType(const QString& uuid, const QString & defineIfNotExists="")
		{
			if(s_nodeTypes.contains(uuid))
				return s_nodeTypes.value(uuid);
			
			if(defineIfNotExists.isEmpty())
				return MNodeType();
				
			return MNodeType(uuid,defineIfNotExists);
		}
		
		QVariant toVariant() const { return m_uuid; }
		static MNodeType fromVariant(QVariant v) { return findNodeType(v.toString()); }
		
		#include "mindspace-types.node.prototypes"
		
		/** \return true if \a a has the same UUID as \a b */
		inline bool operator==(const MNodeType &b)
		{
			return uuid() == b.uuid();
		}
	
	private:
		/** UUID of the type */
		QString m_uuid;
		/** Name of the type */
		QString m_name;
		
		/** Static map of UUIDs to MNodeType objects */
		static QHash<QString,MNodeType> s_nodeTypes;
	};
	
	
	QDebug operator<<(QDebug dbg, const MNodeType& type);
	
	//Q_DECLARE_METATYPE(MindSpace::MNodeType);
	
	
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
		
		static QString toString(const MNode* node=0, bool renderLinks = true);
		static QString toSimpleString(const MNode* node=0, bool renderLinks = true);
		
		static MNode *node(const QString& name, MindSpace::MNodeType type = MNodeType());
		
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
	
	signals:
		/** Emitted when a link is added to this node */
		void linkAdded(MLink *link);
		
		/** Emitted when a link is removed from this node */
		void linkRemoved(MLink *link);
		
	protected:
		/** \brief Creates a new UUID for this node
		  */
		void createUuid();
		
		/** The UUID of this node */
		QString m_uuid;
		/** The MNodeType of this node */
		MNodeType m_type;
		/** The content of this node */
		QString m_content;
		/** The long term importance of this node */
		double m_longTermImportance;
		/** The short term importance of this node */
		double m_shortTermImportance;
		/** The links from and to this node */ 
		QList<MLink*> m_links;
		
		/** Static map of Content to MNode objects */
		static QHash<QString,MNode*> s_nodes;
	
	};
	
	/** Shortcut for MNode::node(...) **/
	static MNode *_node(const QString& name, MindSpace::MNodeType type = MNodeType()) { return MNode::node(name, type); }
	
	QDebug operator<<(QDebug dbg, const MNode* node);
	
};

#endif
