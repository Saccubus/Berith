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
