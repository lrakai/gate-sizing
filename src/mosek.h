// class to abstract away interfacing with mosek 

#ifndef MOSEK_H_
#define MOSEK_H_

#include <vector>
#include <map>
#include <string>

namespace gateSizing {

class mosek
{
public:
	mosek()
	:m_objective(-1), m_currentVar(0)
	{}

	struct sparseTerm
	{	
		sparseTerm(unsigned int t, unsigned int v, int e)
		:term(t), variable(v), exponent(e)
		{}
	
		unsigned int term;
		unsigned int variable;
		int exponent;
	};
	
	inline void addTerm(unsigned int t, unsigned int v, int e)
	{	
		if(m_id2Var.find(v) == m_id2Var.end()) // not found
			m_id2Var[v] = m_currentVar++;
		m_constraintMatrix.push_back(sparseTerm(t, m_id2Var[v], e));
	}

	inline void addCoefficient(double c)
	{	
		m_coefficients.push_back(c);
	}
	
	inline void addAssociation(double c)
	{	
		m_associations.push_back(c);
	}
	
	void solve(const std::string& problemName);
	void writeProblem(std::string problemName) const;
	void readSolution(std::string problemName);
	
	double value(unsigned int id)
	{	
		return m_primalVariables[m_id2Var[id]];
	}
	double objective() const
	{	
		return m_objective;
	}
	
private:
	std::vector<double> m_coefficients;
	std::vector<unsigned int> m_associations;
	std::vector<sparseTerm> m_constraintMatrix;
	std::vector<double> m_primalVariables;
	// mosek variables must start from 0 and be contiguous this map
	// goes between the external identifiers and internal variables
	std::map<unsigned int, unsigned int> m_id2Var;
	double m_objective;
	unsigned int m_currentVar;
};

}

#endif /*MOSEK_H_*/
