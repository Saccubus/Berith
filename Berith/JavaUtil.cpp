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
		warnMsg(L"getJavaPath", L"$JAVA_HOME�͐ݒ肳��Ă��܂���B");
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
		logMsg(L"getJavaPath", L"Java�C���X�g�[����񃌃W�X�^���ݒ肳��Ă��܂���B");
	}
	errMsg(L"JVM", L"Java���C���X�g�[������Ă��Ȃ��悤�ł��B");
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
			errMsg(L"loadJvm", L"jvm.dll��������܂���");
			return nullptr;
		}
		if( !hinstLib ) {
			errMsg(L"loadJvm", L"jvm.dll�̓ǂݍ��݂Ɏ��s���܂����B\n32bit/64bit����v���Ă��邩�m���߂āI");
			return nullptr;
		}
		ptrCreateJavaVM = (PtrCreateJavaVM)GetProcAddress(hinstLib,"JNI_CreateJavaVM");
		if( !ptrCreateJavaVM ) {
			errMsg(L"loadJvm", L"JNI_CreateJavaVM�̃A�h���X�̎擾�Ɏ��s���܂����B.\nJavaVM�����Ă�̂�������܂���B\nJAVA_HOME=\"%s\"", javaPath.c_str() );
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
	{ //����������
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

		{ //vm��env���擾����
			jint ret = initFunc(&vm, (void**)&env, &vm_args);
			if (ret != JNI_OK) {
				errMsg(L"withJava", L"VM�̍쐬�Ɏ��s���܂���\n�G���[�R�[�h�F%d", ret);
				return false;
			}
		}
		logMsg(L"withJava", L"VM����������");
		bool const r = f(vm, env);
		vm->DestroyJavaVM();
		logMsg(L"withJava", L"VM�j������");
		return r;
	}
}
