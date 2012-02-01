#ifndef BotWindow_H
#define BotWindow_H

#include <QtGui>

#include "MindSpace.h"
using namespace MindSpace;
class MindSpaceGraphWidget;

class BotWindow : public QWidget
{
	Q_OBJECT
public:
	BotWindow();
	~BotWindow() {}

public slots:
	void search(const QString&, bool addToHistory=true);
	
private slots:
	void nodeDoubleClicked(MNode*);
	void searchBtnClicked();
	void backBtnClicked();
	
protected:
	void closeEvent(QCloseEvent*);
	
private:
	QLineEdit *m_textBox;
	MindSpaceGraphWidget *m_gw;
	QStringList m_history;
	QPushButton *m_backBtn;
	QString m_lastSearch;
	
};


#endif
