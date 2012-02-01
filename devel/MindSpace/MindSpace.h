#ifndef MindSpace_H
#define MindSpace_H

#include <QString>
#include <QObject>
#include <QHash>
#include <QDebug>
#include <QStringList>

#include "QStorableObject.h"

#include "MindNode.h"
#include "MindLink.h"

/** \brief MindSpace namespace contains the MLink, MNode and other auxiliary classes for MindSpace. 
  */

namespace MindSpace 
{
	/* moc_MindSpace.cpp falls apart if this isn't defined ...*/
	static QMetaObject staticMetaObject;
	/* ...but it isn't used in any valid context that I know of...*/
	
	class MLink;
	class MNode;
	
	/** \brief Manages all nodes and links in MindSpace */
	class MSpace : public QStorableObject
	{
		Q_OBJECT
	public:
		MSpace();
		~MSpace();
		
		const QList<MNode*> & nodes() const { return m_nodes; }
		const QList<MLink*> & links() const { return m_links; }
		
		static MSpace *activeSpace() { return s_activeSpace; }
		
		MNode *uuidToNode(const QString& uuid) { return m_uuidToNode[uuid]; }
		MLink *uuidToLink(const QString& uuid) { return m_uuidToLink[uuid]; }
		
		virtual bool fromVariantMap(const QVariantMap&, bool onlyApplyIfChanged = false);
		virtual QVariantMap toVariantMap();
		
		bool loadFromFile(const QString&);
		bool writeToFile(const QString&);
		
		bool importConceptNet2File(const QString& file="/opt/mindcore/data/src/conceptnet2/predicates_concise_nonkline.txt", double freqConversionFactor=5., bool verbose=false);
	
 	public slots:
		void makeActive();
		
		void addNode(MNode*);
		void removeNode(MNode*);
		
		void addLink(MLink*);
		void removeLink(MLink*);
	
	signals:
		void linkAdded(MLink*);
		void linkRemoved(MLink*);
		
		void nodeAdded(MNode*);
		void nodeRemoved(MNode*);
	
	private:
		QList<MNode*> m_nodes;
		QList<MLink*> m_links;
		QHash<QString,MNode*> m_uuidToNode;
		QHash<QString,MLink*> m_uuidToLink;
		
		static MSpace *s_activeSpace;
	
	};
	
};

#endif
