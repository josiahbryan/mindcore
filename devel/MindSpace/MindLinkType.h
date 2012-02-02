#ifndef MindLinkType_H
#define MindLinkType_H

namespace MindSpace
{

/*! \brief Object representing the type of a MLink 
	 */
	class MLinkType
	{
	public:
		/** Creates a NULL link type object */
		MLinkType() : m_hasList(false) {}
		/** Creates a new MLinkType with the given \a uuid and \a name and optional \a parent link type*/
		MLinkType(const QString& name, const QString& uuid, bool hasList = false, MLinkType parent = MLinkType())
			: m_uuid(uuid)
			, m_name(name)
			, m_hasList(hasList)
		{
			s_linkTypes.insert(name, *this);
			s_linkTypes.insert(uuid, *this);
			m_parents << parent;
		}
		
		/** Creates a new MLinkType with the given \a uuid and \a name and a list of \a parents link types*/
		MLinkType(const QString& name, const QString& uuid, bool hasList, QList<MLinkType> parents)
			: m_uuid(uuid)
			, m_name(name)
			, m_hasList(hasList)
		{
			s_linkTypes.insert(name, *this);
			s_linkTypes.insert(uuid, *this);
			m_parents = parents;
		}
		MLinkType(const QString& name, const QString& uuid, bool hasList, QStringList parents)
			: m_uuid(uuid)
			, m_name(name)
			, m_hasList(hasList)
		{
			s_linkTypes.insert(name, *this);
			s_linkTypes.insert(uuid, *this);
			
			foreach(QString parent, parents)
				if(s_linkTypes.contains(parent))
					m_parents.append(s_linkTypes.value(parent));
			
		}
		/** Removes this link type from the static map of link types */
		~MLinkType()
		{
			//s_linkTypes.remove(m_uuid); 
		}
		
		/** \return the first parent MLinkType, if set, or a null (see isNull()) MLinkType if no parent set. */
		MLinkType parent() { return m_parents.isEmpty() ? MLinkType() : m_parents.first(); }
		
		/** \return true if this MLinkType inherits from \a type (or any of its parents inherit from \a type) - false otherwise */
		bool isa(const MLinkType& type)
		{
			if(m_parents.isEmpty())
				return false;
			foreach(MLinkType parent, m_parents)
			{
				if(parent == type)
					return true;
				if(parent.isa(type))
					return true;
			}
			return false;
		}
		
		/** \return true if this link type is null (no UUID) */
		bool isNull() { return m_uuid.isEmpty(); }
		
		/** \return the UUID if the link */
		QString uuid() const { return m_uuid; }
		/** \return the Name of the link */
		QString name() const { return m_name; }
		
		/** \return a boolean flag indicating if this type of link as a list of arguments */
		bool hasList() const { return m_hasList; }
		
		/** \return A MLinkType object of the given \a uuid if it exists. (\a uuid may also be a name() of a MLinkType)
		 * If the \a uuid does not exist, and \a defineIfNotExists is empty, then a null link type is returned. 
		 * If the \a uuid does not exist, and \a defineIfNotExists is NOT empty, then a new MLinkType object with the given \a uuid and \a defineIfNotExists as the name is returned.
		 */
		static MLinkType findLinkType(const QString& uuid, const QString & defineIfNotExists="")
		{
			if(s_linkTypes.contains(uuid))
				return s_linkTypes.value(uuid);
			
			if(defineIfNotExists.isEmpty())
				return MLinkType();
				
			return MLinkType(uuid,defineIfNotExists);
		}
		
		QVariant toVariant() const { return m_uuid; }
		static MLinkType fromVariant(QVariant v) { return findLinkType(v.toString()); }
		
		#include "mindspace-types.link.prototypes"
	
		/*! \return true if \a a has the same uuid() as \a b */
		inline bool operator==(const MLinkType &b)
		{
			return uuid() == b.uuid();
		}
		
		
	private:
		/** UUID of the link */
		QString m_uuid;
		
		/** Name of the link */
		QString m_name;
		
		/** Bollean flag indicating if this type of link uses lists */
		bool m_hasList;
		
		/** Parent link type, if any */
		QList<MLinkType> m_parents;
		
		/** Static map of UUIDs to MLinkType objects */
		static QHash<QString,MLinkType> s_linkTypes;
	};
	
	QDebug operator<<(QDebug dbg, const MLinkType& type);
	
};

#endif
