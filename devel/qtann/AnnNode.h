#ifndef AnnNode_H
#define AnnNode_H

#include <QtGui>


class AnnNode : public QObject
{
	Q_OBJECT
public:
	AnnNode();
	~AnnNode();
	
	void setInputs(QList<AnnNode*>);
	void addInput(AnnNode *input);
	void removeInput(AnnNode *input);
	
	QList<AnnNode*> inputs() { return m_weights.keys(); }
	bool hasInputs() { return !inputs().isEmpty(); }
	
	// Irrelevant if hasInputs()
	void setInputValue(double);
	
	// Output of last compute
	double value();
	
	// Polls all inputs() for output() and uses actFunc() to calculate the return value
	double compute();
	
	double error() { return m_error; }
	void setError(double error) { m_error = error; }
	
	void setWeight(AnnNode *inputNode, double weight);
	double weight(AnnNode *inputNode) { return m_weights[inputNode]; }
	
	void setDelta(AnnNode *inputNode, double d) { m_deltas[inputNode]=d; }
	double delta(AnnNode *inputNode) { return m_deltas[inputNode]; }
	
	double adjustWeights(double target);
	
	enum ActFunc {
		Linear=0,
		Threshold,
		TanH,
		Logistic,
	};
	
	void setActFunc(ActFunc func, double alphaParam=0.5, double betaParam=0.5);
	
	ActFunc actFunc()   { return m_func; }
	double alphaParam() { return m_alphaParam; }
	double betaParam()  { return m_betaParam; }

protected:
// 	// addInput(node) calls node->registerOutput(this), so that setValue(value) can calculate value then call each output with setValue()
// 	void registerOutput(AnnNode*);
// 	void removeOutput(AnnNode*);
// 	QList<AnnNode*> outputs();
// 	
// 	void outputValue
	
protected:
// 	QList<AnnNode*> m_outputs;
	
	QHash<AnnNode*,double> m_weights;
	QHash<AnnNode*,double> m_deltas;
	double m_value;
	
	double m_learnRate;
	
	ActFunc m_func;
	double m_alphaParam;
	double m_betaParam;
	
	double m_error;
	double m_delta;
	
};

#endif
