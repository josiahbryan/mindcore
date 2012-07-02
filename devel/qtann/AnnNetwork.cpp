#include "AnnNetwork.h"
#include "AnnNode.h"

AnnNetwork::AnnNetwork()
{
}

AnnNetwork::~AnnNetwork()
{
	if(!m_layers.isEmpty())
	{
		qDeleteAll(m_layers);
		m_layers.clear();
	}
}

void AnnNetwork::setupLayers(QList<int> layerSizes)
{
	// for development, hardcode simple 2-3-1 network
	
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

QList<float> AnnNetwork::compute(QList<float> inputPattern)
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
	QList<float> output;
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
float AnnNetwork::calculateError(QList<float> targetPattern)
{
	float globalError = 0.;
	int counter = 0;
	foreach(AnnNode *node, m_layers.last()->nodes())
	{
		float output = node->value();
		float error = targetPattern[counter ++] - output;
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
			float error = 0.0;
			foreach(AnnNode *upperNode, upperLayer->nodes())
				error += upperNode->weight(lowerNode) * upperNode->error();
			
			float output = lowerNode->value();
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
				float finalError = error; //output * error;
				//qDebug() << "AnnNetwork::propogateError(): "<<layerNum<<": "<<lowerNode<<": "<<finalError;
				lowerNode->setError(finalError);
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
	float learningRate = DEFAULT_LEARNING_RATE_TANH;
	
	// use 'for' instead of 'foreach' because we want to start at first()+1
	for(int layerNum = 1; layerNum<m_layers.size(); layerNum++)
	{
		AnnLayer *upperLayer = m_layers[layerNum];
		AnnLayer *lowerLayer = m_layers[layerNum - 1];
		
		foreach(AnnNode *upperNode, upperLayer->nodes())
		{
			float learningFactor = learningRate * upperNode->error();
			
// 			if(learningFactor == 0)
// 			{
// 				const float range = 2.;
// 				float rv = ((float)(rand() % (int)range) - (range/2.)) / 100.; // Add a random +/- 5% to the value to prevent deadlocking
// 				
// 				//m_learnRate += rv;
// 
// 				learningFactor = learningRate + rv;
// 			}
			
			foreach(AnnNode *lowerNode, lowerLayer->nodes())
			{
				float delta = learningFactor * lowerNode->value();
					//+ momentum * upperNode->delta();
				
// 				if(learningFactor == learningRate)
// 				{
// 					const float range = 2.;
// 					float rv = ((float)(rand() % (int)range) - (range/2.)) / 100.; // Add a random +/- 5% to the value to prevent deadlocking
// 					
// 					//m_learnRate += rv;
// 	
// 					learningFactor = learningRate + rv;
// 				}
				
					
				upperNode->setWeight(lowerNode, upperNode->weight(lowerNode) + delta);
				upperNode->setDelta(lowerNode, delta);
				
				//qDebug() << "AnnNetwork::adjustWeights(): "<<layerNum<<": "<<upperNode<<" ( lf:"<<learningFactor<<") -> "<<lowerNode<<": delta:"<<delta<<", weight:"<<upperNode->weight(lowerNode);
				
				//qDebug() << "AnnNetwork::adjustWeights(): layerNum:"<<layerNum<<", n:"<<upperNode<<"/"<<lowerNode<<", lf:"<<learningFactor<<", unErr:"<<upperNode->error()<<", d:"<<delta<<", w:"<<upperNode->weight(lowerNode);
				
			}
		}
	}
}


void AnnNetwork::batchAdjustWeights(int numPatterns)
{
	float learningFactor = DEFAULT_LEARNING_RATE_TANH / ( (float) numPatterns );

	// use 'for' instead of 'foreach' because we want to start at first()+1
	for(int layerNum = 1; layerNum<m_layers.size(); layerNum++)
	{
		AnnLayer *upperLayer = m_layers[layerNum];
		AnnLayer *lowerLayer = m_layers[layerNum - 1];
		
		foreach(AnnNode *upperNode, upperLayer->nodes())
		{
			//float learningFactor = learningRate * upperNode->error();
			//qDebug() << "AnnNetwork::batchAdjustWeights(): "<<layerNum<<": "<<upperNode<<": learningRate:"<<learningRate<<", error:"<<upperNode->error();
			
			foreach(AnnNode *lowerNode, lowerLayer->nodes())
			{
				float delta = learningFactor * upperNode->sumDelta(lowerNode);
					//+ momentum * upperNode->delta();
				
				upperNode->setWeight(lowerNode, upperNode->weight(lowerNode) + delta);
				upperNode->setDelta(lowerNode, delta);
				
				qDebug() << "AnnNetwork::batchAdjustWeights(): "<<layerNum<<": "<<upperNode<<" ( lf:"<<learningFactor<<") -> "<<lowerNode<<": delta:"<<delta<<", weight:"<<upperNode->weight(lowerNode);
				
				//qDebug() << "AnnNetwork::adjustWeights(): layerNum:"<<layerNum<<", n:"<<upperNode<<"/"<<lowerNode<<", lf:"<<learningFactor<<", unErr:"<<upperNode->error()<<", d:"<<delta<<", w:"<<upperNode->weight(lowerNode);
				
			}
		}
	}
}


void AnnNetwork::batchAdjustSumDeltas()
{
	// use 'for' instead of 'foreach' because we want to start at first()+1
	for(int layerNum = 1; layerNum<m_layers.size(); layerNum++)
	{
		AnnLayer *upperLayer = m_layers[layerNum];
		AnnLayer *lowerLayer = m_layers[layerNum - 1];
		
		foreach(AnnNode *upperNode, upperLayer->nodes())
		{
			float error = upperNode->error();
			foreach(AnnNode *lowerNode, lowerLayer->nodes())
			{
				float change = error * lowerNode->value();
				upperNode->setSumDelta(lowerNode,
					upperNode->sumDelta(lowerNode) + change);
					
				qDebug() << "AnnNetwork::batchAdjustSumDeltas(): "<<layerNum<<": "<<upperNode<<" ( err:"<<error<<") -> "<<lowerNode<<": change:"<<change<<", sumDelta:"<<upperNode->sumDelta(lowerNode);
			}
		}
	}
}

void AnnNetwork::batchResetSumDeltas()
{
	// use 'for' instead of 'foreach' because we want to start at first()+1
	for(int layerNum = 1; layerNum<m_layers.size(); layerNum++)
	{
		AnnLayer *upperLayer = m_layers[layerNum];
		AnnLayer *lowerLayer = m_layers[layerNum - 1];
		
		foreach(AnnNode *upperNode, upperLayer->nodes())
		{
			foreach(AnnNode *lowerNode, lowerLayer->nodes())
			{
				upperNode->setSumDelta(lowerNode, 0);
			}
		}
	}
}


void AnnNetwork::printAll()
{
	//qDebug() << "AnnNetwork::printAll()";
	foreach(AnnLayer *upperLayer, m_layers)
	{
		foreach(AnnNode *upperNode, upperLayer->nodes())
		{
			foreach(AnnNode *lowerNode, upperNode->inputs())
			{
				printf("%.06f ",upperNode->weight(lowerNode));
			}
			if(upperNode->hasInputs())
				printf("\n");
		}
	}
}

float AnnNetwork::validate(QList<QList<float> > inputs, QList<QList<float> > targets)
{
	int size = inputs.size();
	
	float errorSum = 0.0;
	for(int patternNum=0; patternNum<size; patternNum++)
	{
		QList<float> inputPattern   = inputs[patternNum];
		QList<float> targetPattern  = targets[patternNum];
		
		QList<float> computedOutput = compute(inputPattern);
		
		errorSum += calculateError(targetPattern);
	}
	
	return errorSum / ((float)size);
}

float AnnNetwork::batchTrain(QList<QList<float> > inputs, QList<QList<float> > targets, float desiredError, bool stopOnOverfit)
{
	int count = 0;
	
	int size = inputs.size();
	
	bool verbose = true;
	
	float errorSum = 0.0;
	float error = 1.0;
	while(error > desiredError)
	{
		errorSum = 0.0;
			
		batchResetSumDeltas();
		
		for(int patternNum=0; patternNum<size; patternNum++)
		{
			QList<float> inputPattern   = inputs[patternNum];
			QList<float> targetPattern  = targets[patternNum];
			
			QList<float> computedOutput = compute(inputPattern);
			error = calculateError(targetPattern);
			
			errorSum += error;
			
			if(verbose)
				qDebug() << "AnnNetwork::batchTrain(): count:" << count << ", pattern:"<<patternNum<<inputPattern<<", output:" << computedOutput<<", expected:"<<targetPattern <<", error:"<<error; //QString().sprintf("%.06f",error);
				
			propogateError();
			batchAdjustSumDeltas();
			
			count ++;
		}
		
		batchAdjustWeights(size);
		
		error = errorSum / size;
		
		if(verbose)
			qDebug() << "AnnNetwork::batchTrain(): count:" << count << ", error:"<<error;
	}
	
	if(verbose)
		qDebug() << "AnnNetwork::batchTrain(): count:"<<count<<", final error:"<<error;
	
	return error;
}


float AnnNetwork::train(QList<QList<float> > inputs, QList<QList<float> > targets, float desiredError, bool stopOnOverfit)
{
	int count = 0;
	
	int size = inputs.size();
	
	bool verbose = false;
	float error = 1.0;
	for(int patternNum=0; patternNum<size; patternNum++)
	{
		error = 1.;
		while(error > desiredError)
		{
			QList<float> inputPattern   = inputs[patternNum];
			QList<float> targetPattern  = targets[patternNum];
			
			QList<float> computedOutput = compute(inputPattern);
			error = calculateError(targetPattern);
			
			if(verbose)
				qDebug() << "AnnNetwork::train(): count:" << count << ", pattern:"<<patternNum<<inputPattern<<", output:" << computedOutput<<", expected:"<<targetPattern <<", error:"<<error; //QString().sprintf("%.06f",error);
			
			propogateError();
			adjustWeights();
			
			count ++;
		}
		
		//if(verbose)
			qDebug() << "AnnNetwork::train(): count:" << count << ", Final error for pattern "<<patternNum<<":"<<error;
	}
	
	//qDebug() << "AnnNetwork::train(): count:"<<count<<", final error:"<<error;
	
	return error;
}
