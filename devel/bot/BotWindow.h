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
	//void addTestItem();
	
private slots:
	void nodeDoubleClicked(MNode*);
	void searchBtnClicked();
	
protected:
	void closeEvent(QCloseEvent*);
	
private:
	QLineEdit *m_textBox;
	MindSpaceGraphWidget *m_gw;
	
};


#endif
