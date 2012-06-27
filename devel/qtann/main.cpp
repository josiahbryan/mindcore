#include "AnnNode.h"

int main(int argc, char **argv)
{
	QApplication app(argc,argv);
	
	AnnNode *node1 = new AnnNode();
	AnnNode *node2 = new AnnNode();
	AnnNode *output = new AnnNode();
	
	output->addInput(node1);
	output->addInput(node2);
	
	node1->setInputValue(0.25);
	node2->setInputValue(0.75);
	
	double target = 0.5;
	
	double error = 1.0;
	
	int count = 0;
	while(fabs(error) > 0.01)
	{
		double value = output->value();
		error = target - value;
		
		qDebug() << "main(): value:" << value<<", error:"<<error;
		
		output->adjustWeights(error);
		
		count ++;
	}
	
	qDebug() << "main(): count:"<<count<<", final error:"<<error;
	
	
	//return app.exec();
	return 0;
}
