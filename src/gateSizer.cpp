#include "gateSizer.h"
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>
#include "io.h"
#include "cell.h"
#include "net.h"
#include "posynomial.h"


gateSizing::gateSizer::gateSizer(const std::string& problemName, double powerFactor, double areaFactor)
{	
	IO::constructNetlist(*this, problemName + ".bdnet");
	populateDescendants();
	populateDelays();
	populateAncestors();
	populateFormulation(powerFactor, areaFactor);
	formulation.solve(problemName);
	populateScaleFactors();
}

gateSizing::gateSizer::~gateSizer()
{	
	for(std::vector<gateSizing::cell*>::iterator vIt = m_cells.begin(); vIt != m_cells.end(); ++vIt)
		delete *vIt;
	for(std::vector<gateSizing::net*>::iterator vIt = m_nets.begin(); vIt != m_nets.end(); ++vIt)
		delete *vIt;
}

gateSizing::cell* gateSizing::gateSizer::addCell(gateSizing::cellTypes cellType)
{	
	unsigned int identifier = m_cells.size();
	m_cells.push_back(new gateSizing::cell(cellType, identifier));
	gateSizing::cell* c = m_cells[identifier];
	if(cellType == gateSizing::PRIMARYIN)
		m_primaryInputs.push_back(c);
	if(cellType == gateSizing::PRIMARYOUT)
		m_primaryOutputs.push_back(c);
	return c;
}

gateSizing::net* gateSizing::gateSizer::addNet(std::string& name, gateSizing::cell* driver)
{	
	m_nets.push_back(new gateSizing::net(name, driver));
	return m_nets[m_nets.size()-1];
}

double gateSizing::gateSizer::evaluatePower() const
{	
	double power = 0.;
	for(std::vector<gateSizing::cell*>::const_iterator vIt = m_cells.begin(); vIt != m_cells.end(); vIt++)
		power += (*vIt)->scaleFactor() * cellLib[(*vIt)->type()].e * cellLib[(*vIt)->type()].f;
	return power;
}

double gateSizing::gateSizer::evaluateArea() const
{	
	double area = 0.;
	for(std::vector<gateSizing::cell*>::const_iterator vIt = m_cells.begin(); vIt != m_cells.end(); vIt++)
		area += (*vIt)->scaleFactor() * cellLib[(*vIt)->type()].a;
	return area;
}

void gateSizing::gateSizer::reportResults(std::string problemName, bool append, double powerFactor, double areaFactor) const
{	
	double delay = formulation.objective();
	double power = evaluatePower();
	double area = evaluateArea();
	
	std::cout << "areaFactor: " << areaFactor << "\tpowerFactor: " << powerFactor << "\tDelay: " << delay << 
	             "ns\tPower: " << power << "uW\tArea: " << area << "nm^2" << std::endl;
	
	std::ostringstream oss;
	oss << areaFactor;
	problemName += "af" + oss.str() + ".res";
	std::ofstream resultStream;
	if(append)
		resultStream.open(problemName.c_str(), std::ofstream::app);
	else
		resultStream.open(problemName.c_str(), std::ofstream::out);
	resultStream << areaFactor << '\t' << powerFactor << '\t' << delay << '\t' << power << '\t' << area << std::endl;
	resultStream.close();
}

void gateSizing::gateSizer::printNetList() const
{	std::cout << "Printing nets" << std::endl;
	for(std::vector<gateSizing::net*>::const_iterator vIt = m_nets.begin(); vIt != m_nets.end(); vIt++)
		(*vIt)->print();
	
	std::cout << "Printing cells" << std::endl;
	for(std::vector<gateSizing::cell*>::const_iterator vIt = m_cells.begin(); vIt != m_cells.end(); vIt++)
		(*vIt)->print();
}

// private functions

void gateSizing::gateSizer::populateDescendants()
{	
	for(std::vector<gateSizing::cell*>::iterator vIt = m_cells.begin(); vIt != m_cells.end(); ++vIt)
		(*vIt)->populateDescendants();
}

void gateSizing::gateSizer::populateAncestors()
{	
	for(std::vector<gateSizing::cell*>::iterator vIt = m_cells.begin(); vIt != m_cells.end(); ++vIt)
		(*vIt)->populateAncestors();
}

void gateSizing::gateSizer::populateDelays()
{	
	for(std::vector<gateSizing::cell*>::iterator vIt = m_cells.begin(); vIt != m_cells.end(); ++vIt)
		(*vIt)->populateDelay(cellLib);
}

