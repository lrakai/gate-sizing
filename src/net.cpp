#include "net.h"
#include <iostream>
#include "cell.h"

gateSizing::net::~net()
{
}

void gateSizing::net::print() const
{	
	std::cout << "Net name: " << m_name << std::endl;
	std::cout << "Driver: " << m_driver->identifier() << ':' << m_driver->cellTypeString() << std::endl;
	std::cout << "Loads: ";
	for(std::vector<gateSizing::cell*>::const_iterator vIt = m_loads.begin(); vIt != m_loads.end(); vIt++)
		std::cout << (*vIt)->identifier() << ':' << (*vIt)->cellTypeString() << ' ';
	std::cout << std::endl;
}