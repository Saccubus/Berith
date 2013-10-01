#include "stdafx.h"
#include "Util.h"
#include <cassert>

#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

static HANDLE fhandle;
void initUtil()
{
	fhandle = CreateFileW(L"launch.log",GENERIC_WRITE,FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if(INVALID_HANDLE_VALUE == fhandle){
		errMsg("init", "Failed to open launch.log");
	}
	SetStdHandle(STD_OUTPUT_HANDLE, fhandle);
	SetStdHandle(STD_ERROR_HANDLE, fhandle);

	logMsg("init", "stdout reopened.");
}
void closeUtil()
{
	CloseHandle(fhandle);
}

std::wstring getDirname(const wchar_t* dir)
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

#define BUFF_SIZE (1024*16)

static std::wstring getLastErrorMsg()
{
	wchar_t* buff_;
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0, GetLastError(), LANG_USER_DEFAULT, (LPWSTR)&buff_, 0, 0);
	std::wstring r(buff_);
	LocalFree(buff_);
	return r;
}

static void outMsg(DWORD stream, bool enableDiag, const wchar_t* const tag, const wchar_t* const fmt, va_list args)
{
	wchar_t buff_[BUFF_SIZE];
	wchar_t buff[BUFF_SIZE];
	memset(buff_, 0, BUFF_SIZE);
	memset(buff, 0, BUFF_SIZE);
	_vsnwprintf_s(buff_, BUFF_SIZE, fmt, args);
	DWORD len = _snwprintf_s(buff, BUFF_SIZE, L"[%s] %s\n", tag, buff_);
	HANDLE hand = GetStdHandle(stream);
	DWORD wlen;
	auto str = toMultiByte(buff);
	if( 0 == WriteFile(hand, str.c_str(), str.size(), &wlen, nullptr) ){
		auto msg = getLastErrorMsg();
		_snwprintf_s(buff, BUFF_SIZE, L"Failed to write console");
		MessageBoxW(NULL, (std::wstring(buff)+L"\n"+msg).c_str(), L"Error",MB_OK | MB_ICONERROR);
	}

	if(enableDiag){
		MessageBoxW(NULL, buff_, (std::wstring(L"Error at ")+tag).c_str(),MB_OK | MB_ICONERROR);
	}
}

static void outMsg(DWORD stream, bool enableDiag, const char* const tag, const char* const fmt, va_list args)
{
	char buff_[BUFF_SIZE];
	char buff[BUFF_SIZE];
	memset(buff_, 0, BUFF_SIZE);
	memset(buff, 0, BUFF_SIZE);
	vsnprintf_s(buff_, BUFF_SIZE, fmt, args);
	DWORD len = _snprintf_s(buff, BUFF_SIZE, "[%s] %s\n", tag, buff_);
	HANDLE hand = GetStdHandle(stream);
	DWORD wlen;
	if( 0 == WriteFile(hand, buff, len, &wlen, nullptr) ){
		auto msg = getLastErrorMsg();
		wchar_t buff[BUFF_SIZE];
		_snwprintf_s(buff, BUFF_SIZE, L"Failed to write console");
		MessageBoxW(NULL, (std::wstring(buff)+L"\n"+msg).c_str(), L"Error",MB_OK | MB_ICONERROR);
	}

	if(enableDiag){
		MessageBoxA(NULL, buff_, (std::string("Error at ")+tag).c_str(),MB_OK | MB_ICONERROR);
	}
}

void errMsg(const wchar_t* const tag, const wchar_t* const fmt, ...)
{
	assert( stderr == stderr );
	va_list args;
	va_start(args, fmt);
	outMsg(STD_ERROR_HANDLE, true, tag, fmt, args);
	va_end(args);
}

void errMsg(const char* const tag, const char* const fmt, ...)
{
	assert( stderr == stderr );
	va_list args;
	va_start(args, fmt);
	outMsg(STD_ERROR_HANDLE, true, tag, fmt, args);
	va_end(args);
}

void logMsg(const wchar_t* const tag, const wchar_t* const fmt, ...)
{
	assert( stdout == stdout );
	va_list args;
	va_start(args, fmt);
	outMsg(STD_OUTPUT_HANDLE, false, tag, fmt, args);
	va_end(args);
}

void logMsg(const char* const tag, const char* const fmt, ...)
{
	assert( stdout == stdout );
	va_list args;
	va_start(args, fmt);
	outMsg(STD_OUTPUT_HANDLE, false, tag, fmt, args);
	va_end(args);
}

void warnMsg(const wchar_t* const tag, const wchar_t* const fmt, ...)
{
	assert( stderr == stderr );
	va_list args;
	va_start(args, fmt);
	outMsg(STD_ERROR_HANDLE, false, tag, fmt, args);
	va_end(args);
}

void warnMsg(const char* const tag, const char* const fmt, ...)
{
	assert( stderr == stderr );
	va_list args;
	va_start(args, fmt);
	outMsg(STD_ERROR_HANDLE, false, tag, fmt, args);
	va_end(args);
}
