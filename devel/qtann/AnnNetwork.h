#ifndef AnnNetwork_H
#define AnnNetwork_H

class AnnNode;
#include <QtCore>

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
	void setupLayers(QList<int>);
	
	QList<float> compute(QList<float> inputPattern);
	float calculateError(QList<float> targetPattern); // uses most recent values from compute to calculate error
	
	void propogateError();
	void adjustWeights();
	
	void printAll();
	
	void batchResetSumDeltas();
	void batchAdjustSumDeltas();
	void batchAdjustWeights(int numPatterns);
	
	
	float train(QList<QList<float> > inputPatterns, QList<QList<float> > targetPatterns, float desiredError=0.001, bool stopOnOverfit=true);
	float batchTrain(QList<QList<float> > inputPatterns, QList<QList<float> > targetPatterns, float desiredError=0.001, bool stopOnOverfit=true);
	float validate(QList<QList<float> > inputPatterns, QList<QList<float> > targetPatterns);
	
protected:
	QList<AnnLayer*> m_layers;
};

#endif
