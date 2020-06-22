#include "Registry.h"
#include <windows.h>
#pragma comment (lib, "advapi32")
CRegistry::CRegistry(HKEY hKey, LPCSTR lpSubKey, bool mode)
{
	_hKey = 0;
	if (RegOpenKeyExA(hKey, lpSubKey, 0, KEY_ALL_ACCESS, &_hKey) != ERROR_SUCCESS)
	{ 
		if (mode == true) RegCreateKeyExA(hKey, lpSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &_hKey, NULL);
	}
	else error_success = true;
}
CRegistry::~CRegistry()
{
	if (_hKey != NULL)
	RegCloseKey(_hKey);
}
bool CRegistry::GetRegister(LPCSTR lpName, DWORD dwType, void* outBuf, DWORD size)
{
	if (outBuf == nullptr || _hKey == NULL)
		return false;
	if (!strlen(lpName))
		return false;
	if (!AutoSizeRead(dwType, outBuf, size))
		return false;

	return !RegQueryValueExA(_hKey, lpName, NULL, &dwType, (byte*)outBuf, &size);
}
bool CRegistry::SetRegister(LPCSTR lpName, DWORD dwType, void* inBuf, DWORD size)
{
	if (inBuf == nullptr || _hKey == NULL)
		return false;
	if (!strlen(lpName))
		return false;
	if (!AutoSizeWrite(dwType, inBuf, size))
		return false;

	return !RegSetValueExA(_hKey, lpName, 0, dwType, (byte*)inBuf, size);
}

bool CRegistry::GetRegisterDefault(LPSTR outBuf, LONG maxSize)
{
	if (outBuf == nullptr || _hKey == NULL)
		return false;

	return !RegQueryValueA(_hKey, NULL, (LPSTR)outBuf, &maxSize);
}
bool CRegistry::SetRegisterDefault(LPCSTR inBuf)
{
	if (inBuf == nullptr || _hKey == NULL)
		return false;

	return !RegSetValueA(_hKey, NULL, REG_SZ, inBuf, strlen(inBuf));
}

bool CRegistry::DeleteRegister(LPCSTR lpName)
{
	if (_hKey == NULL)
		return false;
	if (!strlen(lpName))
		return false;
	return !RegDeleteKeyEx(_hKey, lpName, KEY_WOW64_32KEY, NULL);
}

bool CRegistry::AutoSizeWrite(DWORD dwType, void* inBuf, DWORD &size)
{
	if (!size){
		switch (dwType)
		{
		case REG_BINARY:
			size = 1;
			break;
		case REG_DWORD:
		case REG_DWORD_BIG_ENDIAN:
			size = 4;
			break;
		case REG_QWORD:
			size = 8;
			break;
		case REG_EXPAND_SZ:
		case REG_LINK:
		case REG_MULTI_SZ:
		case REG_SZ:
			size = strlen((LPCSTR)inBuf);
			break;
		case REG_NONE:
		default:
			return false;
		}
	}
	return true;
}
bool CRegistry::AutoSizeRead(DWORD dwType, void* outBuf, DWORD &size)
{
	if (!size){
		switch (dwType)
		{
		case REG_BINARY:
			size = 1;
			break;
		case REG_DWORD:
		case REG_DWORD_BIG_ENDIAN:
			size = 4;
			break;
		case REG_QWORD:
			size = 8;
			break;
		case REG_EXPAND_SZ:
		case REG_LINK:
		case REG_MULTI_SZ:
		case REG_SZ:
		case REG_NONE:
		default:
			return false;
		}
	}
	return true;
}
CEasyRegistry::CEasyRegistry(HKEY hKey, LPCSTR lpSubKey, bool mode) : CRegistry(hKey, lpSubKey, mode)
{
	no_error = true;
}
CEasyRegistry::~CEasyRegistry()
{

}
void CEasyRegistry::DeleteKey(LPCSTR lpName)
{
	no_error = DeleteRegister(lpName);
}
void CEasyRegistry::WriteString(LPCSTR lpName, LPSTR lpString, ...)
{
	va_list ap;
	char    *szStr = new char[strlen(lpString) * 2 + 1024];
	va_start(ap, lpString);
	vsprintf(szStr, lpString, ap);
	va_end(ap);

	no_error = SetRegister(lpName, REG_SZ, szStr, strlen(szStr));
	delete[] szStr;
}
std::string CEasyRegistry::ReadString(LPCSTR lpName)
{
	char szStr[0x1000];
	no_error = GetRegister(lpName, REG_MULTI_SZ, szStr, 0x1000);
	return szStr;
}

void CEasyRegistry::WriteInteger(LPCSTR lpName, int value)
{
	no_error = SetRegister(lpName, REG_DWORD, &value, 0);
}
int CEasyRegistry::ReadInteger(LPCSTR lpName)
{
	int value;
	no_error = GetRegister(lpName, REG_DWORD, &value, 0);
	return value;
}

void CEasyRegistry::WriteFloat(LPCSTR lpName, float value)
{
	no_error = SetRegister(lpName, REG_DWORD, &value, 0);
}
float CEasyRegistry::ReadFloat(LPCSTR lpName)
{
	float value;
	no_error = GetRegister(lpName, REG_DWORD, &value, 0);
	return value;
}

void CEasyRegistry::WriteLongLong(LPCSTR lpName, long long value)
{
	no_error = SetRegister(lpName, REG_QWORD, &value, 0);
}
long long CEasyRegistry::ReadLongLong(LPCSTR lpName)
{
	long long value;
	no_error = GetRegister(lpName, REG_QWORD, &value, 0);
	return value;
}

void CEasyRegistry::WriteDouble(LPCSTR lpName, double value)
{
	no_error = SetRegister(lpName, REG_QWORD, &value, 0);
}
double CEasyRegistry::ReadDouble(LPCSTR lpName)
{
	double value;
	no_error = GetRegister(lpName, REG_QWORD, &value, 0);
	return value;
}

bool CEasyRegistry::IsError()
{
	return !no_error;
}

bool CEasyRegistry::ErrorSuccess()
{
	if (error_success == true) return true;
	return false;
}
