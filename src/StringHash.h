#pragma once

class StringHash
{
public:
	StringHash(const char * str);

	inline const char * getStr()
	{ return m_str; }

	inline unsigned int getHash()
	{ return m_hash; }

	bool operator==(const StringHash & rhs) const
	{
		return m_hash == rhs.m_hash;
	}

private:
	char 			m_str[28];
	unsigned int 	m_hash;
};