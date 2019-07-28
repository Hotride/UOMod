/***********************************************************************************
**
** Signature.h
**
** Copyright (C) February 2017 Hotride
**
************************************************************************************
*/
//----------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------
#define SIGNATURE(name, array, count, ignoreValue) CSignature name((unsigned char*)array, count, ignoreValue)
#define SIGNATURE_ITEM(array, count, ignoreValue) CSignature((unsigned char*)array, count, ignoreValue)
//----------------------------------------------------------------------------------
class CSignature
{
private:
	unsigned char m_IgnoreValue;
	std::vector<unsigned char> m_Signature;

public:
	CSignature(const unsigned char *signature, const int &size, const unsigned char &ignoreValue);
	~CSignature() {}

	bool Find(unsigned char *buf);
};
//----------------------------------------------------------------------------------