// The variables for the formulation are the scaleFactors x_i,
// the path delay to a cell T_i, and additional variables to 
// remove the max{T_j1,...,T_jk} for each cell called V_i.
// One last variable is added for the objective D (max of all) 
// As an example of how to remove a max operator, consider
// the formulation 
// minimize z
// s.t. max{y1, y2} <= z
// It can be equivalently reformulated as
// minimize z 
// s.t. w <= z
// y1 <= w
// y2 <= w
// The variables are indexed as follows
// [0..numCells-1] are the corresponding x_i variable
// [numCells..2*numCells-1] are the corresponding T_i variable
// [2*numCells..3*numCells-1] are the corresponding V_i variable
// 3*numCells is the objective variable D
// 
// The input powerFactor is used to assign Pmax = powerFactor * Pmin
// where Pmin is the power with all gates at minimum allowed size
// the input areaFactor is used to assign Amax = areaFactor * Amin
// where Amin is the area when all gates are minimum size
void gateSizing::gateSizer::populateFormulation(double powerFactor, double areaFactor)
{	
	const unsigned int numCells = m_cells.size();
	const unsigned int D = 3*numCells;
	unsigned int constraint = 0; // objective is constraint 0
	unsigned int term = 0;
	
	// objective
	formulation.addTerm(term++, D, 1);
	formulation.addCoefficient(1.);
	formulation.addAssociation(constraint++);
	
	// delay constraints
	// all inputs are primaryInputs
	std::set<unsigned int> inputDescendants;
	// not all inputs are primaryInputs
	std::set<unsigned int> internals;
	for(std::vector<gateSizing::cell*>::iterator vIt = m_cells.begin(); vIt != m_cells.end(); vIt++)
		internalOrPIDescendant(*vIt, internals, inputDescendants);
	
	// inputDescendants D_i <= T_i
	for(std::set<unsigned int>::iterator sIt = inputDescendants.begin(); sIt != inputDescendants.end(); sIt++)
	{	
		gateSizing::posynomial p = m_cells[*sIt]->delay(); // D_i
		gateSizing::monomial m(1., numCells+(*sIt), -1); // T_i^-1
		p *= m; // D_i * T_i^-1
		writePosynomialTerms(p, term, constraint);
		constraint++;
	}
	
	// internals D_i + V_i <= T_i 
	for(std::set<unsigned int>::iterator sIt = internals.begin(); sIt != internals.end(); sIt++)
	{	
		gateSizing::posynomial p = m_cells[*sIt]->delay(); // D_i
		gateSizing::monomial m(1., numCells+(*sIt), -1); // T_i^-1
		gateSizing::monomial v(1., 2*numCells+(*sIt), 1); // V_i
		p += v; // D_i + V_i
		p *= m; // (D_i + V_i) * T_i^-1
		writePosynomialTerms(p, term, constraint);
		constraint++;
	}
	
	// internals T_ancestor <= V_i 
	for(std::set<unsigned int>::iterator sIt = internals.begin(); sIt != internals.end(); sIt++)
	{	
		std::set<unsigned int> internalAncestors;
		nonPIAncestors(m_cells[*sIt], internalAncestors);
		for(std::set<unsigned int>::iterator aIt = internalAncestors.begin(); aIt != internalAncestors.end(); aIt++)
		{	formulation.addTerm(term, numCells+(*aIt), 1); // T_ancestor
			formulation.addTerm(term++, 2*numCells+(*sIt), -1); // V_i^-1
			formulation.addCoefficient(1.);
			formulation.addAssociation(constraint++);
		}
	}
	
	// delay constraints for primaryOutputs (T_i <= D)
	// ids of non-primaryInput ancestors of primaryOutputs 
	std::set<unsigned int> outputAncestors;
	for(std::vector<gateSizing::cell*>::iterator vIt = m_primaryOutputs.begin(); vIt != m_primaryOutputs.end(); vIt++)
		nonPIAncestors((*vIt), outputAncestors);
	// find cells that output only to primaryOutputs
	exclusivePODescendants(outputAncestors);
	// T_i <= D
	for(std::set<unsigned int>::iterator sIt = outputAncestors.begin(); sIt != outputAncestors.end(); sIt++)
	{	
		formulation.addTerm(term, numCells+(*sIt), 1); // T_i
		formulation.addTerm(term++, D, -1); // D^-1
		formulation.addCoefficient(1.);
		formulation.addAssociation(constraint++);
	}
	
	// Power constraint P <= P_max
	const double Pmin = evaluatePower(); // minimum power initially
	const double Pmax = powerFactor * Pmin;
	for(std::vector<gateSizing::cell*>::iterator vIt = m_cells.begin(); vIt != m_cells.end(); vIt++)
	{	
		gateSizing::cellTypes type = (*vIt)->type();
		if(type != gateSizing::PRIMARYIN && type != gateSizing::PRIMARYOUT)
		{	
			formulation.addTerm(term++, (*vIt)->identifier(), 1);
			formulation.addCoefficient(cellLib[type].f * cellLib[type].e / Pmax);
			formulation.addAssociation(constraint);
		}
	}
	constraint++;
	
	// Area constraint A <= A_max
	const double Amin = evaluateArea(); // minimum area initially
	const double Amax = areaFactor * Amin;
	for(std::vector<gateSizing::cell*>::iterator vIt = m_cells.begin(); vIt != m_cells.end(); vIt++)
	{	
		gateSizing::cellTypes type = (*vIt)->type();
		if(type != gateSizing::PRIMARYIN && type != gateSizing::PRIMARYOUT)
		{	
			formulation.addTerm(term++, (*vIt)->identifier(), 1);
			formulation.addCoefficient(cellLib[type].a / Amax);
			formulation.addAssociation(constraint);
		}
	}
	constraint++;
	
	// scaleFactor constraint 1 <= x_i
	for(std::vector<gateSizing::cell*>::iterator vIt = m_cells.begin(); vIt != m_cells.end(); vIt++)
	{	
		gateSizing::cellTypes type = (*vIt)->type();
		if(type != gateSizing::PRIMARYIN && type != gateSizing::PRIMARYOUT)
		{	
			formulation.addTerm(term++, (*vIt)->identifier(), -1);
			formulation.addCoefficient(1.);
			formulation.addAssociation(constraint++);
		}
	}
}

