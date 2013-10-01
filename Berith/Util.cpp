#include "stdafx.h"
#include "Util.h"

std::wstring getDirname(wchar_t* dir)
{
	auto size = wcslen(dir);
	std::vector<wchar_t> drive_;
	drive_.resize(size);
	std::vector<wchar_t> dir_;
	dir_.resize(size);
	_wsplitpath_s(dir, drive_.data(), size, dir_.data(), size, nullptr, 0, nullptr, 0);

	return std::wstring(drive_.data())+std::wstring(dir_.data());
}

std::string toMultiByte(std::wstring const& s)
{
	size_t r = WideCharToMultiByte(CP_ACP, 0, s.c_str(), static_cast<int>(s.size()), nullptr, 0, nullptr, nullptr);
	std::vector<char> buff;
	buff.resize(r);
	WideCharToMultiByte(CP_ACP, 0, s.c_str(), static_cast<int>(s.size()), buff.data(), static_cast<int>(r), nullptr, nullptr);
	return std::string(buff.data(), buff.size());
}

bool fileExists(std::string const& path)
{
	DWORD const dwAttrib = GetFileAttributesA(const_cast<char*>(path.c_str()));
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool fileExists(std::wstring const& path)
{
	DWORD const dwAttrib = GetFileAttributesW(const_cast<wchar_t*>(path.c_str()));
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}


void showErrorDialog(std::wstring const& title, std::wstring const& contents, ...)
{
	wchar_t buff[8192];
	va_list args;
	va_start(args, contents);
	vswprintf_s(buff, contents.c_str(), args);
	va_end(args);
	MessageBoxW(NULL, buff, (std::wstring(L"Error at ")+title).c_str(),MB_OK | MB_ICONERROR);
}

void showErrorDialog(std::string const& title, std::string const& contents, ...)
{
	char buff[8192];
	va_list args;
	va_start(args, contents);
	vsprintf_s(buff, contents.c_str(), args);
	va_end(args);
	MessageBoxA(NULL, buff, (std::string("Error at ")+title).c_str(),MB_OK | MB_ICONERROR);
}

void logMessage(std::string const& tag, std::string const& fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vfprintf_s(stdout, fmt.c_str(), args);
	va_end(args);

}
void logMessage(std::wstring const& tag, std::wstring const& fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vfwprintf_s(stdout, fmt.c_str(), args);
	va_end(args);

}
