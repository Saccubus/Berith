#include "stdafx.h"
#include "Util.h"
#include <cassert>

static FILE* fperr = nullptr;
static FILE* fpout = nullptr;
void initUtil()
{
	// FIXME: XXX: ?????
	// �Ȃ����ŏ���freopen����������Ă��܂��Bstdin���g��Ȃ��̂ł��܂���
	{
		FILE* f;
		errno_t e = _wfreopen_s(&f, L"__", L"r", stdin);
	}
	{
		errno_t e = _wfreopen_s(&fperr, L"stderr.log", L"w+", stderr);
		if( e != 0 ){
			errMsg("main", "Failed to redirect stderr: %d", e);
		}
		assert( stderr == fperr );
	}
	warnMsg("init", "stderr reopened.");
	{ //redirect stdout/stderr to file.
		errno_t e = _wfreopen_s(&fpout, L"stdout.log", L"w+", stdout);
		if( e != 0 ){
			errMsg("main", "Failed to redirect stdout: %d", e);
		}
		assert( stdout == fpout );
	}
	fpout = stdout;
	logMsg("init", "stdout reopened.");
}
void closeUtil()
{
	fclose(fperr);
	fclose(fpout);
}

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
	wchar_t buff[1024*16];
	vswprintf_s(buff, fmt, args);
	fwprintf_s(stream, L"[%s] %s\n", tag, buff);
	fflush(stream);
	fflush(stream);
	fflush(stream);

	if(enableDiag){
		MessageBoxW(NULL, buff, (std::wstring(L"Error at ")+tag).c_str(),MB_OK | MB_ICONERROR);
	}
}

static void outMsg(FILE*stream, bool enableDiag, const char* const tag, const char* const fmt, va_list args)
{
	char buff[1024*16];
	vsprintf_s(buff, fmt, args);
	fprintf_s(stream, "[%s] %s\n", tag, buff);
	fflush(stream);
	fflush(stream);
	fflush(stream);

	if(enableDiag){
		MessageBoxA(NULL, buff, (std::string("Error at ")+tag).c_str(),MB_OK | MB_ICONERROR);
	}
}

void errMsg(const wchar_t* const tag, const wchar_t* const fmt, ...)
{
	assert( stderr == fperr );
	va_list args;
	va_start(args, fmt);
	outMsg(fperr, true, tag, fmt, args);
	va_end(args);
}

void errMsg(const char* const tag, const char* const fmt, ...)
{
	assert( stderr == fperr );
	va_list args;
	va_start(args, fmt);
	outMsg(fperr, true, tag, fmt, args);
	va_end(args);
}

void logMsg(const wchar_t* const tag, const wchar_t* const fmt, ...)
{
	assert( stdout == fpout );
	va_list args;
	va_start(args, fmt);
	outMsg(fpout, false, tag, fmt, args);
	va_end(args);
}

void logMsg(const char* const tag, const char* const fmt, ...)
{
	assert( stdout == fpout );
	va_list args;
	va_start(args, fmt);
	outMsg(fpout, false, tag, fmt, args);
	va_end(args);
}

void warnMsg(const wchar_t* const tag, const wchar_t* const fmt, ...)
{
	assert( stderr == fperr );
	va_list args;
	va_start(args, fmt);
	outMsg(fperr, false, tag, fmt, args);
	va_end(args);
}

void warnMsg(const char* const tag, const char* const fmt, ...)
{
	assert( stderr == fperr );
	va_list args;
	va_start(args, fmt);
	outMsg(fperr, false, tag, fmt, args);
	va_end(args);
}