void gateSizing::gateSizer::populateScaleFactors()
{	
	for(std::vector<gateSizing::cell*>::iterator vIt = m_cells.begin(); vIt != m_cells.end(); vIt++)
	{	
		if((*vIt)->type() != gateSizing::PRIMARYIN && (*vIt)->type() != gateSizing::PRIMARYOUT)
		{	
			(*vIt)->assignScaleFactor(formulation.value((*vIt)->identifier()));
		}
	}
}

void gateSizing::gateSizer::writeMonomialTerms(const monomial& m, unsigned int& term, unsigned int constraint)
{	
	const std::map<unsigned int, int>& map = m.exponents();
	for(std::map<unsigned int, int>::const_iterator mIt = map.begin(); mIt != map.end(); mIt++)
		formulation.addTerm(term, (*mIt).first, (*mIt).second); // T_i
	formulation.addCoefficient(m.coefficient());
	formulation.addAssociation(constraint);
	term++;
}

void gateSizing::gateSizer::writePosynomialTerms(const posynomial& p, unsigned int& term, unsigned int constraint)
{	
	for(std::vector<gateSizing::monomial>::const_iterator vIt = p.monomials().begin(); vIt != p.monomials().end(); vIt++)
		writeMonomialTerms(*vIt, term, constraint);
}

void gateSizing::gateSizer::internalOrPIDescendant(const gateSizing::cell* c, std::set<unsigned int>& internals, std::set<unsigned int>& inputDescendants)
{	
	if(c->type() != gateSizing::PRIMARYIN && c->type() != gateSizing::PRIMARYOUT)
	{	
		std::set<unsigned int> internalAncestors;
		nonPIAncestors(c, internalAncestors);
		if(internalAncestors.size())
			internals.insert(c->identifier());
		else
			inputDescendants.insert(c->identifier());
	}	
}

void gateSizing::gateSizer::nonPIAncestors(const gateSizing::cell* c, std::set<unsigned int>& ancestors)
{	
	for(std::vector<unsigned int>::const_iterator vIt = c->ancestors().begin(); vIt != c->ancestors().end(); vIt++)
		if(m_cells[(*vIt)]->type() != gateSizing::PRIMARYIN)
			ancestors.insert(*vIt);
}

void gateSizing::gateSizer::exclusivePODescendants(std::set<unsigned int>& ancestors)
{	
	std::set<unsigned int> exclusivePOs;
	for(std::set<unsigned int>::iterator sIt = ancestors.begin(); sIt != ancestors.end(); sIt++)
	{	
		std::vector<gateSizing::cell*> d = m_cells[*sIt]->descendants();
		bool allPOs = true;
		for(std::vector<gateSizing::cell*>::iterator vIt = d.begin(); vIt != d.end(); vIt++)
			if((*vIt)->type() != gateSizing::PRIMARYOUT)
			{	
				allPOs = false;
				break;
			}
		if(allPOs)
			exclusivePOs.insert(*sIt);
	}
	ancestors = exclusivePOs;
}