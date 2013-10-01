std::wstring getDirname(wchar_t* dir);
std::string toMultiByte(std::wstring const& s);
bool fileExists(std::string const& path);
bool fileExists(std::wstring const& path);
void showErrorDialog(std::wstring const& title, std::wstring const& contents, ...);
void showErrorDialog(std::string const& title, std::string const& contents, ...);
void logMessage(std::string const& tag, std::string const& fmt, ...);
void logMessage(std::wstring const& tag, std::wstring const& fmt, ...);

#define CERR(...) showErrorDialog(__VA_ARGS__);
#define COUT(...) logMessage(__VA_ARGS__);
