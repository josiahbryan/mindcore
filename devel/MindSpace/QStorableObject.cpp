#include "QStorableObject.h"

#include <QDebug>
#include <QMetaProperty>

bool QStorableObject::fromByteArray(QByteArray& array)
{
	QDataStream stream(&array, QIODevice::ReadOnly);
	QVariantMap map;
	stream >> map;

	if(map.isEmpty())
	{
		qDebug() << "Error: QStorableObject::fromByteArray(): Map is empty, unable to load object.";
		return false;
	}

	return fromVariantMap(map);
}

bool QStorableObject::fromVariantMap(const QVariantMap& map, bool onlyApplyIfChanged)
{
	// So we dont have to engineer our own method of tracking
	// properties, just assume all inherited objects delcare the relevant
	// properties using Q_PROPERTY macro
	foreach(QString propName, map.keys())
	{
		QVariant value = map[propName];

		// Allow subclasses to ignore loading properties
		if(ignoreProperty(propName, true)) // true = loading, false would mean saving
			continue;

		if(value.isValid())
		{
			if(onlyApplyIfChanged)
			{
				if(storableProperty(propName) != value)
				{
					//qDebug() << "QStorableObject::loadPropsFromMap():"<<this<<": [onlyApplyIfChanged] i:"<<i<<", count:"<<count<<", prop:"<<name<<", value:"<<value;
					setStoredProperty(propName,value);
				}
			}
			else
			{
				//qDebug() << "QStorableObject::loadPropsFromMap():"<<this<<": i:"<<i<<", count:"<<count<<", prop:"<<name<<", value:"<<value<<" (calling set prop)";

				setStoredProperty(propName,value);
				//m_props[name] = value;
			}
		}
	}

	return true;
}

QByteArray QStorableObject::toByteArray()
{
	QByteArray array;
	QDataStream stream(&array, QIODevice::WriteOnly);
	stream << toVariantMap();
	return array;
}

QVariantMap QStorableObject::toVariantMap()
{
	QVariantMap map;
	
	// So we dont have to engineer our own method of tracking
	// properties, just assume all inherited objects delcare the relevant
	// properties using Q_PROPERTY macro
	const QMetaObject *metaobject = metaObject();
	int count = metaobject->propertyCount();
	for (int i=0; i<count; ++i)
	{
		QMetaProperty metaproperty = metaobject->property(i);
		const char *name = metaproperty.name();
		
		if(ignoreProperty(QString(name), false)) // true = loading, false=saving
			continue;

		QVariant value = storableProperty(name);
		//qDebug() << "QStorableObject::propsToMap():"<<(QObject*)this<<": prop:"<<name<<", value:"<<value;

		map[name] = value;
	}
	
	// Store dynamic properties as well
	QList<QByteArray> dynamicProps = dynamicPropertyNames();
	foreach(QByteArray name, dynamicProps)
	{
		if(name.startsWith("_q"))
			continue;
			
		QVariant var = storableProperty(name.data());
		// dont store userdefined types
		if(var.isValid() && (int)var.type() < (int)QVariant::UserType)
		{
			map[QString(name)] = var;
			//qDebug() << "QStorableObject::propsToMap():"<<(QObject*)this<<": dynamic prop:"<<name<<", value:"<<var;
		}
	}
	
	return map;
}
