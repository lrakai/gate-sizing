// class cell stores various information relating to cells/instances in the design 
// including inputs, output, and scale factor for the gate sizing problem formulation

#ifndef CELL_H_
#define CELL_H_

#include <vector>
#include "posynomial.h"

namespace gateSizing {

class net;
class cellLibrary;

enum cellTypes { PRIMARYIN, PRIMARYOUT, INV, NAND2, NAND3, NAND4, NOR2, NOR3,  
	             NOR4, AND2, OR2, XOR, XNOR, AOI21, AOI22, OAI21, OAI22 };

class cell
{
public:
	cell(const cellTypes cellType, const unsigned int identifier);
	virtual ~cell();
	
	inline void assignOutput(net* outputNet)
	{	
		m_output = outputNet;
	}

	inline void addInput(net* inputNet)
	{	
		m_inputs.push_back(inputNet);
	}

	inline void assignScaleFactor(double s)
	{	
		m_scaleFactor = s;
	}

	void populateDescendants();
	void populateAncestors();
	void populateDelay(const cellLibrary& cellLib);
	
	void print() const;
	const char* cellTypeString() const;
	inline unsigned int identifier() const
	{ 
		return m_id;
	}

	inline enum cellTypes type() const
	{ 
		return m_type;
	}

	inline const posynomial& delay() const
	{	
		return m_delay;
	}
	
	inline const std::vector<unsigned int>& ancestors() const
	{	
		return m_ancestors;
	}

	inline const std::vector<cell*>& descendants() const
	{	
		return m_descendants;
	}

	inline double scaleFactor() const
	{	
		return m_scaleFactor;
	}
	
private:
	const enum cellTypes m_type;
	const unsigned int m_id;
	double m_scaleFactor;
	posynomial m_delay;
	net* m_output;
	std::vector<net*> m_inputs;
	// first level upstream neighbor cell ids
	std::vector<unsigned int> m_ancestors;
	// first level downstream neighbor cell ids
	std::vector<cell*> m_descendants;
};

}

#endif /*CELL_H_*/
