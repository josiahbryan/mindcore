#ifndef BotWindow_H
#define BotWindow_H

#include <QtGui>


class BotWindow : public QWidget
{
	Q_OBJECT
public:
	BotWindow();
	~BotWindow() {}

public slots:
	void addTestItem();
	
	
protected:
	void closeEvent(QCloseEvent*);
	
private:
	
};


#endif
