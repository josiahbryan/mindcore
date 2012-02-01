#include "BotWindow.h"
#include <QApplication>

int main(int argc, char **argv)
{
	QApplication::setGraphicsSystem("raster");
	QApplication app(argc, argv);
	
	BotWindow *mw = new BotWindow;
	mw->show();
	
	return app.exec();
}
