// Functions used in the process of communicating via files

#ifndef IO_H_
#define IO_H_

#include <string>
#include <map>
#include "cell.h"

namespace gateSizing {
	class gateSizer;
}
class vector;

namespace IO {

	// read in the file specified by bdnetFile and populate the given gateSizer net and cell information
	void constructNetlist(gateSizing::gateSizer& gs, const std::string& bdnetFile);
	// populate the map with hard-coded (string, cellType) key-value pairs
	void prepareCellTypeMap(std::map<const std::string, gateSizing::cellTypes>& cellTypeMap);
	void extractPinTypeAndNetName(const std::string& line, std::string& pinType, std::string& netName);
	void extractInstanceType(const std::string& line, std::string& instanceType);
	// populate net driver/load and cell output/input
	void fillInNetAndCell(gateSizing::net* n, gateSizing::cell* c, bool isOutput);
	
	inline void removeQuotes(std::string& str)
	{	
		str = str.substr(1, str.find_last_of('\"') - 1);
	}
	
	void plotResults(std::string problemName, const std::vector<double>& areaFactors);

}
#endif /*IO_H_*/
