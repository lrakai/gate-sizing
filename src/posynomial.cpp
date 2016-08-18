#include "posynomial.h"
#include <vector>
#include <map>
#include <iostream>

bool gateSizing::monomial::sameVariables(const gateSizing::monomial& m)
{	
	bool sameVariables = true;
	if(m.m_exponents.size() != m_exponents.size())
	{	
		sameVariables = false;
	}
	else
	{	
		for(std::map<unsigned int, int>::const_iterator mIt = m.m_exponents.begin(); mIt != m.m_exponents.end(); mIt++)
		{	
			std::map<unsigned int, int>::const_iterator eIt = m_exponents.find(((*mIt).first));
			if(eIt == m_exponents.end() || (*eIt).second != (*mIt).second)
			{ 	
				sameVariables = false;
				break;
			}
		}		
	}	
	return sameVariables;
}

gateSizing::posynomial gateSizing::monomial::operator+(const gateSizing::monomial& m)
{	
	gateSizing::posynomial p;
	// check if same terms exist in both cases
	bool isSame = sameVariables(m);
	
	if(isSame)
	{	
		m_coefficient += m.m_coefficient;	
		p.addMonomial(*this);
	}
	else
	{	
		p.addMonomial(*this);
		p.addMonomial(m);
	}
	return p;
}

gateSizing::monomial& gateSizing::monomial::operator*=(const gateSizing::monomial& m)
{	
	m_coefficient *= m.m_coefficient;
	for(std::map<unsigned int, int>::const_iterator mIt = m.m_exponents.begin(); mIt != m.m_exponents.end(); mIt++)
	{	
		if(m_exponents.find(((*mIt).first)) == m_exponents.end())
		{ 	
			m_exponents[(*mIt).first] = (*mIt).second;
		}
		else
		{	
			m_exponents[(*mIt).first] += (*mIt).second;
		}
	}
	return *this;
}

void gateSizing::monomial::print() const
{	
	std::cout << m_coefficient;
	if(m_exponents.size())
	{	
		for(std::map<unsigned int, int>::const_iterator mIt = m_exponents.begin(); mIt != m_exponents.end(); mIt++)
		{	
			std::cout << "*(x"<< (*mIt).first << ")^" << (*mIt).second;
		}
	}
}

gateSizing::posynomial& gateSizing::posynomial::operator+=(const monomial& m)
{	
	bool isSame = false;
	for(std::vector<gateSizing::monomial>::iterator vIt = m_monomials.begin(); vIt != m_monomials.end(); vIt++)
	{	
		isSame = (*vIt).sameVariables(m);
		if(isSame)
		{	
			(*vIt).incrementCoefficient(m.coefficient());
			break;
		}
	}
	if(!isSame)
	{	
		addMonomial(m);
	}
	return *this;
}

gateSizing::posynomial& gateSizing::posynomial::operator*=(const gateSizing::monomial& m)
{	for(std::vector<gateSizing::monomial>::iterator vIt = m_monomials.begin(); vIt != m_monomials.end(); vIt++)
	{	
		(*vIt) *= m;		
	}
	return *this;
}

gateSizing::posynomial& gateSizing::posynomial::operator+=(const gateSizing::posynomial& p)
{	
	for(std::vector<gateSizing::monomial>::const_iterator vIt = p.m_monomials.begin(); vIt != p.m_monomials.end(); vIt++)
	{	
		*this += (*vIt);		
	}
	return *this;
}

void gateSizing::posynomial::print() const
{	
	if(m_monomials.size())
	{	
		std::vector<gateSizing::monomial>::const_iterator end = m_monomials.end();
		--end;
		for(std::vector<gateSizing::monomial>::const_iterator vIt = m_monomials.begin(); vIt != end; vIt++)
		{	
			(*vIt).print();
			std::cout << " + ";
		}
		(*end).print();
		std::cout << std::endl;
	}
}