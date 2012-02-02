#ifndef MindSpaceLink_H
#define MindSpaceLink_H

#include "MindSpace.h"
#include "MindLinkType.h"

namespace MindSpace 
{
	class MNode;
	
	/*! \brief Truthfulness assertion for a link
	 */
	class MTruthValue
	{
	public:
		/** Creates a SimpleTruth value of 1.0 */
		MTruthValue()
			: m_type(SimpleTruth)
			, m_value(1.0)
			, m_rangeA(0.0)
			, m_rangeB(0.0)
		{}
		
		/** Creates a SimpleTruth value of \a value */
		MTruthValue(double value)
			: m_type(SimpleTruth)
			, m_value(value)
			, m_rangeA(0.0)
			, m_rangeB(0.0)
		{}
		
		/** Creates a RangeTruth value representing truthfulness between \a rangeA and \a rangeB */ 
		MTruthValue(double rangeA, double rangeB)
			: m_type(RangeTruth)
			, m_value(0.0)
			, m_rangeA(rangeA)
			, m_rangeB(rangeB)
		{}
		
		/** \brief TruthType The types of Truthfulness represented by this value */
		enum TruthType {
			SimpleTruth = 1, /*!< Simple probability value between 0.0 and 1.0, inclusive */
			RangeTruth,      /*!< A probability range between two values */
		};
		
		/** \return the TruthType of this value */
		const TruthType & type() const { return m_type;   }
		
		/** \return the SimpleTruth value */
		double value() const   { return m_value;  }
		
		/** \return the start value for a RangeTruth */
		double rangeA() const  { return m_rangeA; }
		
		/** \return the end value for a RangeTruth */
		double rangeB() const  { return m_rangeB; }
		
		QDebug operator<<(QDebug dbg);
		
		QVariant toVariant() const { return QVariantList() << (int)m_type << m_value << m_rangeA << m_rangeB; }
		static MTruthValue fromVariant(QVariant v) 
		{
			QVariantList list = v.toList();
			
			MTruthValue t;
			t.m_type   = (TruthType)list[0].toInt();
			t.m_value  = list[1].toDouble();
			t.m_rangeA = list[2].toDouble();
			t.m_rangeB = list[3].toDouble();
			
			return t;
		}
		
		
	private:
		TruthType m_type;
		double m_value;
		double m_rangeA;
		double m_rangeB;
	};
	
	//Q_DECLARE_METATYPE(MindSpace::MTruthValue);
	
	
	//Q_DECLARE_METATYPE(MindSpace::MLinkType);
	
	/*! \brief A link between nodes
	 * A link between two MindSpace::MNode node objects */
	class MLink : public QStorableObject
	{
		Q_OBJECT
	
		/** The UUID of this link */
		Q_PROPERTY(QString uuid READ uuid);
		/** The MLinkType of this link */
		Q_PROPERTY(MLinkType type READ type WRITE setType);
		/** The first MNode in the link */
		Q_PROPERTY(MNode* node1 READ node1 WRITE setNode1);
		/** The second MNode in the link */
		Q_PROPERTY(MNode* node2 READ node2 WRITE setNode2);
		/** A list of outgoing or argument nodes */
		Q_PROPERTY(QList<MindSpace::MNode*> arguments READ arguments WRITE setArguments);
		//Q_PROPERTY(MindSpace::MLink::LinkDirection linkDirection READ linkDirection WRITE setLinkDirection);
		/** The MTruthValue representing the truthfulness assertion of this link */
		Q_PROPERTY(MTruthValue truthValue READ truthValue WRITE setTruthValue);
		
	public:
		MLink();
		
		MLink(MNode* node1, MNode* node2, 
			MLinkType linkType = MLinkType::PartOfLink(), MTruthValue truth = MTruthValue());
		
		MLink(MNode* node1, QList<MNode*> argumentList, 
			MLinkType linkType = MLinkType::PartOfLink(), MTruthValue truth = MTruthValue());
		
		virtual ~MLink();
		
		bool isNull();
		bool isValid();
		
		QString uuid() const { return m_uuid; }						/*!< \return the UUID of the link. \sa uuid */
		const MindSpace::MLinkType& type() const { return m_linkType; }			/*!< \return the MLinkType of the link. \sa type */
		MindSpace::MNode* node1() const { return m_node1; }				/*!< \return the first MNode in the link. \sa node1 */
		MindSpace::MNode* node2() const { return m_node2; }				/*!< \return the second MNode in the link. \sa node2 */
		QList<MindSpace::MNode*> arguments() const { return m_args; }			/*!< \return the list of argument nodes. \sa arguments */
		const MindSpace::MTruthValue& truthValue() const { return m_truthValue; }	/*!< \return the MTruthValue of the link. \sa truthValue */
		
		static QString toString(const MLink *link, const MNode* from=0);
		
		// From QStorableObject, used to return storable versions of the relevant properties (all except 'uuid' need to be sanitized by these functions)
		virtual QVariant storableProperty(QString name); 
		virtual void setStoredProperty(QString name, QVariant value);
		
	public slots:
		void setType(MindSpace::MLinkType type);
		void setNode1(MindSpace::MNode* node1);
		void setNode2(MindSpace::MNode* node2);
		void setTruthValue(MindSpace::MTruthValue value);
		void setArguments(QList<MNode*> arguments);
		
	signals:
		void truthValueChanged(MTruthValue);
		//void node1Changed(MNode *oldValue, MNode *newValue);
		//void node2Changed(MNode *oldValue, MNode *newValue);
		//void argumentsChanged(QList<MNode*> oldList, QList<MNode*> newList);
	
	protected:
		/** Creates a new UUID for this link */
		void createUuid();
		
		/*! The UUID of this link */
		QString m_uuid;
		
		/*! First part of the link */
		MindSpace::MNode* m_node1;
		
		/*! Second part of the link */
		MindSpace::MNode* m_node2;
		
		/*! Arguments for the link */
		QList<MindSpace::MNode*> m_args;
		
		/*! Type of link this link represents (such as an IS-A link, IMPLIES link, etc */
		MindSpace::MLinkType m_linkType;
		
		/*! An assertion that this link is true */
		MindSpace::MTruthValue m_truthValue;
	};
	
	QDebug operator<<(QDebug dbg, const MLink* type);

};

#endif
