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
	
	// Polls all inputs() for value() and uses actFunc() to calculate the return value
	double value();
	
	void setWeight(AnnNode *inputNode, double weight);
	
	void adjustWeights(double error);
	
	enum ActFunc {
		Linear=0,
		Threshold,
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
	double m_value;
	
	double m_learnRate;
	
	ActFunc m_func;
	double m_alphaParam;
	double m_betaParam;
	
};

#endif
