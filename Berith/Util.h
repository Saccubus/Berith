std::wstring getDirname(wchar_t* dir);
std::string toMultiByte(std::wstring const& s);
bool fileExists(std::string const& path);
bool fileExists(std::wstring const& path);
void showErrorDialog(const wchar_t* const title, const wchar_t* const fmt, ...);
void showErrorDialog(const char* const title, const char* const fmt, ...);
void logMessage(std::string const& tag, std::string const& fmt, ...);
void logMessage(std::wstring const& tag, std::wstring const& fmt, ...);

#define CERR(...) showErrorDialog(__VA_ARGS__);
#define COUT(...) logMessage(__VA_ARGS__);
