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
		errDlg(GetLastError(), "init", "init", "Failed to open launch.log: %d", GetLastError());
		return;
	}
	if( 0 == SetStdHandle(STD_OUTPUT_HANDLE, fhandle) ) {
		errDlg(GetLastError(), "init", "Failed to set STD_OUTPUT_HANDLE: %d", GetLastError());
	}
	if ( 0 == SetStdHandle(STD_ERROR_HANDLE, fhandle) ) {
		errDlg(GetLastError(), "init", "Failed to set STD_ERROR_HANDLE: %d", GetLastError());
	}

	const HANDLE hStandard = GetStdHandle(STD_OUTPUT_HANDLE);
	if( INVALID_HANDLE_VALUE == hStandard ) {
		errDlg(GetLastError(), "init", "Failed to open STD_OUTPUT_HANDLE : %d", GetLastError());
	}
	const HANDLE hError = GetStdHandle(STD_ERROR_HANDLE);
	if( INVALID_HANDLE_VALUE == hError ) {
		errDlg(GetLastError(), "init", "Failed to open STD_ERROR_HANDLE : %d", GetLastError());
	}

	if( hStandard != fhandle ) {
		errMsg("init", "STD_OUTPUT_HANDLE mismatched: %d != %d", hStandard, fhandle);
	}
	if( hError != fhandle ) {
		errMsg("init", "STD_ERROR_HANDLE mismatched: %d != %d", hError, fhandle);
	}

	logMsg("init", "stdout reopened.");
	warnMsg("init", "stderr reopened.");
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

static std::wstring getLastErrorMsg(DWORD errorCode)
{
	wchar_t* buff_;
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0, errorCode, LANG_USER_DEFAULT, (LPWSTR)&buff_, 0, 0);
	std::wstring r(buff_);
	LocalFree(buff_);
	return r;
}

void errDlg(DWORD errCode, const wchar_t* const tag, const wchar_t* const fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	{
		wchar_t buff[BUFF_SIZE];
		auto msg = getLastErrorMsg(errCode);
		_vsnwprintf_s(buff, BUFF_SIZE, fmt, args);
		MessageBoxW(NULL, (std::wstring(buff)+L"\n"+msg).c_str(), L"Error",MB_OK | MB_ICONERROR);
	}
	va_end(args);
}
void errDlg(DWORD errCode, const char* const tag, const char* const fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	{
		char buff[BUFF_SIZE];
		auto msg = toMultiByte(getLastErrorMsg(errCode));
		_vsnprintf_s(buff, BUFF_SIZE, fmt, args);
		MessageBoxA(NULL, (std::string(buff)+"\n"+msg).c_str(), "Error",MB_OK | MB_ICONERROR);
	}
	va_end(args);
}

static void outMsg(DWORD stream, bool enableDiag, const wchar_t* const tag, const wchar_t* const fmt, va_list args)
{
	wchar_t buff_[BUFF_SIZE];
	wchar_t buff[BUFF_SIZE];
	memset(buff_, 0, BUFF_SIZE);
	memset(buff, 0, BUFF_SIZE);
	_vsnwprintf_s(buff_, BUFF_SIZE, fmt, args);
	const DWORD len = _snwprintf_s(buff, BUFF_SIZE, L"[%s] %s\n", tag, buff_);
	const HANDLE hand = GetStdHandle(stream);
	DWORD wlen;
	const auto str = toMultiByte(buff);
	SetLastError(0);
	const BOOL result = WriteFile(hand, str.c_str(), static_cast<DWORD>(str.size()), &wlen, nullptr);
	if( result == 0 ){
		const DWORD errCode = GetLastError();
		errDlg(errCode, tag, L"Failed to write console: %d", errCode);
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
	const DWORD len = _snprintf_s(buff, BUFF_SIZE, "[%s] %s\n", tag, buff_);
	const HANDLE hand = GetStdHandle(stream);
	DWORD wlen;
	SetLastError(0);
	BOOL result = WriteFile(hand, buff, len, &wlen, nullptr);
	if( result == 0 ){
		const DWORD errCode = GetLastError();
		errDlg(errCode, tag, "Failed to write console: %d", errCode);
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
