#ifndef MindSpaceNodeType_H
#define MindSpaceNodeType_H

#include <QString>

namespace MindSpace 
{

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
	
};
#endif
