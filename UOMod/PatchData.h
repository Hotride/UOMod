/***********************************************************************************
**
** PatchData.h
**
** Copyright (C) February 2017 Hotride
**
************************************************************************************
*/
//----------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------
class CPatchData
{
public:
	CPatchData();
	virtual ~CPatchData() {}

	bool Found{ false };
	unsigned int Address{ 0 };

	std::vector<unsigned char> OriginalSignature;
	std::vector<unsigned char> NewSignature;

	virtual bool IsPatched();

	virtual bool Patch(const bool &state);

	virtual bool PatchFile(unsigned char *file, const int &size);

	virtual void Reset();
};
//----------------------------------------------------------------------------------
class CPatchDataFPS : public CPatchData
{
public:
	CPatchDataFPS();
	virtual ~CPatchDataFPS() {}

	unsigned int TimerAddress{ 0 };

	unsigned int ReturnAddress{ 0 };
	unsigned int Offset1{ 0 };
	unsigned int Offset2{ 0 };

	unsigned int AddressOfCutting{ 0 };

	unsigned int DataSize{ 0 };

	virtual bool IsPatched();

	virtual bool Patch(const bool &state);

	virtual void Reset();
};
//----------------------------------------------------------------------------------
class CPatchDataLight : public CPatchData
{
public:
	CPatchDataLight();
	virtual ~CPatchDataLight() {}

	unsigned int ValueAddress{ 0 };

	virtual bool Patch(const bool &state);

	virtual void Reset();
};
//----------------------------------------------------------------------------------
class CPatchDataPaperdollSlots : public CPatchData
{
public:
	CPatchDataPaperdollSlots();
	virtual ~CPatchDataPaperdollSlots() {}

	std::vector<unsigned char> OriginalSignature2;
	std::vector<unsigned char> NewSignature2;

	unsigned int Address2{ 0 };

	virtual bool IsPatched();

	virtual bool Patch(const bool &state);

	virtual bool PatchFile(unsigned char *file, const int &size);

	virtual void Reset();
};
//----------------------------------------------------------------------------------
class CPatchDataMultiUO : public CPatchDataPaperdollSlots
{
public:
	CPatchDataMultiUO();
	virtual ~CPatchDataMultiUO() {}
};
//----------------------------------------------------------------------------------
class CPatchDataNoCrypt : public CPatchData
{
public:
	CPatchDataNoCrypt();
	virtual ~CPatchDataNoCrypt() {}

	unsigned int AddressNew{ 0 };
	unsigned int DecryptAddress1{ 0 };
	unsigned int DecryptAddress2{ 0 };
	unsigned int Decrypt2Address{ 0 };
	unsigned int TFOldAddress{ 0 };
	unsigned int TFNewAddress{ 0 };

	virtual bool PatchFile(unsigned char *file, const int &size);

	virtual void Reset();
};
//----------------------------------------------------------------------------------
class CPatchDataViewRange : public CPatchData
{
public:
	CPatchDataViewRange();
	virtual ~CPatchDataViewRange() {}

	std::vector<unsigned char> OriginalSignature2;
	std::vector<unsigned char> OriginalSignature3;
	std::vector<unsigned char> OriginalSignature4;
	std::vector<unsigned char> OriginalSignature5;
	std::vector<unsigned char> OriginalSignature6;
	std::vector<unsigned char> OriginalSignature7;
	std::vector<unsigned char> OriginalSignature8;
	std::vector<unsigned char> OriginalSignature9;
	std::vector<unsigned char> OriginalSignature10;
	std::vector<unsigned char> OriginalSignature11;

	std::vector<unsigned char> NewSignature2;
	std::vector<unsigned char> NewSignature3;
	std::vector<unsigned char> NewSignature4;
	std::vector<unsigned char> NewSignature5;
	std::vector<unsigned char> NewSignature6;
	std::vector<unsigned char> NewSignature7;
	std::vector<unsigned char> NewSignature8;
	std::vector<unsigned char> NewSignature9;
	std::vector<unsigned char> NewSignature10;
	std::vector<unsigned char> NewSignature11;

	unsigned int Address2{ 0 };
	unsigned int Address3{ 0 };
	unsigned int Address4{ 0 };
	unsigned int Address5{ 0 };
	unsigned int Address6{ 0 };
	unsigned int Address7{ 0 };
	unsigned int Address8{ 0 };
	unsigned int Address9{ 0 };
	unsigned int Address10{ 0 };
	unsigned int Address11{ 0 };

	unsigned int AddressViewRangeValue1{ 0 };
	unsigned int AddressViewRangeValue2{ 0 };

	virtual bool IsPatched();

	virtual bool Patch(const bool &state);

	virtual bool PatchFile(unsigned char *file, const int &size);

	virtual void Reset();

	void SetPatchViewRange(const int &value);

	int GetCurrentClientViewRange();
};
//----------------------------------------------------------------------------------
