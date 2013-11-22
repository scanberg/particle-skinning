#include "StringHash.h"
#include "crc32.h"

#include <cstdio>

StringHash::StringHash(const char * const str)
{
	m_str[27] = '\0';

	if(!str) {
		printf("String was null \n");
		return;
	}

	size_t len = strlen(str);
	len = (len > 26) ? 26 : len;

	memcpy(m_str, str, len);
	m_str[len+1] = '\0';

	m_hash = crc32(m_str, len);
}