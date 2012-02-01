#include "MSpaceViewerWidget.h"
#include <QApplication>

int main(int argc, char **argv)
{
	QApplication::setGraphicsSystem("raster");
	QApplication app(argc, argv);
	
	MSpaceViewerWidget *mw = new MSpaceViewerWidget;
	if(argc > 1)
		mw->loadFile(argv[1]);
	else
		mw->loadConceptNetDemo();
	mw->show();
	
	return app.exec();
}
