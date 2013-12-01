#include "StringHash.h"
#include "crc32.h"

#include <cstdio>
#include <ctype.h>

StringHash::StringHash(const char * str)
{
	m_str[27] = '\0';

	if(!str) {
		printf("StringHash error: String was null \n");
		return;
	}

	size_t len = strlen(str);
	len = (len > 26) ? 26 : len;

	for (int i = 0; i < len; ++i)
		m_str[i] = tolower(str[i]);

	m_str[len] = '\0';

	m_hash = crc32(m_str, len);
}