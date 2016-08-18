// Main function to test the gateSizer class
// Basic command line argument checking is performed

#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <vector>
#include "gateSizer.h"
#include "io.h"

int reportUsage(const char** argv)
{	
	std::cerr << "  Usage: " << argv[0] << " problemName" << std::endl;
	std::cerr << "Example: " << argv[0] << " benchmarks/comb/b1" << std::endl;
	return EXIT_FAILURE;
}

int main(int argc, const char** argv)
{	
	if(argc != 2)
		return reportUsage(argv);

	const std::string problemName(argv[1]);
	// test if file can be opened
	std::ifstream bdnetStream((problemName + ".bdnet").c_str());
	if(bdnetStream)
	{	
		bdnetStream.close();
		
		// an areaFactor sets Amax = areaFactor * Amin
		// where Amin is obtained when all gates have unit scaling
		std::vector<double> areaFactors;
		const double areaFactorIncrement = 0.5;
		const double areaFactorMax = 3.; 
		for(double areaFactor = 1.5; areaFactor < areaFactorMax; areaFactor += areaFactorIncrement)
			areaFactors.push_back(areaFactor);

		// a powerFactor sets Pmax = powerFactor * Pmin
		std::vector<double> powerFactors;
		const double powerFactorIncrement = 0.1;
		const double powerFactorMax = 3.5;
		for(double powerFactor = 1.0; powerFactor < powerFactorMax; powerFactor += powerFactorIncrement)
			powerFactors.push_back(powerFactor);
		
		for(unsigned int a = 0; a < areaFactors.size(); a++)
		{	
			for(unsigned int p = 0; p < powerFactors.size(); p++)
			{	
				gateSizing::gateSizer sizingProblem(problemName, powerFactors[p], areaFactors[a]);
				sizingProblem.reportResults(problemName, p, powerFactors[p], areaFactors[a]);
			}
		}
		IO::plotResults(problemName, areaFactors);
		return EXIT_SUCCESS;
	}
	else
		return reportUsage(argv);
}
