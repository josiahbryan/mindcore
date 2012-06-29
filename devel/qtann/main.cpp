#include "AnnNode.h"

#define NETWORK_ACTIVATION AnnNode::TanH

class AnnLayer
{
public:
	AnnLayer() {};
	~AnnLayer() { qDeleteAll(m_list); }
	QList<AnnNode*> nodes() { return m_list; }
	void setNodes(QList<AnnNode*> nodes) { m_list = nodes; }
	
protected:
	QList<AnnNode*> m_list;
};

class AnnNetwork
{
public:
	AnnNetwork();
	~AnnNetwork();
	
	QList<AnnLayer*> layers() { return m_layers; }
	void setupLayers();
	
	QList<double> compute(QList<double> inputPattern);
	double calculateError(QList<double> targetPattern); // uses most recent values from compute to calculate error
	
	//double train(QList<QList<double> > inputPatterns, QList<QList<double> > validationPatterns, QList<QList<double> > checkPatterns=QList<QList<double> >(), double desiredError=0.1, bool stopOnOverfit=true);

	void propogateError();
	void adjustWeights();
	
protected:
	QList<AnnLayer*> m_layers;
};


AnnNetwork::AnnNetwork()
{
	setupLayers();
}

AnnNetwork::~AnnNetwork()
{
	if(!m_layers.isEmpty())
	{
		qDeleteAll(m_layers);
		m_layers.clear();
	}
}

void AnnNetwork::setupLayers()
{
	// for development, hardcode simple 2-3-1 network
	
	QList<int> layerSizes = QList<int>() << 2 << 3 << 1;
	
	foreach(int layerSize, layerSizes)
	{
		QList<AnnNode*> nodes;
		for(int i=0; i<layerSize; i++)
			nodes << new AnnNode();
		
		AnnLayer *layer = new AnnLayer();
		layer->setNodes(nodes);
		m_layers << layer;
	}	
	
	// use 'for' instead of 'foreach' because we want to start at last()-1
	for(int layerNum = m_layers.size()-1; layerNum>0; layerNum--)
		foreach(AnnNode *upperNode, m_layers[layerNum]->nodes())
			upperNode->setInputs(m_layers[layerNum-1]->nodes());
}

QList<double> AnnNetwork::compute(QList<double> inputPattern)
{
	// Set inputs
	int counter = 0;
	foreach(AnnNode *inputNode, m_layers.first()->nodes())
		inputNode->setInputValue(inputPattern[counter++]);
	
	// Compute values
	foreach(AnnLayer *layer, m_layers)
	{
		//qDebug() << "AnnNetwork::compute(): Layer:"<<layer;
		foreach(AnnNode *node, layer->nodes())
			node->compute();
	}
	
	// Retrieve outputs
	QList<double> output;
	foreach(AnnNode *upperNode, m_layers.last()->nodes())
		output << upperNode->value();

	return output;
}

/*!\brief Compute the output error of a network.

 * \param targetPattern A sequence of numbers.
 * \return Output error of the neural network.
 *
 * The return value is the square of the Euclidean distance between the 
 * actual output and the target. This routine also prepares the network
 * for  backpropagation training by storing (internally in the neural
 * network) the errors associated with each of the outputs. */
double AnnNetwork::calculateError(QList<double> targetPattern)
{
	double globalError = 0.;
	int counter = 0;
	foreach(AnnNode *node, m_layers.last()->nodes())
	{
		double output = node->value();
		double error = targetPattern[counter ++] - output;
		/*
		if(NETWORK_ACTIVATION == AnnNode::TanH)
		{
			node->setError(output * (1 - output) * error);
		}
		else
		if(NETWORK_ACTIVATION == AnnNode::Logistic)
		{
			node->setError((1 - output * output) * error);
		}
		else
		*/
		{
			node->setError(error);
		}
		
		globalError += error * error;
	}
	
	return globalError;
}

void AnnNetwork::propogateError()
{
	// use 'for' instead of 'foreach' because we want to start at last()-1
	for(int layerNum = m_layers.size()-1; layerNum>0; layerNum--)
	{
		AnnLayer *upperLayer = m_layers[layerNum];
		AnnLayer *lowerLayer = m_layers[layerNum - 1];
		
		foreach(AnnNode *lowerNode, lowerLayer->nodes())
		{
			double error = 0.0;
			foreach(AnnNode *upperNode, upperLayer->nodes())
				error += upperNode->weight(lowerNode) * upperNode->error();
			
			double output = lowerNode->value();
			/*
			if(NETWORK_ACTIVATION == AnnNode::TanH)
			{
				lowerNode->setError(output * (1 - output) * error);
			}
			else
			if(NETWORK_ACTIVATION == AnnNode::Logistic)
			{
				lowerNode->setError((1 - output * output) * error);
			}
			else
			*/
			{
				lowerNode->setError(error);
			}
		}
	}
}

/*!\brief [Internal] Default learning rate of a network (for logistic and tanh)
 */
#define DEFAULT_LEARNING_RATE_LOGISTIC 0.3
#define DEFAULT_LEARNING_RATE_TANH 0.1

void AnnNetwork::adjustWeights()
{
	double learningRate = DEFAULT_LEARNING_RATE_TANH;
	
	// use 'for' instead of 'foreach' because we want to start at first()+1
	for(int layerNum = 1; layerNum<m_layers.size(); layerNum++)
	{
		AnnLayer *upperLayer = m_layers[layerNum];
		AnnLayer *lowerLayer = m_layers[layerNum - 1];
		
		foreach(AnnNode *upperNode, upperLayer->nodes())
		{
			double learningFactor = learningRate * upperNode->error();
			
			foreach(AnnNode *lowerNode, lowerLayer->nodes())
			{
				double delta = learningFactor * lowerNode->value();
					//+ momentum * upperNode->delta();
				
				upperNode->setWeight(lowerNode, upperNode->weight(lowerNode) + delta);
				upperNode->setDelta(lowerNode, delta);
			}
		}
	}
}

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

	QList<QList<double> > inputs;
	QList<QList<double> > targets;
	
	inputs  << (QList<double>() << 1. << 1.);
	targets << (QList<double>() << 0.);
	
	inputs  << (QList<double>() << 1. << 0.);
	targets << (QList<double>() << 1.);
	
	inputs  << (QList<double>() << 0. << 1.);
	targets << (QList<double>() << 1.);
	
	inputs  << (QList<double>() << 0. << 0.);
	targets << (QList<double>() << 0.);
	
	double error = 1.0;
	
	int count = 0;
	
	int size = inputs.size();
	//size = 3;
	for(int patternNum=0; patternNum<size; patternNum++)
	//for(int patternNum=size-1; patternNum>-1; patternNum--)
	{
		error = 1.;
		while(error > 0.001)
		{
			QList<double> inputPattern = inputs[patternNum];
			QList<double> targetPattern = targets[patternNum];
			
			QList<double> computedOutput = ann->compute(inputPattern);
			error = ann->calculateError(targetPattern);
			
			qDebug() << "main(): count:" << count << ", pattern:"<<patternNum<<"("<<inputPattern[0]<<inputPattern[1]<<"), value:" << QString().sprintf("%.2f",computedOutput.first()) <<", error:"<<error; //QString().sprintf("%.06f",error);
			
			ann->propogateError();
			ann->adjustWeights();
			
			count ++;
			
			//error = 0;
		}
	}
	
	qDebug() << "main(): count:"<<count<<", final error:"<<error;
	
	
	//return app.exec();
	return 0;
}
