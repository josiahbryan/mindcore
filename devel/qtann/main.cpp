#include "AnnNode.h"

int main(int argc, char **argv)
{
	QApplication app(argc,argv);
	
	AnnNode *input1 = new AnnNode();
	AnnNode *input2 = new AnnNode();
	
	AnnNode *node1 = new AnnNode();
	AnnNode *node2 = new AnnNode();
	AnnNode *node3 = new AnnNode();
	AnnNode *output = new AnnNode();
	
	output->addInput(node1);
	output->addInput(node2);
	output->addInput(node3);
	
	node1->addInput(input1);
	node1->addInput(input2);
	node2->addInput(input1);
	node2->addInput(input2);
	node3->addInput(input1);
	node3->addInput(input2);
	
	input1->setInputValue(0.99);
	input2->setInputValue(0.01);
	
	double target = 0.99;
	
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
