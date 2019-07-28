/***********************************************************************************
**
** PatchData.cpp
**
** Copyright (C) February 2017 Hotride
**
************************************************************************************
*/
//----------------------------------------------------------------------------------
#include "stdafx.h"
#include "PatchData.h"
#include "ProcessManager.h"
#include "FPSPatchASM.h"
//----------------------------------------------------------------------------------
//-----------------------------------CPatchData-------------------------------------
//----------------------------------------------------------------------------------
CPatchData::CPatchData()
{
}
//----------------------------------------------------------------------------------
bool CPatchData::IsPatched()
{
	int size = OriginalSignature.size();

	if (Found && size)
	{
		std::vector<unsigned char> vec(size, 0);

		g_ProcessManager.ReadPByte(Address, &vec[0], size);

		return !memcmp(&NewSignature[0], &vec[0], size);
	}

	return false;
}
//----------------------------------------------------------------------------------
bool CPatchData::Patch(const bool &state)
{
	if (Found)
	{
		if (state)
			g_ProcessManager.WritePByte(Address, &NewSignature[0], NewSignature.size());
		else
			g_ProcessManager.WritePByte(Address, &OriginalSignature[0], OriginalSignature.size());

		return true;
	}

	return false;
}
//----------------------------------------------------------------------------------
bool CPatchData::PatchFile(unsigned char *file, const int &size)
{
	if (Found && NewSignature.size())
	{
		memcpy(&file[Address - 0x00400000], &NewSignature[0], NewSignature.size());

		return true;
	}

	return false;
}
//----------------------------------------------------------------------------------
void CPatchData::Reset()
{
	Found = false;
	Address = 0;

	OriginalSignature.clear();
	NewSignature.clear();
}
//----------------------------------------------------------------------------------
//----------------------------------CPatchDataFPS-----------------------------------
//----------------------------------------------------------------------------------
CPatchDataFPS::CPatchDataFPS()
: CPatchData()
{
}
//----------------------------------------------------------------------------------
bool CPatchDataFPS::IsPatched()
{
	if (Found)
		return g_FPSPatch;

	return false;
}
//----------------------------------------------------------------------------------
bool CPatchDataFPS::Patch(const bool &state)
{
	if (Found)
	{
		g_FPSPatch = state;

		BYTE timerval = 0;
		BYTE dataval[5004] = { 0 };

		BYTE v1 = 0x08;
		BYTE v2 = 0x04;

		if (!g_FPSPatch)
		{
			timerval = 0x4C;
			v1 = 0x04;
			v2 = 0x02;
		}

		int size = DataSize;

		if (size > 5000)
			size = 5000;

		int fsize = (size / 2) + 1;

		for (int i = 0; i < fsize; i++)
		{
			dataval[i * 2] = v1;
			dataval[i * 2 + 1] = v2;
		}

		g_ProcessManager.WriteByte(TimerAddress, timerval);

		g_ProcessManager.WritePByte(Address, dataval, DataSize);

		return true;
	}

	return false;
}
//----------------------------------------------------------------------------------
void CPatchDataFPS::Reset()
{
	TimerAddress = 0;
	ReturnAddress = 0;
	Offset1 = 0;
	Offset2 = 0;
	AddressOfCutting = 0;
	DataSize = 0;

	CPatchData::Reset();
}
//----------------------------------------------------------------------------------
//--------------------------------CPatchDataLight-----------------------------------
//----------------------------------------------------------------------------------
CPatchDataLight::CPatchDataLight()
: CPatchData()
{
}
//----------------------------------------------------------------------------------
bool CPatchDataLight::Patch(const bool &state)
{
	if (Found)
	{
		CPatchData::Patch(state);

		if (ValueAddress && state)
			g_ProcessManager.WriteUInt(ValueAddress, 0);

		return true;
	}

	return false;
}
//----------------------------------------------------------------------------------
void CPatchDataLight::Reset()
{
	ValueAddress = 0;

	CPatchData::Reset();
}
//----------------------------------------------------------------------------------
//----------------------------CPatchDataPaperdollSlots------------------------------
//----------------------------------------------------------------------------------
CPatchDataPaperdollSlots::CPatchDataPaperdollSlots()
: CPatchData()
{
}
//----------------------------------------------------------------------------------
bool CPatchDataPaperdollSlots::IsPatched()
{
	int size = OriginalSignature.size();
	int size2 = OriginalSignature2.size();

	if (Found && size && size2)
	{
		std::vector<unsigned char> vec(size, 0);
		std::vector<unsigned char> vec2(size2, 0);

		g_ProcessManager.ReadPByte(Address, &vec[0], size);
		g_ProcessManager.ReadPByte(Address2, &vec2[0], size2);

		return (!memcmp(&NewSignature[0], &vec[0], size) && !memcmp(&NewSignature2[0], &vec2[0], size2));
	}

	return false;
}
//----------------------------------------------------------------------------------
bool CPatchDataPaperdollSlots::Patch(const bool &state)
{
	if (Found && NewSignature.size() && NewSignature2.size())
	{
		if (state)
		{
			g_ProcessManager.WritePByte(Address, &NewSignature[0], NewSignature.size());
			g_ProcessManager.WritePByte(Address2, &NewSignature2[0], NewSignature2.size());
		}
		else
		{
			g_ProcessManager.WritePByte(Address, &OriginalSignature[0], OriginalSignature.size());
			g_ProcessManager.WritePByte(Address2, &OriginalSignature2[0], OriginalSignature2.size());
		}

		return true;
	}

	return false;
}
//----------------------------------------------------------------------------------
bool CPatchDataPaperdollSlots::PatchFile(unsigned char *file, const int &size)
{
	if (Found && NewSignature.size() && NewSignature2.size())
	{
		memcpy(&file[Address - 0x00400000], &NewSignature[0], NewSignature.size());
		memcpy(&file[Address2 - 0x00400000], &NewSignature2[0], NewSignature2.size());

		return true;
	}

	return false;
}
//----------------------------------------------------------------------------------
void CPatchDataPaperdollSlots::Reset()
{
	OriginalSignature2.clear();
	NewSignature2.clear();

	Address2 = 0;

	CPatchData::Reset();
}
//----------------------------------------------------------------------------------
//--------------------------------CPatchDataMultiUO---------------------------------
//----------------------------------------------------------------------------------
CPatchDataMultiUO::CPatchDataMultiUO()
: CPatchDataPaperdollSlots()
{
}
//----------------------------------------------------------------------------------
//--------------------------------CPatchDataNoCrypt---------------------------------
//----------------------------------------------------------------------------------
CPatchDataNoCrypt::CPatchDataNoCrypt()
: CPatchData()
{
}
//----------------------------------------------------------------------------------
bool CPatchDataNoCrypt::PatchFile(unsigned char *file, const int &size)
{
	if (Found)
	{
		if (Address)
			file[Address - 0x00400000] = 0x85;

		if (AddressNew)
			file[AddressNew - 0x00400000] = 0xEB;

		if (DecryptAddress1)
		{
			file[DecryptAddress1 - 0x00400000] = 0x3B;
			file[DecryptAddress1 - 0x00400000 + 1] = 0xC0;
		}

		if (DecryptAddress2)
		{
			file[DecryptAddress2 - 0x00400000] = 0x3B;
			file[DecryptAddress2 - 0x00400000 + 1] = 0xC0;
		}

		if (Decrypt2Address)
			file[Decrypt2Address - 0x00400000] = 0xEB;

		if (TFOldAddress)
			file[TFOldAddress - 0x00400000] = 0x85;

		if (TFNewAddress)
			file[TFNewAddress - 0x00400000] = 0xEB;

		return true;
	}

	return false;
}
//----------------------------------------------------------------------------------
void CPatchDataNoCrypt::Reset()
{
	AddressNew = 0;
	DecryptAddress1 = 0;
	DecryptAddress2 = 0;
	Decrypt2Address = 0;
	TFOldAddress = 0;
	TFNewAddress = 0;

	CPatchData::Reset();
}
//----------------------------------------------------------------------------------
//----------------------------CPatchDataPaperdollSlots------------------------------
//----------------------------------------------------------------------------------
CPatchDataViewRange::CPatchDataViewRange()
: CPatchData()
{
}
//----------------------------------------------------------------------------------
bool CPatchDataViewRange::IsPatched()
{
	int size = OriginalSignature.size();
	int size2 = OriginalSignature2.size();
	int size3 = OriginalSignature3.size();
	int size4 = OriginalSignature4.size();
	int size5 = OriginalSignature5.size();
	int size6 = OriginalSignature6.size();
	int size7 = OriginalSignature7.size();
	int size8 = OriginalSignature8.size();
	int size9 = OriginalSignature9.size();
	int size10 = OriginalSignature10.size();
	int size11 = OriginalSignature11.size();

	if (Found && size && size2 && size3 && size4 && size5 && size6 && size7 && size8 && size9 && size10 && size11)
	{
		std::vector<unsigned char> vec(size, 0);
		std::vector<unsigned char> vec2(size2, 0);
		std::vector<unsigned char> vec3(size3, 0);
		std::vector<unsigned char> vec4(size4, 0);
		std::vector<unsigned char> vec5(size5, 0);
		std::vector<unsigned char> vec6(size6, 0);
		std::vector<unsigned char> vec7(size7, 0);
		std::vector<unsigned char> vec8(size8, 0);
		std::vector<unsigned char> vec9(size9, 0);
		std::vector<unsigned char> vec10(size10, 0);
		std::vector<unsigned char> vec11(size11, 0);

		g_ProcessManager.ReadPByte(Address, &vec[0], size);
		g_ProcessManager.ReadPByte(Address2, &vec2[0], size2);
		g_ProcessManager.ReadPByte(Address3, &vec3[0], size3);
		g_ProcessManager.ReadPByte(Address4, &vec4[0], size4);
		g_ProcessManager.ReadPByte(Address5, &vec5[0], size5);
		g_ProcessManager.ReadPByte(Address6, &vec6[0], size6);
		g_ProcessManager.ReadPByte(Address7, &vec7[0], size7);
		g_ProcessManager.ReadPByte(Address8, &vec8[0], size8);
		g_ProcessManager.ReadPByte(Address9, &vec9[0], size9);
		g_ProcessManager.ReadPByte(Address10, &vec10[0], size10);
		g_ProcessManager.ReadPByte(Address11, &vec11[0], size11);

		return (!memcmp(&NewSignature[0], &vec[0], size) && !memcmp(&NewSignature2[0], &vec2[0], size2) && !memcmp(&NewSignature3[0], &vec3[0], size3) && !memcmp(&NewSignature4[0], &vec4[0], size4) &&
			!memcmp(&NewSignature5[0], &vec5[0], size5) && !memcmp(&NewSignature6[0], &vec6[0], size6) && !memcmp(&NewSignature7[0], &vec7[0], size7) && !memcmp(&NewSignature8[0], &vec8[0], size8) &&
			!memcmp(&NewSignature9[0], &vec9[0], size9) && !memcmp(&NewSignature10[0], &vec10[0], size10) && !memcmp(&NewSignature11[0], &vec11[0], size11));
	}

	return false;
}
//----------------------------------------------------------------------------------
bool CPatchDataViewRange::Patch(const bool &state)
{
	if (Found && NewSignature.size() && NewSignature2.size() && NewSignature3.size() && NewSignature4.size() && NewSignature5.size() && NewSignature6.size() &&
		NewSignature7.size() && NewSignature8.size() && NewSignature9.size() && NewSignature10.size() && NewSignature11.size())
	{
		if (state)
		{
			g_ProcessManager.WritePByte(Address, &NewSignature[0], NewSignature.size());
			g_ProcessManager.WritePByte(Address2, &NewSignature2[0], NewSignature2.size());
			g_ProcessManager.WritePByte(Address3, &NewSignature3[0], NewSignature3.size());
			g_ProcessManager.WritePByte(Address4, &NewSignature4[0], NewSignature4.size());
			g_ProcessManager.WritePByte(Address5, &NewSignature5[0], NewSignature5.size());
			g_ProcessManager.WritePByte(Address6, &NewSignature6[0], NewSignature6.size());
			g_ProcessManager.WritePByte(Address7, &NewSignature7[0], NewSignature7.size());
			g_ProcessManager.WritePByte(Address8, &NewSignature8[0], NewSignature8.size());
			g_ProcessManager.WritePByte(Address9, &NewSignature9[0], NewSignature9.size());
			g_ProcessManager.WritePByte(Address10, &NewSignature10[0], NewSignature10.size());
			g_ProcessManager.WritePByte(Address11, &NewSignature11[0], NewSignature11.size());

			g_ProcessManager.WriteUInt(AddressViewRangeValue1, NewSignature5[0]);
			g_ProcessManager.WriteUInt(AddressViewRangeValue2, NewSignature5[0]);
		}
		else
		{
			g_ProcessManager.WritePByte(Address, &OriginalSignature[0], OriginalSignature.size());
			g_ProcessManager.WritePByte(Address2, &OriginalSignature2[0], OriginalSignature2.size());
			g_ProcessManager.WritePByte(Address3, &OriginalSignature3[0], OriginalSignature3.size());
			g_ProcessManager.WritePByte(Address4, &OriginalSignature4[0], OriginalSignature4.size());
			g_ProcessManager.WritePByte(Address5, &OriginalSignature5[0], OriginalSignature5.size());
			g_ProcessManager.WritePByte(Address6, &OriginalSignature6[0], OriginalSignature6.size());
			g_ProcessManager.WritePByte(Address7, &OriginalSignature7[0], OriginalSignature7.size());
			g_ProcessManager.WritePByte(Address8, &OriginalSignature8[0], OriginalSignature8.size());
			g_ProcessManager.WritePByte(Address9, &OriginalSignature9[0], OriginalSignature9.size());
			g_ProcessManager.WritePByte(Address10, &OriginalSignature10[0], OriginalSignature10.size());
			g_ProcessManager.WritePByte(Address11, &OriginalSignature11[0], OriginalSignature11.size());

			g_ProcessManager.WriteUInt(AddressViewRangeValue1, OriginalSignature5[0]);
			g_ProcessManager.WriteUInt(AddressViewRangeValue2, OriginalSignature5[0]);
		}

		return true;
	}

	return false;
}
//----------------------------------------------------------------------------------
bool CPatchDataViewRange::PatchFile(unsigned char *file, const int &size)
{
	if (Found && NewSignature.size() && NewSignature2.size() && NewSignature3.size() && NewSignature4.size() && NewSignature5.size() && NewSignature6.size() &&
		NewSignature7.size() && NewSignature8.size() && NewSignature9.size() && NewSignature10.size() && NewSignature11.size())
	{
		memcpy(&file[Address - 0x00400000], &NewSignature[0], NewSignature.size());
		memcpy(&file[Address2 - 0x00400000], &NewSignature2[0], NewSignature2.size());
		memcpy(&file[Address3 - 0x00400000], &NewSignature3[0], NewSignature3.size());
		memcpy(&file[Address4 - 0x00400000], &NewSignature4[0], NewSignature4.size());
		memcpy(&file[Address5 - 0x00400000], &NewSignature5[0], NewSignature5.size());
		memcpy(&file[Address6 - 0x00400000], &NewSignature6[0], NewSignature6.size());
		memcpy(&file[Address7 - 0x00400000], &NewSignature7[0], NewSignature7.size());
		memcpy(&file[Address8 - 0x00400000], &NewSignature8[0], NewSignature8.size());
		memcpy(&file[Address9 - 0x00400000], &NewSignature9[0], NewSignature9.size());
		memcpy(&file[Address10 - 0x00400000], &NewSignature10[0], NewSignature10.size());
		memcpy(&file[Address11 - 0x00400000], &NewSignature11[0], NewSignature11.size());

		return true;
	}

	return false;
}
//----------------------------------------------------------------------------------
void CPatchDataViewRange::Reset()
{
	OriginalSignature2.clear();
	OriginalSignature3.clear();
	OriginalSignature4.clear();
	OriginalSignature5.clear();
	OriginalSignature6.clear();
	OriginalSignature7.clear();
	OriginalSignature8.clear();
	OriginalSignature9.clear();
	OriginalSignature10.clear();
	OriginalSignature11.clear();

	NewSignature2.clear();
	NewSignature3.clear();
	NewSignature4.clear();
	NewSignature5.clear();
	NewSignature6.clear();
	NewSignature7.clear();
	NewSignature8.clear();
	NewSignature9.clear();
	NewSignature10.clear();
	NewSignature11.clear();

	Address2 = 0;
	Address2 = 0;
	Address3 = 0;
	Address4 = 0;
	Address5 = 0;
	Address6 = 0;
	Address7 = 0;
	Address8 = 0;
	Address9 = 0;
	Address10 = 0;
	Address11 = 0;
	AddressViewRangeValue1 = 0;
	AddressViewRangeValue2 = 0;

	CPatchData::Reset();
}
//----------------------------------------------------------------------------------
void CPatchDataViewRange::SetPatchViewRange(const int &value)
{
	if (Found && value >= 0x12 && value <= 0x1F && NewSignature5.size() && NewSignature6.size() && NewSignature7.size() && NewSignature8.size())
	{
		NewSignature5[0] = value;
		NewSignature6[0] = value;
		NewSignature7[0] = value;
		NewSignature8[0] = value;
	}
}
//----------------------------------------------------------------------------------
int CPatchDataViewRange::GetCurrentClientViewRange()
{
	int size = OriginalSignature8.size();

	if (Found && size)
	{
		std::vector<unsigned char> vec(size, 0);

		g_ProcessManager.ReadPByte(Address8, &vec[0], size);

		return vec[0];
	}

	return 0;
}
//----------------------------------------------------------------------------------