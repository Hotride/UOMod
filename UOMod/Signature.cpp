/***********************************************************************************
**
** Signature.cpp
**
** Copyright (C) February 2017 Hotride
**
************************************************************************************
*/
//----------------------------------------------------------------------------------
#include "stdafx.h"
#include "Signature.h"
//----------------------------------------------------------------------------------
CSignature::CSignature(const unsigned char *signature, const int &size, const unsigned char &ignoreValue)
: m_IgnoreValue(ignoreValue), m_Signature(size)
{
	memcpy(&m_Signature[0], &signature[0], size);
}
//----------------------------------------------------------------------------------
bool CSignature::Find(unsigned char *buf)
{
	unsigned char *ptr = buf;

	for (const unsigned char &value : m_Signature)
	{
		if (value != m_IgnoreValue && value != *ptr)
			return false;

		ptr++;
	}

	return true;
}
//----------------------------------------------------------------------------------