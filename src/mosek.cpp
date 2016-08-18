#include "mosek.h"
#include <string>
#include <cstdlib>
#include <cmath>
#include <limits>
#include <fstream>
#include <iostream>
#include <sstream>

void gateSizing::mosek::solve(const std::string& problemName)
{	
	writeProblem(problemName);
	std::string command = "mskexpopt ";
	command += problemName + ".eo";
	int returnValue = system(command.c_str());
	if(returnValue)
	{	
		std::cerr << "Call to mskexpopt returned an error. (" << returnValue << ")" << std::endl;
		exit(EXIT_FAILURE);
	}
	readSolution(problemName);
}

void gateSizing::mosek::writeProblem(std::string problemName) const
{	
	problemName+=".eo";
	std::ofstream problemFile(problemName.c_str());
	if(!problemFile)
	{	
		std::cerr << problemName << " at " << __LINE__ << " could not be opened for output.\n";
		exit(EXIT_FAILURE);
	}
	
	problemFile.precision(std::numeric_limits<double>::digits10);
	problemFile << m_associations[m_associations.size() - 1] + 1 << " * numConstraints" << std::endl;
	problemFile << m_id2Var.size() << " * numVariables" << std::endl;
	problemFile << m_coefficients.size() << " * numTerms" << std::endl;
	problemFile << "* Coefficients of terms" << std::endl;
	for(std::vector<double>::const_iterator vIt = m_coefficients.begin(); vIt != m_coefficients.end(); vIt++)
		problemFile << *vIt << std::endl;
	problemFile << "* Constraints each term belongs to" << std::endl;
	for(std::vector<unsigned int>::const_iterator vIt = m_associations.begin(); vIt != m_associations.end(); vIt++)
		problemFile << *vIt << std::endl;
	problemFile << "* Section defining a_kj\n* Format: term var exp" << std::endl;
	for(std::vector<sparseTerm>::const_iterator vIt = m_constraintMatrix.begin(); vIt != m_constraintMatrix.end(); vIt++)
		problemFile << (*vIt).term << ' ' << (*vIt).variable << ' ' << (*vIt).exponent << std::endl;
	
	problemFile.close();
}

void gateSizing::mosek::readSolution(std::string problemName)
{	std::string line, str1;
	std::istringstream strline;

	if(problemName.find_first_of('.') != std::string::npos)
		problemName = problemName.substr(0, problemName.find_first_of('.'));
	problemName = problemName + ".sol";
	std::ifstream solutionFile(problemName.c_str());
	if(!solutionFile) 
	{	
		std::cerr << problemName << " at " << __LINE__ << " could not be opened for input.\n";
		exit(EXIT_FAILURE);
	}

	while (getline(solutionFile, line)) 
	{	
		if(line.find("INFEASIBLE") != std::string::npos) 
		{	
			std::cerr << "GP is infeasible.  Terminating..." << std::endl;
			exit(EXIT_FAILURE);
		} 
		else if(line.find("OBJECTIVE") != std::string::npos) 
		{	
			strline.str(line);
			strline >> str1 >> str1 >> m_objective;
			strline.clear();
			break;
		}
	}

	while (getline(solutionFile, line)) 
	{	
		if (line.find("PRIMAL VARIABLES") != std::string::npos) 
		{	
			getline(solutionFile, line);
			break;
		}
	}
	
	const unsigned int numVariables = m_id2Var.size();
	m_primalVariables.resize(numVariables);
	for (unsigned int variable = 0; variable < numVariables; variable++) 
	{	
		double value;
		getline(solutionFile, line);
		strline.str(line);
		strline >> str1 >> value;
		m_primalVariables[variable] = exp(value);
	}

	solutionFile.close();
}	