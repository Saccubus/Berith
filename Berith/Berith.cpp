<<<<<<< HEAD
// Berith.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "JVM.h"
#include "Util.h"

std::string const MainKlass = std::string("saccubus/Saccubus");
std::string const JarFilename = std::string("Saccubus.jar");

int mainImpl(std::wstring const& moduleFilename, int argc, wchar_t** argv)
{
	std::vector<std::string> vmArgs;
	std::vector<std::wstring> progArgs;
	
	std::string const progDir = toMultiByte(getDirname(moduleFilename.c_str()));
	logMsg(L"main", L"self: %s", moduleFilename.c_str() );
	logMsg(L"main", L"argc: %d", argc);
	for( int i=0; i<argc; ++i ) {
		logMsg(L"main", L"[%d] \"%s\"", i, argv[i]);
		if(i>=1){
			progArgs.emplace_back(argv[i]);
		}
	}
	std::string const jarFilename = progDir+JarFilename;
	if( !fileExists(jarFilename) ){
		errMsg("main","Jar not found! => \"%s\"", jarFilename.c_str());
		return -1;
	}
	vmArgs.push_back(std::string("-Djava.class.path=")+jarFilename);
	//vmArgs.push_back("-Djava.compiler=NONE");
	//vmArgs.push_back("-verbose:jni");
	bool const r = withJava(vmArgs, progArgs, [&progArgs](JavaVM* vm, JNIEnv* env) -> bool {
		jclass klass = env->FindClass( MainKlass.c_str() );
		if(!klass) {
			errMsg("main", "Klass %s not found.", MainKlass.c_str());
			return false;
		}
		jmethodID method = env->GetStaticMethodID(klass, "main", "([Ljava/lang/String;)V");
		if(!method) {
			errMsg("main", "method main not found.");
			return false;
		}
		jobjectArray array = (jobjectArray)env->NewObjectArray(static_cast<jsize>(progArgs.size()), env->FindClass("java/lang/String"), env->NewStringUTF(""));
		size_t const max = progArgs.size();
		for(size_t i=0;i<max;++i){
			env->SetObjectArrayElement(array, static_cast<jsize>(i), env->NewStringUTF(reinterpret_cast<const char*>(progArgs[i].c_str())));
		}
		env->CallStaticVoidMethod(klass, method, array);
		env->DeleteLocalRef( klass );
		return true;
	});
	return r ? 0 : 1;
}

// Consoleモードにした時用の仮エントリポイント
// 余計にある分には困らないです
int main(int argc, wchar_t** argv)
{
	initUtil();
	logMsg("main", "Launching");
	int const r = mainImpl(argv[0], argc-1, &argv[1]);
	closeUtil();
	return r;
}

//int _tmain(int argc, wchar_t* argv[])
int WINAPI WinMain(
	HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	LPSTR lpCmdLine, 
	int nCmdShow
)
{
	wchar_t buff[8192];
	GetModuleFileName(GetModuleHandle(NULL), buff, 8192);
	int argc;
	wchar_t** argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	if(SW_NORMAL == nCmdShow) {
		STARTUPINFO startupInfo;
		PROCESS_INFORMATION processInfo;
		ZeroMemory(&startupInfo,sizeof(startupInfo));
		startupInfo.cb = sizeof(startupInfo);
		startupInfo.dwFlags = STARTF_USESHOWWINDOW;
		startupInfo.wShowWindow = SW_HIDE;
		if (0 == CreateProcessW(
				buff,
				NULL,
				NULL,
				NULL,
				TRUE,
				CREATE_NEW_CONSOLE,
				NULL, NULL, &startupInfo, &processInfo)) {
			errMsg("main", "Failed to create virtual console.");
			return -1;
		}
		return 0;
	}
	initUtil();
	logMsg("main", "Launching");
	int const r = mainImpl(buff, argc-1, &argv[1]);
	closeUtil();
	return r;
}

=======
// Berith.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "JavaUtil.h"
#include "Util.h"

