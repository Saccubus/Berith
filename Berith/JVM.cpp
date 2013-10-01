#include "stdafx.h"
#include "JVM.h"
#include "Util.h"

static void error(std::string const& title, std::string const& msg)
{
}

typedef jint (JNICALL *PtrCreateJavaVM)(JavaVM **, void **, void *);

static std::wstring getJavaPath()
{
	{
		wchar_t buff[8192];
		wmemset(buff, 0, 8192);
		size_t size;
		if(0 == _wgetenv_s(&size, buff, L"JAVA_HOME") && size > 0){
			return buff;
		}
		COUT("getJavaPath", "$JAVA_HOME is not set.");
	}
	{
		HKEY key;
		RegOpenKeyEx(HKEY_LOCAL_MACHINE,L"SOFTWARE\\JavaSoft\\Java Runtime Environment", 0, KEY_READ, &key);
		wchar_t buff[8192];
		DWORD len=8192;
		std::wstring r;
		std::wstring ans;
		RegGetValue(key, nullptr, L"CurrentVersion", RRF_RT_REG_SZ, nullptr, buff, &len);
		if(len > 0){
			r = std::wstring(buff);
			len=8192;
			RegGetValue(key, r.c_str(), L"JavaHome", RRF_RT_REG_SZ, nullptr, buff, &len);
			if(len > 0){
				ans = std::wstring(buff);
			}
		}
		RegCloseKey(key);
		if( !ans.empty() ){
			return ans;
		}
		COUT("getJavaPath", "Registory value is not set.");
	}
	CERR("JVM", "Java VM not installed.");
	return L"";
}

static PtrCreateJavaVM loadJvm()
{
	static PtrCreateJavaVM ptrCreateJavaVM = nullptr;
	if(!ptrCreateJavaVM){
		std::wstring const javaPath ( getJavaPath() );
		if( javaPath.empty() ) {
			return nullptr;
		}
		std::wstring const clientJvm = javaPath + L"\\bin\\client\\jvm.dll";
		std::wstring const serverJvm = javaPath + L"\\bin\\server\\jvm.dll";
		
		HINSTANCE hinstLib = nullptr;
		if(fileExists(clientJvm)){
			hinstLib = LoadLibrary(clientJvm.c_str());
		}else if(fileExists(serverJvm)){
			hinstLib = LoadLibrary(serverJvm.c_str());
		}else{
			CERR("loadJvm", "failed to open jvm.dll");
			return nullptr;
		}
		if( !hinstLib ) {
			CERR("loadJvm", "failed to LoadLibrary.\nArchtecture (32bit/64bit) may be mismatched.");
			return nullptr;
		}
		ptrCreateJavaVM = (PtrCreateJavaVM)GetProcAddress(hinstLib,"JNI_CreateJavaVM");
		if( !ptrCreateJavaVM ) {
			CERR("loadJvm", "failed to Get JNI_CreateJavaVM addr.\nMaybe jvm library is broken.\nJAVA_HOME=\"%s\"", javaPath.c_str() );
			return nullptr;
		}
	}
	return ptrCreateJavaVM;
}

bool withJava(std::vector<std::string> vmArgs, std::vector<std::wstring> progArgs,std::function<bool(JavaVM* vm, JNIEnv* env)> const& f)
{
	std::vector<JavaVMOption> opts;
	PtrCreateJavaVM initFunc = loadJvm();
	if(!initFunc){
		return false;
	}
	for( std::string const& e : vmArgs ) {
		JavaVMOption op;
		op.optionString = const_cast<char*>(e.c_str());
		op.extraInfo = 0;
		opts.push_back( op );
	}

	JavaVMInitArgs vm_args;
	vm_args.version = JNI_VERSION_1_6;
	vm_args.ignoreUnrecognized=false;
	vm_args.nOptions = static_cast<jint>(opts.size());
	vm_args.options  = opts.data();
/*
	{ //初期化する
		jint ret = JNI_GetDefaultJavaVMInitArgs(&vm_args);
		if (ret != JNI_OK) {
			CERR("withJava", "Failed to open jvm: %d", ret);
			return false;
		}
	}
*/
	COUT("withJava", "vm_args.version:%x\n", vm_args.version);

	{
		JavaVM* vm;
		JNIEnv* env;

		{ //vmとenvを取得する
			jint ret = initFunc(&vm, (void**)&env, &vm_args);
			if (ret != JNI_OK) {
				CERR("withJava", "create vm error:%d\n", ret);
				return false;
			}
		}
		bool const r = f(vm, env);
		vm->DestroyJavaVM();
		COUT("withJava", "VM destroyed");
		return r;
	}
}
