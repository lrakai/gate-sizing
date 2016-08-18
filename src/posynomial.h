// class monomial is a coefficient and a (cell id, exponent) map
// class posynomial is a vector of monomials, 

#ifndef POSYNOMIAL_H_
#define POSYNOMIAL_H_

#include <vector>
#include <map>

namespace gateSizing {

class posynomial;

class monomial
{
public:
	monomial()
	:m_coefficient(0)
	{}

	monomial(double c)
	:m_coefficient(c)
	{}

	monomial(double c, unsigned int index, int e)
	:m_coefficient(c)
	{
		m_exponents[index] = e;
	}
	
	bool sameVariables(const monomial& m);
	
	inline double coefficient() const
	{	
		return m_coefficient;
	}
	
	inline void incrementCoefficient(const double c)
	{	
		m_coefficient += c;		
	}

	posynomial operator+(const monomial& m);
	monomial& operator*=(const monomial& m);
	
	inline const std::map<unsigned int, int>& exponents() const
	{	
		return m_exponents;
	}
	
	void print() const;
	
private:
	double m_coefficient;
	std::map<unsigned int, int> m_exponents;
};

class posynomial
{
public:
	posynomial()
	{	
		m_monomials.reserve(10);
	}
	
	inline void addMonomial(const monomial& m)
	{	
		m_monomials.push_back(m);
	}

	inline void addMonomial(monomial& m)
	{	
		m_monomials.push_back(m);
	}

	inline const std::vector<monomial>& monomials() const
	{	
		return m_monomials;
	}
	
	posynomial& operator+=(const monomial& m);
	posynomial& operator*=(const monomial& m);
	posynomial& operator+=(const posynomial& p);
	
	inline bool isEmpty() const
	{	
		return m_monomials.size()==0;
	}
	
	void print() const;
	
private:
	std::vector<monomial> m_monomials;
};

}

#endif /*POSYNOMIAL_H_*/
