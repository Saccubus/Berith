#include "stdafx.h"

#include "Util.h"

#include <cassert>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

ChangeStdHandle::ChangeStdHandle(std::wstring const& fname)
:fhandle_(CreateFileW(fname.c_str() ,GENERIC_WRITE,FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr))
{
	init(fname);
}

ChangeStdHandle::ChangeStdHandle()
:fhandle_(CreateFileW(L"launch.log",GENERIC_WRITE,FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr))
{
	init(L"launch.log");
}

void ChangeStdHandle::init(std::wstring const& fname)
{
	if(INVALID_HANDLE_VALUE == fhandle_){
		errDlg(GetLastError(), L"init", L"ログファイル（%s）を開けません: %d", fname.c_str(), GetLastError());
		return;
	}
	if( 0 == SetStdHandle(STD_OUTPUT_HANDLE, fhandle_) ) {
		errDlg(GetLastError(), L"init", L"STD_OUTPUT_HANDLEの入れ替えに失敗: %d", GetLastError());
	}
	if ( 0 == SetStdHandle(STD_ERROR_HANDLE, fhandle_) ) {
		errDlg(GetLastError(), L"init", L"STD_ERROR_HANDLEの入れ替えに失敗: %d", GetLastError());
	}

	const HANDLE hStandard = GetStdHandle(STD_OUTPUT_HANDLE);
	if( INVALID_HANDLE_VALUE == hStandard ) {
		errDlg(GetLastError(), L"init", L"STD_OUTPUT_HANDLEの取得に失敗しました : %d", GetLastError());
	}
	const HANDLE hError = GetStdHandle(STD_ERROR_HANDLE);
	if( INVALID_HANDLE_VALUE == hError ) {
		errDlg(GetLastError(), L"init", L"STD_ERROR_HANDLEの取得に失敗しました : %d", GetLastError());
	}

	if( hStandard != fhandle_ ) {
		errMsg(L"init", L"STD_OUTPUT_HANDLEが一致しません: %d != %d", hStandard, fhandle_);
	}
	if( hError != fhandle_ ) {
		errMsg(L"init", L"STD_ERROR_HANDLEが一致しません: %d != %d", hError, fhandle_);
	}
	logMsg(L"init", L"stdout reopened.");
	warnMsg(L"init", L"stderr reopened.");
}

ChangeStdHandle::~ChangeStdHandle()
{
	logMsg(L"init", L"stdout closing...");
	warnMsg(L"init", L"stderr closing...");
	CloseHandle(fhandle_);
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

static void outMsg(DWORD stream, bool enableDiag, const wchar_t* const tag, const wchar_t* const fmt, va_list args)
{
	wchar_t buff_[BUFF_SIZE];
	wchar_t buff[BUFF_SIZE];
	memset(buff_, 0, BUFF_SIZE);
	memset(buff, 0, BUFF_SIZE);
	_vsnwprintf_s(buff_, BUFF_SIZE, fmt, args);
	_snwprintf_s(buff, BUFF_SIZE, L"[%s] %s\n", tag, buff_);

	const auto str = toMultiByte(buff);
	SetLastError(0);
	
	DWORD wlen;
	if( WriteFile(GetStdHandle(stream), str.c_str(), static_cast<DWORD>(str.size()), &wlen, nullptr) == 0 ){
		const DWORD errCode = GetLastError();
		errDlg(errCode, tag, L"コンソールに書けません！\nエラーコード： %d", errCode);
	}

	if(enableDiag){
		MessageBoxW(NULL, buff_, (std::wstring(L"[エラー] ")+tag).c_str(),MB_OK | MB_ICONERROR);
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
void logMsg(const wchar_t* const tag, const wchar_t* const fmt, ...)
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