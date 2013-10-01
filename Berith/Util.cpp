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

static void outMsg(FILE*stream, bool enableDiag, const wchar_t* const tag, const wchar_t* const fmt, va_list args)
{
	wchar_t buff[8192];
	vswprintf_s(buff, fmt, args);

	if(enableDiag){
		MessageBoxW(NULL, buff, (std::wstring(L"Error at ")+tag).c_str(),MB_OK | MB_ICONERROR);
	}
	fwprintf_s(stream, L"[%s] %s\n", tag, buff);
	fflush(stream);
}

static void outMsg(FILE*stream, bool enableDiag, const char* const tag, const char* const fmt, va_list args)
{
	char buff[8192];
	vsprintf_s(buff, fmt, args);

	if(enableDiag){
		MessageBoxA(NULL, buff, (std::string("Error at ")+tag).c_str(),MB_OK | MB_ICONERROR);
	}
	fprintf_s(stream, "[%s] %s\n", tag, buff);
	fflush(stream);
}

void errMsg(const wchar_t* const tag, const wchar_t* const fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	outMsg(stderr, true, tag, fmt, args);
	va_end(args);
}

void errMsg(const char* const tag, const char* const fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	outMsg(stderr, true, tag, fmt, args);
	va_end(args);
}

void logMsg(const wchar_t* const tag, const wchar_t* const fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	outMsg(stdout, false, tag, fmt, args);
	va_end(args);
}

void logMsg(const char* const tag, const char* const fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	outMsg(stdout, false, tag, fmt, args);
	va_end(args);
}
