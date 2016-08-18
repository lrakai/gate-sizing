#include "cellLibrary.h"

void gateSizing::cellLibrary::populateElements()
{	
	m_elements.resize(17, gateSizing::cellLibrary::libraryElement(-1, -1, -1, -1, -1, -1));
	m_elements[gateSizing::PRIMARYIN] = gateSizing::cellLibrary::libraryElement(0, 10, 0, 0, 0, 0);
	m_elements[gateSizing::PRIMARYOUT] = gateSizing::cellLibrary::libraryElement(0, 10, 0, 0, 0, 0);
	m_elements[gateSizing::INV] = gateSizing::cellLibrary::libraryElement(1, 1, 1, 1, 1, 1);
	m_elements[gateSizing::NAND2] = gateSizing::cellLibrary::libraryElement(2, 1, 1, 1, 2, 0.7);
	m_elements[gateSizing::NAND3] = gateSizing::cellLibrary::libraryElement(3, 1, 1, 1, 3, 0.55);
	m_elements[gateSizing::NAND4] = gateSizing::cellLibrary::libraryElement(4, 1, 1, 1, 4, 0.4);
	m_elements[gateSizing::NOR2] = gateSizing::cellLibrary::libraryElement(2, 1, 1, 1, 2, 0.7);
	m_elements[gateSizing::NOR3] = gateSizing::cellLibrary::libraryElement(3, 1, 1, 1, 3, 0.55);
	m_elements[gateSizing::NOR4] = gateSizing::cellLibrary::libraryElement(4, 1, 1, 1, 4, 0.4);
	m_elements[gateSizing::AND2] = gateSizing::cellLibrary::libraryElement(2, 1, 1, 1, 2, 0.7);
	m_elements[gateSizing::OR2] = gateSizing::cellLibrary::libraryElement(2, 1, 1, 1, 2, 0.7);
	m_elements[gateSizing::XOR] = gateSizing::cellLibrary::libraryElement(8, 1, 1, 1, 8, 0.5);
	m_elements[gateSizing::XNOR] = gateSizing::cellLibrary::libraryElement(8, 1, 1, 1, 8, 0.5);
	m_elements[gateSizing::AOI21] = gateSizing::cellLibrary::libraryElement(6, 1, 1, 1, 6, 0.6);
	m_elements[gateSizing::AOI22] = gateSizing::cellLibrary::libraryElement(8, 1, 1, 1, 8, 0.55);
	m_elements[gateSizing::OAI21] = gateSizing::cellLibrary::libraryElement(6, 1, 1, 1, 6, 0.6);
	m_elements[gateSizing::OAI22] = gateSizing::cellLibrary::libraryElement(8, 1, 1, 1, 8, 0.55);
}