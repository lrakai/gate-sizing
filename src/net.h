// class net retains driver, loads, and name information about nets in the design

#ifndef NET_H_
#define NET_H_

#include <vector>
#include <string>

namespace gateSizing {

class cell;

class net
{
public:
	net()
	:m_driver(NULL)
	{	
		m_loads.reserve(10);	
	}

	net(std::string& n, cell* d)
	:m_name(n), m_driver(d)
	{	
		m_loads.reserve(10);	
	}
	
	virtual ~net();
	
	inline void assignDriver(cell* d)
	{	
		m_driver = d;
	}
	
	inline void addLoad(cell* l)
	{	
		m_loads.push_back(l);
	}
	
	inline cell* driver() const
	{	
		return m_driver;
	}
	
	inline const std::vector<cell*>& loads() const
	{	
		return m_loads;
	}
	
	void print() const;
	
	inline const std::string name() const
	{	
		return m_name;
	}
	
private:
	std::string m_name;
	cell* m_driver;
	std::vector<cell*> m_loads;
};

}

#endif /*NET_H_*/
