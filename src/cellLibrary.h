// class cellLibary stores parameters related to each cellType including
// area (a), intrinsic capacitance (alpha), per unit capacitance (beta), driving 
// resistance factor (gamma), transition energy (e), transition frequency (f)

#ifndef CELLLIBRARY_H_
#define CELLLIBRARY_H_

#include <vector>
#include "cell.h"

namespace gateSizing {

class cellLibrary
{	
public:
	cellLibrary()
	{	
		populateElements();
	}
	
	// libraryElement contains all the parameters for a cell
	struct libraryElement
	{	
		libraryElement(double a, double alpha, double beta, double gamma, double e, double f)
		:a(a), alpha(alpha), beta(beta), gamma(gamma), e(e), f(f)
		{}
		
		double a, alpha, beta, gamma, e, f;
	};
	
	const libraryElement& operator[](const cellTypes& c) const
	{	
		return m_elements[c];
	}
	
private:	
	// populate elements with hard-coded values for each cell type
	void populateElements();
	std::vector<libraryElement> m_elements;
};

}

#endif /*CELLLIBRARY_H_*/
