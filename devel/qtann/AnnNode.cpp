#include "AnnNode.h"

AnnNode::AnnNode()
	: m_value(0.)
	, m_learnRate(0.01)
	, m_func(Linear)
	, m_alphaParam(0.)
	, m_betaParam(0.)
{
	const double range = 5.;
	double rv = ((double)(rand() % (int)range) - (range/2.)) / 100.; // Add a random +/- 5% to the value to prevent deadlocking
	
	//m_learnRate += rv;
}

AnnNode::~AnnNode()
{
}

void AnnNode::setInputs(QList<AnnNode*> nodes)
{
	m_weights.clear();
	
	foreach(AnnNode *node, nodes)
		addInput(node);
}

void AnnNode::addInput(AnnNode *input)
{
	const double range = 5.;
	double rv = ((double)(rand() % (int)range) - (range/2.)) / 100.; // Add a random +/- 5% to the value to prevent deadlocking
	
	m_weights[input] = 0.5 + rv;
}

void AnnNode::removeInput(AnnNode *input)
{
	m_weights.remove(input);
}

void AnnNode::setInputValue(double val)
{
	if(!hasInputs())
		m_value = val;
}

double AnnNode::value()
{
	if(!hasInputs())
		return m_value;
	
	double sum = 0.;
	foreach(AnnNode *node, inputs())
		sum += node->value() * m_weights[node];
	
	double avg = sum / (double)inputs().size();
	
	if(m_func == AnnNode::Linear)
		return avg;
		
	if(m_func == AnnNode::Threshold)
	{
		// defaults for alpha/beta are 0.5, so this just returns 0.1 if avg<0.5 and 0.99 if avg>=0.5, never returns 0.50 with default alpha/beta
		if(avg < m_alphaParam)
			return 0.01;
		if(avg >= m_betaParam)
			return 0.99;
		return 0.50;
	}

	// Fallthru...
	return avg;
}

void AnnNode::adjustWeights(double error)
{
	if(error > 0)
		m_learnRate = fabs(m_learnRate)*-1;
	else
		m_learnRate = fabs(m_learnRate);
	
	m_learnRate /= (1.0-error*error);
	
	foreach(AnnNode *node, inputs())
	{
		double oldWeight = m_weights[node];
		m_weights[node] *= m_learnRate;
		qDebug() << this << "AnnNode::adjustWeights(): error:"<<error<<", learnRate:"<<m_learnRate<<", node:"<<node<<", weight:"<<m_weights[node]<<", error sqd:"<<(1.0-error*error);
		
		node->adjustWeights(error);// * oldWeight);
	} 
}

void AnnNode::setWeight(AnnNode *inputNode, double weight)
{
	m_weights[inputNode] = weight;
}

void AnnNode::setActFunc(ActFunc func, double alphaParam, double betaParam)
{
	m_func = func;
	m_alphaParam = alphaParam;
	m_betaParam = betaParam;
}
