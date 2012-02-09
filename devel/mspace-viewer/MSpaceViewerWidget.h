#ifndef MSpaceViewerWidget_H
#define MSpaceViewerWidget_H

#include <QtGui>

#include "MindSpace.h"
using namespace MindSpace;
class MindSpaceGraphWidget;

class MSpaceViewerWidget : public QWidget
{
	Q_OBJECT
public:
	MSpaceViewerWidget();
	~MSpaceViewerWidget() {}
	
	MindSpaceGraphWidget *graphWidget() { return m_gw; }

public slots:
	void search(const QString&, bool addToHistory=true);
	
	void loadFile(QString);
	void loadConceptNetDemo();
	
	void setMindSpace(MSpace *, QList<MNodeType> typeFilter = QList<MNodeType>());
	void mapNode(MNode *node, int levels=2);
	
private slots:
	void nodeDoubleClicked(MNode*);
	void searchBtnClicked();
	void backBtnClicked();
	
protected:
	void closeEvent(QCloseEvent*);
	
private:
	void _search(const QString&, bool addToHistory=true, bool reallyMap=true);
	
	QLineEdit *m_textBox;
	MindSpaceGraphWidget *m_gw;
	QStringList m_history;
	QPushButton *m_backBtn;
	QString m_lastSearch;
	MSpace *m_mind;
	QString m_lastFileLoaded;
	
};


#endif
