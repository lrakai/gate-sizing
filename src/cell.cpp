#include "cell.h"
#include <vector>
#include <iostream>
#include "net.h"
#include "posynomial.h"
#include "cellLibrary.h"

gateSizing::cell::cell(const gateSizing::cellTypes cellType, const unsigned int identifier)
:m_type(cellType), m_id(identifier), m_scaleFactor(1.), m_output(NULL)
{	
	m_inputs.reserve(10);
}

gateSizing::cell::~cell()
{
}

void gateSizing::cell::populateDescendants()
{	
	if(m_output)
		for(std::vector<gateSizing::cell*>::const_iterator vIt = m_output->loads().begin(); vIt != m_output->loads().end(); ++vIt)
			m_descendants.push_back((*vIt));
}

void gateSizing::cell::populateAncestors()
{	
	for(std::vector<gateSizing::net*>::iterator vIt = m_inputs.begin(); vIt != m_inputs.end(); ++vIt)
		m_ancestors.push_back((*vIt)->driver()->identifier());
}

// cell delay is the output resistance * sum of load capacitances
void gateSizing::cell::populateDelay(const gateSizing::cellLibrary& cellLib)
{	
	if(m_output && m_inputs.size()) // skip primary inputs and  outputs
	{	
		gateSizing::monomial outputResistance(cellLib[m_type].gamma, m_id, -1);
		for(std::vector<gateSizing::cell*>::iterator vIt = m_descendants.begin(); vIt != m_descendants.end(); ++vIt)
		{
			gateSizing::monomial intrinsicCap(cellLib[(*vIt)->m_type].alpha);
			if((*vIt)->m_type != gateSizing::PRIMARYOUT)
			{	
				gateSizing::monomial perUnitCap(cellLib[(*vIt)->m_type].beta, (*vIt)->m_id, 1);
				m_delay += perUnitCap + intrinsicCap;
			}
			else // primary outputs have only intrinsic capacitance
				m_delay += intrinsicCap;
		}
		m_delay *= outputResistance;
	}
}

void gateSizing::cell::print() const
{	
	std::cout << "Cell type: " << cellTypeString() << " id: " << m_id << std::endl;
	if(m_inputs.size())
	{	
		std::cout << "Inputs: ";
		for(std::vector<gateSizing::net*>::const_iterator vIt = m_inputs.begin(); vIt != m_inputs.end(); vIt++)
			std::cout << (*vIt)->name() << ' ';
		std::cout << std::endl;
	}
	if(m_output)
		std::cout << "Output: " << m_output->name() << std::endl;
	
	if(m_ancestors.size())
	{	
		std::cout << "Ancestors: ";
		for(std::vector<unsigned int>::const_iterator vIt = m_ancestors.begin(); vIt != m_ancestors.end(); ++vIt)
			std::cout << *vIt << ' ';
		std::cout << std::endl;
	}
	if(m_descendants.size())
	{	
		std::cout << "Descendants: ";
		for(std::vector<cell*>::const_iterator vIt = m_descendants.begin(); vIt != m_descendants.end(); ++vIt)
			std::cout << (*vIt)->m_id << ' ';
		std::cout << std::endl;
	}
	if(!m_delay.isEmpty())
	{	
		std::cout << "Delay = ";
		m_delay.print();
	}
}

const char* gateSizing::cell::cellTypeString() const
{	
	switch(m_type)
	{	
		case gateSizing::PRIMARYIN: return "primaryInput";
		case gateSizing::PRIMARYOUT: return "primaryOutput";
		case gateSizing::INV: return "inv";
		case gateSizing::NAND2: return "nand2";
		case gateSizing::NAND3: return "nand3";
		case gateSizing::NAND4: return "nand4";
		case gateSizing::NOR2: return "nor2";
		case gateSizing::NOR3: return "nor3";
		case gateSizing::NOR4: return "nor4";
		case gateSizing::AND2: return "and2";
		case gateSizing::OR2: return "or2";
		case gateSizing::XOR: return "xor";
		case gateSizing::XNOR: return "xnor";
		case gateSizing::AOI21: return "aoi21";
		case gateSizing::AOI22: return "aoi22";
		case gateSizing::OAI21: return "oai21";
		case gateSizing::OAI22: return "oai22";
	}
	return "invalidType";
}