// class gateSizer is the main class for solving a gate sizing problem

#ifndef GATESIZER_H_
#define GATESIZER_H_

#include <vector>
#include <string>
#include <set>
#include "net.h"
#include "cell.h"
#include "cellLibrary.h"
#include "mosek.h"

namespace gateSizing {

class posynomial;
class monomial;

class gateSizer
{
public:
	// problem is read in, formulation is written, formulation is solved, 
	// solution is read in the constructor 
	// powerFactor and areaFactor are numbers >= 1.0 and scale their
	// respective limit away from the minimum
	// i.e. Pmax = powerFactor * Pmin; 
	gateSizer(const std::string& problemName, double powerFactor = 2.0, double areaFactor = 2.0);
	virtual ~gateSizer();
	
	cell* addCell(cellTypes cellType);
	net* addNet(std::string& name, cell* driver);
	
	double evaluatePower() const;
	double evaluateArea() const;
	void reportResults(std::string resultFile, bool append, double powerFactor = 2.0, double areaFactor = 2.0) const;
	void plotResults(std::string problemName, const std::vector<double>& areaFactors);
	void printNetList() const;
	
private:
	void populateDescendants();
	void populateAncestors();
	void populateDelays();
	void populateFormulation(double powerFactor, double areaFactor);
	void populateScaleFactors();
	
	// add the given monomial to the formulation
	void writeMonomialTerms(const monomial& p, unsigned int& term, unsigned int constraint);
	// add the given posynomial to the formulation
	void writePosynomialTerms(const posynomial& p, unsigned int& term, unsigned int constraint);
	// determine if the given cell is a descendant of a primaryInput or an internal cell
	// the identifier of the cell is added to the appropriate set
	// if the cell is a primaryInput or primaryOutput it does nothing
	void internalOrPIDescendant(const cell* c, std::set<unsigned int>& internals, std::set<unsigned int>& inputDescendants);
	// fill the ancestor set with ancestors of c that are not primaryInputs
	void nonPIAncestors(const cell* c, std::set<unsigned int>& ancestors);
	// of the cells whose identifiers are in ancestors, remove all
	// but the ones that exclusively drive primaryOutputs
	void exclusivePODescendants(std::set<unsigned int>& ancestors);

	// all cells (primaryInputs, primaryOutputs, and internal gates)
	std::vector<cell*> m_cells;
	std::vector<net*> m_nets;
	// primary inputs and outputs are stored for convenience 
	// (they are duplicated in the m_cells member) 
	std::vector<cell*> m_primaryInputs;
	std::vector<cell*> m_primaryOutputs;
	// cellLibrary stores parameters for each cellType
	cellLibrary cellLib;
	mosek formulation;
};

}

#endif /*GATESIZER_H_*/
