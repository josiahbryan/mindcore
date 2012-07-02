#include "AnnNode.h"
#include "AnnNetwork.h"

#include <QtGui>


int main(int argc, char **argv)
{
	QApplication app(argc,argv);
	
	AnnNetwork *ann = new AnnNetwork();
	
// 	1.0 1.0
// 	0.0
// 	1.0 0.0
// 	1.0
// 	0.0 1.0
// 	1.0
// 	0.0 0.0
// 	0.0

	QString filename = argc > 1 ? argv[1] : "";
	bool validateOnly = argc > 2 ? QString(argv[2]) == "1" : false; 
	
	QList<QList<float> > inputs;
	QList<QList<float> > targets;
	
	if(filename.isEmpty())
	{
		ann->setupLayers(QList<int>() << 2 << 3 << 1);
		
		qDebug() << "Internal: Loading XOR training data...";
		inputs  << (QList<float>() << 1. << 1.);
		targets << (QList<float>() << 0.);
		
		inputs  << (QList<float>() << 1. << 0.);
		targets << (QList<float>() << 1.);
		
		inputs  << (QList<float>() << 0. << 1.);
		targets << (QList<float>() << 1.);
		
		inputs  << (QList<float>() << 0. << 0.);
		targets << (QList<float>() << 0.);
	}
	else
	{
		QFile file(filename);
		if(!file.open(QIODevice::ReadOnly))
		{
			qDebug() << "Unable to read "<<filename;
			return -1;
		}
	
		QTextStream stream(&file);
		QString fileContents = stream.readAll();
		fileContents = fileContents.replace("\r\n","\n");
		
		QStringList lines = fileContents.split("\n");
		
		int numPairs = -1;
		int numInputs = -1;
		int numOutputs = -1;
		bool flag = false;
		foreach(QString line, lines)
		{
			if(line.startsWith("#") || line.isEmpty())
				continue;
			if(numPairs < 0)
				numPairs = line.toInt();
			else
			if(numInputs < 0)
				numInputs = line.toInt();
			else
			if(numOutputs < 0)
				numOutputs = line.toInt();
			else
			{
				QStringList numbers = line.split(" ");
				QList<float> list;
				foreach(QString num, numbers)
					list << num.toFloat();
					
				if(!flag)
					inputs.append(list);
				else
					targets.append(list);
				
				//qDebug() << line << "Read list: "<<list<<", flag:"<<flag;
				flag = !flag;
			}
		}
			
		
		ann->setupLayers(QList<int>() << numInputs << (numInputs * 2) << numOutputs);
		
		qDebug() << "Setup network from "<<filename<<", "<<numInputs<<" inputs, "<<numOutputs<<" outputs, "<<inputs.size()<<" inputs, "<<targets.size()<<" targets";
		qDebug() << "Inputs:  "<<inputs;
		qDebug() << "Targets: "<<targets;
		
		if(inputs.size() != targets.size())
		{
			qDebug() << "Error reading file: inputs.size() != targets.size()";
			exit(-1);
		}
	}
	
// 	ann->printAll();
// 	exit(-1);

// 	
	ann->train(inputs, targets);
	float error = ann->validate(inputs, targets);
	qDebug() << "Validate: "<<error;
	
	//ann->printAll();
	
	//return app.exec();
	return 0;
}
