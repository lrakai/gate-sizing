#include "io.h"
#include <map>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include "net.h"
#include "cell.h"
#include "gateSizer.h"

// Parse the bdnet file filling in cell and net structures as it progresses
// Requires bdnetFile to be the name of a valid bdnet input file and
//          assumes the netlist is well-formed (connected, valid cell types, etc.)
void IO::constructNetlist(gateSizing::gateSizer& gs, const std::string& bdnetFile)
{	
	std::map<const std::string, gateSizing::net*> netMap;
	std::map<const std::string, gateSizing::cellTypes> cellTypeMap;
	std::string line, instanceType, pinType, netName;
	
	IO::prepareCellTypeMap(cellTypeMap);
	
	std::ifstream bdnetStream(bdnetFile.c_str());
	while (getline(bdnetStream, line)) 
	{	
		if (line.find("INPUT") != std::string::npos) 
		{	
			while (getline(bdnetStream, line)) 
			{	
				if (line.find("\"") == std::string::npos)
					break;
				IO::extractPinTypeAndNetName(line, pinType, netName);
				gateSizing::cell* c = gs.addCell(gateSizing::PRIMARYIN);
				gateSizing::net* n = gs.addNet(netName, c);
				netMap[netName] = n;
				IO::fillInNetAndCell(n,c,true);
			}
		}
		if (line.find("OUTPUT") != std::string::npos) 
		{	
			while (getline(bdnetStream, line)) 
			{	
				if (line.find("\"") == std::string::npos)
					break;
				IO::extractPinTypeAndNetName(line, pinType, netName);
				gateSizing::cell* c = gs.addCell(gateSizing::PRIMARYOUT);
				gateSizing::net* n;
				if(netMap.find(netName) == netMap.end())
				{	
					n = gs.addNet(netName, NULL);
					netMap[netName] = n;
				}
				else // net already exists (primary input is a primary output) 
				{	
					n = netMap[netName];
				}
				IO::fillInNetAndCell(n,c,false);
			}
		}
		if (line.find("INSTANCE") != std::string::npos) 
		{	
			IO::extractInstanceType(line, instanceType);
			gateSizing::cell* c = gs.addCell(cellTypeMap[instanceType]);
			while (getline(bdnetStream, line)) 
			{	
				if (line.find("\"") == std::string::npos)
					break;
				IO::extractPinTypeAndNetName(line, pinType, netName);
				bool isOutput = false;
				if(pinType == "O")
					isOutput = true;
				if(netMap.find(netName) == netMap.end())
				{	
					gateSizing::net* n = gs.addNet(netName, NULL);
					netMap[netName] = n;
					IO::fillInNetAndCell(n,c,isOutput);
				}
				else // net already exists (primary input is a primary output) 
				{	
					gateSizing::net* n = netMap[netName];
					IO::fillInNetAndCell(n,c,isOutput);
				}
			}
		}
	}

	bdnetStream.close();
}

void IO::prepareCellTypeMap(std::map<const std::string, gateSizing::cellTypes>& cellTypeMap)
{	
	cellTypeMap["inv"] = gateSizing::INV;
	cellTypeMap["nand2"] = gateSizing::NAND2;
	cellTypeMap["nand3"] = gateSizing::NAND3;
	cellTypeMap["nand4"] = gateSizing::NAND4;
	cellTypeMap["nor2"] = gateSizing::NOR2;
	cellTypeMap["nor3"] = gateSizing::NOR3;
	cellTypeMap["nor4"] = gateSizing::NOR4;
	cellTypeMap["and2"] = gateSizing::AND2;
	cellTypeMap["or2"] = gateSizing::OR2;
	cellTypeMap["xor"] = gateSizing::XOR;
	cellTypeMap["xnor"] = gateSizing::XNOR;
	cellTypeMap["aoi21"] = gateSizing::AOI21;
	cellTypeMap["aoi22"] = gateSizing::AOI22;
	cellTypeMap["oai21"] = gateSizing::OAI21;
	cellTypeMap["oai22"] = gateSizing::OAI22;
}

