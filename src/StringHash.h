#pragma once

class StringHash
{
public:
	StringHash(const char * str);

	inline const char * getStr()
	{ return m_str; }

	inline unsigned int getHash()
	{ return m_hash; }

private:
	char 			m_str[28];
	unsigned int 	m_hash;
};