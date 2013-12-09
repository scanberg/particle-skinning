#include <map>
#include <vector>
#include "StringHash.h"

template<class T>
class Store
{
public:
	Store()	{}
	~Store() {}

	T* get(StringHash);
	T* get(size_t);

	void add(StringHash, T);

	void remove(StringHash);
	void remove(size_t);

	void clearAll();
private:
	std::map<unsigned int, size_t> m_indexTable;
	std::vector<T> m_data;
};

template<class T>
T* Store::get(StringHash sh)
{
	std::map<unsigned int, size_t>::iterator it = m_indexTable.find(sh.getHash());

	if(it == m_indexTable.end())
		return nullptr;
	
	return m_data[*it->second];
}

template<class T>
T* Store::get(size_t i)
{
	if(i >= m_data.size())
		return nullptr;

	return m_data[i];
}

template<class T>
void Store::add(StringHash sh, T item)
{
	
}

template<class T>
void Store::remove(StringHash)
{

}

template<class T>
void Store::remove(size_t)
{

}

template<class T>
void Store::clearAll()
{

}