void IO::extractPinTypeAndNetName(const std::string& line, std::string& pinType, std::string& netName)
{	
	std::istringstream isstr(line);
	isstr >> pinType >> netName >> netName;
	removeQuotes(pinType);
	removeQuotes(netName);
}

void IO::extractInstanceType(const std::string& line, std::string& instanceType)
{	
	std::istringstream isstr(line);
	isstr >> instanceType >> instanceType;
	instanceType = instanceType.substr(1, instanceType.find_first_of(':')-2);
}

void IO::fillInNetAndCell(gateSizing::net* n, gateSizing::cell* c, bool isOutput)
{	
	if(isOutput)
	{	
		n->assignDriver(c);
		c->assignOutput(n);
	}
	else
	{	
		n->addLoad(c);
		c->addInput(n);
	}
}

void IO::plotResults(std::string problemName, const std::vector<double>& areaFactors)
{	
	std::ofstream plotStream((problemName + ".plt").c_str(), std::ofstream::out);
	plotStream << "set terminal png enhanced size 1280, 768" << std::endl;
	plotStream << "set output '" << problemName << ".png'" << std::endl;
	plotStream << "set lmargin 12" << std::endl;
	plotStream << "set size 1,1" << std::endl;
	plotStream << "set origin 0,0" << std::endl;
	plotStream << "set multiplot" << std::endl;
	plotStream << "set size 1,0.334" << std::endl;
	plotStream << "set origin 0,0.666" << std::endl;
	plotStream << "set key left center" << std::endl;
	plotStream << "set title 'Delay Plot'" << std::endl;
	plotStream << "set ylabel \"Delay (ns)\"" << std::endl;
	plotStream << "set xlabel \"Pmax\"" << std::endl;
	plotStream << "plot ";
	for(unsigned int a = 0; a < areaFactors.size(); a++)
	{	
		double areaFactor = areaFactors[a];
		std::ostringstream oss;
		oss << areaFactor;
		std::string resultName = problemName + "af" + oss.str() + ".res";
		plotStream << "\"" << resultName << "\" using 2:3 with lines title \"areaFactor " << areaFactor << "\"";
		if(a != areaFactors.size() - 1)
			plotStream << ", \\";
		plotStream << std::endl;
	}
	plotStream << "set size 1,0.333" << std::endl;
	plotStream << "set origin 0,0.333" << std::endl;
	plotStream << "set key left center" << std::endl;
	plotStream << "set title 'Power Plot'" << std::endl;
	plotStream << "set ylabel \"Power (uW)\"" << std::endl;
	plotStream << "set xlabel \"Pmax\"" << std::endl;
	plotStream << "plot ";
	for(unsigned int a = 0; a < areaFactors.size(); a++)
	{	
		double areaFactor = areaFactors[a];
		std::ostringstream oss;
		oss << areaFactor;
		std::string resultName = problemName + "af" + oss.str() + ".res";
		plotStream << "\"" << resultName << "\" using 2:4 with lines title \"areaFactor " << areaFactor << "\"";
		if(a != areaFactors.size() - 1)
			plotStream << ", \\";
		plotStream << std::endl;
	}
	plotStream << "set size 1,0.333" << std::endl;
	plotStream << "set origin 0,0" << std::endl;
	plotStream << "set key left center" << std::endl;
	plotStream << "set title 'Area Plot'" << std::endl;
	plotStream << "set ylabel \"Area (nm*nm)\"" << std::endl;
	plotStream << "set xlabel \"Pmax\"" << std::endl;
	plotStream << "plot ";
	for(unsigned int a = 0; a < areaFactors.size(); a++)
	{	
		double areaFactor = areaFactors[a];
		std::ostringstream oss;
		oss << areaFactor;
		std::string resultName = problemName + "af" + oss.str() + ".res";
		plotStream << "\"" << resultName << "\" using 2:5 with lines title \"areaFactor " << areaFactor << "\"";
		if(a != areaFactors.size() - 1)
			plotStream << ", \\";
		plotStream << std::endl;
	}
	plotStream << "unset multiplot" << std::endl;
	
	plotStream.close();
	
	std::string command;
	command = "gnuplot " + problemName + ".plt; display " + problemName + ".png &";
	system(command.c_str());
}