#ifndef QStorableObject_H
#define QStorableObject_H

#include <QObject>
#include <QByteArray>
#include <QVariant>

class QStorableObject : public QObject
{
	Q_OBJECT
public:
	virtual bool fromByteArray(QByteArray&);
	virtual QByteArray toByteArray();
	
	virtual bool ignoreProperty(const QString& /*propertyName*/, bool /*isLoading*/) { return false; }
	
	virtual bool fromVariantMap(const QVariantMap&, bool onlyApplyIfChanged = false);
	virtual QVariantMap toVariantMap();
	
	/// Override storableProperty() and setStoredProperty() to return storable versions of the specified property
	virtual QVariant storableProperty(QString name) 
		{ return property(qPrintable(name)); }
	virtual void setStoredProperty(QString name, QVariant value) 
		{ setProperty(qPrintable(name), value); }
};
	
#endif