std::wstring const MainKlass = (L"saccubus/Saccubus");
std::wstring const JarFilename = (L"Saccubus.jar");

int mainImpl(std::wstring const& moduleFilename, int argc, wchar_t** argv)
{
	std::vector<std::wstring> vmArgs;
	std::vector<std::wstring> progArgs;
	
	std::wstring const progDir = getDirname(moduleFilename.c_str());
	logMsg(L"main", L"self: %s", moduleFilename.c_str() );
	logMsg(L"main", L"argc: %d", argc);
	for( int i=0; i<argc; ++i ) {
		logMsg(L"main", L"[%d] \"%s\"", i, argv[i]);
		if(i>=1){
			progArgs.emplace_back(argv[i]);
		}
	}
	std::wstring const jarFilename = progDir+JarFilename;
	if( !fileExists(jarFilename) ){
		errMsg(L"main",L"Jarファイルが見つかりません：\n\"%s\"", jarFilename.c_str());
		return -1;
	}
	vmArgs.push_back(std::wstring(L"-Djava.class.path=")+jarFilename);
	//vmArgs.push_back(L"-Djava.compiler=NONE");
	//vmArgs.push_back(L"-verbose:jni");
	bool const r = withJava(vmArgs, progArgs, [&progArgs](JavaVM* vm, JNIEnv* env)->bool{
		jclass klass = env->FindClass( toMultiByte(MainKlass).c_str() );
		if(!klass) {
			errMsg(L"main", L"クラスが見つかりません：\n\"%s\"", MainKlass.c_str());
			return false;
		}
		jmethodID method = env->GetStaticMethodID(klass, "main", "([Ljava/lang/String;)V");
		if(!method) {
			errMsg(L"main", L"クラス \"%s\" にmainメソッドが見つかりません", MainKlass.c_str());
			return false;
		}
		jobjectArray array = (jobjectArray)env->NewObjectArray(static_cast<jsize>(progArgs.size()), env->FindClass("java/lang/String"), env->NewStringUTF(""));

		size_t const max = progArgs.size();
		for(size_t i=0;i<max;++i){
			env->SetObjectArrayElement(array, static_cast<jsize>(i), env->NewStringUTF(reinterpret_cast<const char*>(progArgs[i].c_str())));
		}
		env->CallStaticVoidMethod(klass, method, array);
		return true;
	});
	return r ? 0 : 1;
}

// Consoleモードにした時用の仮エントリポイント
// 余計にある分には困らないです
int main(int argc, wchar_t** argv)
{
	ChangeStdHandle session;
	logMsg(L"main", L"ランチャ起動…");
	int const r = mainImpl(argv[0], argc-1, &argv[1]);
	return r;
}

//int _tmain(int argc, wchar_t* argv[])
int WINAPI WinMain(
	HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	LPSTR lpCmdLine, 
	int nCmdShow
)
{
	wchar_t buff[8192];
	GetModuleFileName(GetModuleHandle(NULL), buff, 8192);
	int argc;
	wchar_t** argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	if(SW_NORMAL == nCmdShow) {
		STARTUPINFO startupInfo;
		PROCESS_INFORMATION processInfo;
		ZeroMemory(&startupInfo,sizeof(startupInfo));
		startupInfo.cb = sizeof(startupInfo);
		startupInfo.dwFlags = STARTF_USESHOWWINDOW;
		startupInfo.wShowWindow = SW_HIDE;
		if (0 == CreateProcessW(
				buff,
				NULL,
				NULL,
				NULL,
				TRUE,
				CREATE_NEW_CONSOLE,
				NULL, NULL, &startupInfo, &processInfo)) {
			errDlg(GetLastError(), L"main", L"前段階CreateProcessWに失敗しました。\nエラーコード:%d", GetLastError());
			return -1;
		}
		return 0;
	}
	ChangeStdHandle session;
	logMsg(L"main", L"ランチャ起動…");
	int const r = mainImpl(buff, argc-1, &argv[1]);
	return r;
}

>>>>>>> aa4853266e08712f35c510b831dea6823ef09232
