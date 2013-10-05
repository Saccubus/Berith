#include "stdafx.h"
#include "JavaUtil.h"
#include "Util.h"

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
		warnMsg(L"getJavaPath", L"$JAVA_HOMEは設定されていません。");
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
		logMsg(L"getJavaPath", L"Javaインストール情報レジスタが設定されていません。");
	}
	errMsg(L"JVM", L"Javaがインストールされていないようです。");
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
			errMsg(L"loadJvm", L"jvm.dllが見つかりません");
			return nullptr;
		}
		if( !hinstLib ) {
			errMsg(L"loadJvm", L"jvm.dllの読み込みに失敗しました。\n32bit/64bitが一致しているか確かめて！");
			return nullptr;
		}
		ptrCreateJavaVM = (PtrCreateJavaVM)GetProcAddress(hinstLib,"JNI_CreateJavaVM");
		if( !ptrCreateJavaVM ) {
			errMsg(L"loadJvm", L"JNI_CreateJavaVMのアドレスの取得に失敗しました。.\nJavaVMが壊れてるのかもしれません。\nJAVA_HOME=\"%s\"", javaPath.c_str() );
			return nullptr;
		}
	}
	return ptrCreateJavaVM;
}

bool withJava(std::vector<std::wstring> vmArgs, std::vector<std::wstring> progArgs,std::function<bool(JavaVM* vm, JNIEnv* env)> const& f)
{
	std::vector<JavaVMOption> opts;
	PtrCreateJavaVM initFunc = loadJvm();
	if(!initFunc){
		return false;
	}
	for( std::wstring const& e : vmArgs ) {
		JavaVMOption op;
		op.optionString = const_cast<char*>(toMultiByte(e).c_str());
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
			errMsg("withJava", "Failed to open jvm: %d", ret);
			return false;
		}
	}
	COUT("withJava", "vm_args.version:%x\n", vm_args.version);
*/

	{
		JavaVM* vm;
		JNIEnv* env;

		{ //vmとenvを取得する
			jint ret = initFunc(&vm, (void**)&env, &vm_args);
			if (ret != JNI_OK) {
				errMsg(L"withJava", L"VMの作成に失敗しました\nエラーコード：%d", ret);
				return false;
			}
		}
		logMsg(L"withJava", L"VM初期化完了");
		bool const r = f(vm, env);
		vm->DestroyJavaVM();
		logMsg(L"withJava", L"VM破棄完了");
		return r;
	}
}
