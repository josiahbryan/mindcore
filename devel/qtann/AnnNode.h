#ifndef AnnNode_H
#define AnnNode_H

#include <QtCore>


class AnnNode : public QObject
{
	Q_OBJECT
public:
	AnnNode();
	~AnnNode();
	
	void setInputs(QList<AnnNode*>);
	void addInput(AnnNode *input);
	void removeInput(AnnNode *input);
	
	QList<AnnNode*> inputs() { return m_inputs; }
	bool hasInputs() { return !m_inputs.isEmpty(); }
	
	// Irrelevant if hasInputs()
	void setInputValue(float);
	
	// Output of last compute
	float value();
	
	// Polls all inputs() for output() and uses actFunc() to calculate the return value
	float compute();
	
	float error() { return m_error; }
	void setError(float error) { m_error = error; }
	
	void setWeight(AnnNode *inputNode, float weight);
	float weight(AnnNode *inputNode) { return m_weights[inputNode]; }
	
	void setDelta(AnnNode *inputNode, float d) { m_deltas[inputNode]=d; }
	float delta(AnnNode *inputNode) { return m_deltas[inputNode]; }
	
	void setSumDelta(AnnNode *inputNode, float d) { m_sumDeltas[inputNode]=d; }
	float sumDelta(AnnNode *inputNode) { return m_sumDeltas[inputNode]; }
	
	float adjustWeights(float target);
	
	enum ActFunc {
		Linear=0,
		Threshold,
		TanH,
		Logistic,
	};
	
	void setActFunc(ActFunc func, float alphaParam=0.5, float betaParam=0.5);
	
	ActFunc actFunc()   { return m_func; }
	float alphaParam() { return m_alphaParam; }
	float betaParam()  { return m_betaParam; }

protected:
// 	// addInput(node) calls node->registerOutput(this), so that setValue(value) can calculate value then call each output with setValue()
// 	void registerOutput(AnnNode*);
// 	void removeOutput(AnnNode*);
// 	QList<AnnNode*> outputs();
// 	
// 	void outputValue
	
protected:
// 	QList<AnnNode*> m_outputs;
	
	QList<AnnNode*> m_inputs;
	QHash<AnnNode*,float> m_weights;
	QHash<AnnNode*,float> m_deltas;
	QHash<AnnNode*,float> m_sumDeltas;
	float m_value;
	
	float m_learnRate;
	
	ActFunc m_func;
	float m_alphaParam;
	float m_betaParam;
	
	float m_error;
	float m_delta;
	
};

#endif
