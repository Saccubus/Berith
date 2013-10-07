
class ChangeStdHandle final{
private:
	HANDLE fhandle_;
	void init(std::wstring const& fname);
public:
	ChangeStdHandle(std::wstring const& fname);
	ChangeStdHandle();
	~ChangeStdHandle();
};

std::wstring getDirname(const wchar_t* dir);
std::string toMultiByte(std::wstring const& s);
bool fileExists(std::string const& path);
bool fileExists(std::wstring const& path);

void errDlg(DWORD errCode, const wchar_t* const tag, const wchar_t* const fmt, ...);

void errMsg(const wchar_t* const tag, const wchar_t* const fmt, ...);
void logMsg(const wchar_t* const tag, const wchar_t* const fmt, ...);
void warnMsg(const wchar_t* const tag, const wchar_t* const fmt, ...);
