#include "AnnNode.h"

#define DEFAULT_MOMENTUM 0.0

#define DEFAULT_LEARNING_RATE_LOGISTIC 0.3
#define DEFAULT_LEARNING_RATE_TANH 0.1

#define DEFAULT_MAX_NU 30
#define DEFAULT_MIN_NU 0.00000001
#define DEFAULT_NUUP 1.2
#define DEFAULT_NUDOWN 0.8
#define min(X, Y)  ((X) < (Y) ? (X) : (Y))
#define max(X, Y)  ((X) > (Y) ? (X) : (Y))


AnnNode::AnnNode()
	: m_value(0.)
	, m_learnRate(0.1)
	, m_func(TanH)
	, m_alphaParam(0.)
	, m_betaParam(0.)
	, m_error(0.)
{
	const float range = 5.;
	float rv = ((float)(rand() % (int)range) - (range/2.)) / 100.; // Add a random +/- 5% to the value to prevent deadlocking
	
	m_learnRate += rv;
}

AnnNode::~AnnNode()
{
}

void AnnNode::setInputs(QList<AnnNode*> nodes)
{
	m_inputs.clear();
	m_weights.clear();
	m_deltas.clear();
	m_sumDeltas.clear();
	
	foreach(AnnNode *node, nodes)
		addInput(node);
}

void AnnNode::addInput(AnnNode *input)
{
	//const float range = 5.;
	//float rv = ((float)(rand() % (int)range) - (range/2.)) / 100.; // Add a random +/- 5% to the value to prevent deadlocking
	
	#define RANDOM_LARGE_WEIGHTS_LOGISTIC 0.1
	#define RANDOM_LARGE_WEIGHTS_TANH 0.05
	float range = RANDOM_LARGE_WEIGHTS_TANH;

	if(m_inputs.contains(input))
		return;
		
	m_inputs.append(input);
	
	m_deltas[input] = 0;
	m_sumDeltas[input] = 0;
	
	m_weights[input] = range * ((float) rand () / RAND_MAX - 0.5);
}

void AnnNode::removeInput(AnnNode *input)
{
	m_weights.remove(input);
	m_deltas.remove(input);
	m_sumDeltas.remove(input);
	m_inputs.removeAll(input);
}

void AnnNode::setInputValue(float val)
{
	if(!hasInputs())
		m_value = val;
}

float AnnNode::value()
{
	return m_value;
}

float AnnNode::compute()
{
	//qDebug() << "AnnNode::compute(): "<<this<<" hasInputs():"<<hasInputs();

	if(!hasInputs())
		return m_value;
	
	float sum = 0.;
	foreach(AnnNode *node, inputs())
		sum += node->value() * m_weights[node];
	
	//float val = sum / (float)inputs().size();
	float val = sum;
	
	if(m_func == AnnNode::Linear)
		m_value = val;
	else
	if(m_func == AnnNode::Threshold)
	{
		if(val < m_alphaParam)
			m_value = -1.0;
		else
		if(val >= m_betaParam)
			m_value = 1.0;
		else
			m_value = 0.0;
	}
	else
	if(m_func == AnnNode::TanH)
		m_value = tanh(val);
	else
	if(m_func == AnnNode::Logistic)
		m_value = 1.0 / (1.0 + exp (-val));
		
	//qDebug() << "AnnNode::compute(): "<<this<<" m_value:"<<m_value;

	// Fallthru...
	return m_value;
}

float AnnNode::adjustWeights(float target)
{
	//compute_output_error
	
	float output = m_value;
	float error = target - output;
	
	if(m_func == AnnNode::Logistic)
	{
		error = output * (1 - output) * error;
	}
	else
	if(m_func == AnnNode::TanH)
	{
		error = (1 - output * output) * error;
	}
	
	float returnError = error * error;
	
	//backpropagate_layer
	float computeError = 0.;
	//foreach(AnnNode *node, inputs())
	//	computeError += m_weights[node] * m_weights[node];
	
// 	if(error > 0)
// 		m_learnRate = fabs(m_learnRate)*-1;
// 	else
// 		m_learnRate = fabs(m_learnRate);
// 	
	//m_learnRate /= (1.0-error*error);
	
	foreach(AnnNode *node, inputs())
	{
		float output = node->value();
		float thisError = error;;
	
		if(m_func == AnnNode::Logistic)
		{
			thisError = output * (1 - output) * error;
		}
		else
		if(m_func == AnnNode::TanH)
		{
			thisError = (1 - output * output) * error;
		}
		
		float learning_factor = DEFAULT_LEARNING_RATE_TANH * thisError;
		
		float delta =
			learning_factor *
			output;
			//momentum * layer[l].neuron[nu].delta[nl];
		m_weights[node] += delta;
// 		layer[l].neuron[nu].delta[nl] = delta;
// 		
// 		float oldWeight = m_weights[node];
// 		m_weights[node] += m_weights[node] * m_learnRate;
		qDebug() << this << "AnnNode::adjustWeights(): error:"<<error<<", thisError:"<<thisError<<", learning_factor:"<<learning_factor<<", node:"<<node<<", weight:"<<m_weights[node];
		
		//node->adjustWeights(error);// * oldWeight);
	} 
}

void AnnNode::setWeight(AnnNode *inputNode, float weight)
{
	m_weights[inputNode] = weight;
}

void AnnNode::setActFunc(ActFunc func, float alphaParam, float betaParam)
{
	m_func = func;
	m_alphaParam = alphaParam;
	m_betaParam = betaParam;
}
