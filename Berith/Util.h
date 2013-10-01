

std::wstring getDirname(wchar_t* dir);
std::string toMultiByte(std::wstring const& s);
bool fileExists(std::string const& path);
bool fileExists(std::wstring const& path);

void errMsg(const wchar_t* const tag, const wchar_t* const fmt, ...);
void errMsg(const char* const tag, const char* const fmt, ...);
void logMsg(const wchar_t* const tag, const wchar_t* const fmt, ...);
void logMsg(const char* const tag, const char* const fmt, ...);
