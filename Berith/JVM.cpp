#include "stdafx.h"
#include "JVM.h"

static void error(std::string const& title, std::string const& msg)
{
	
}

bool withJava(std::vector<std::string> vmArgs, std::vector<std::wstring> progArgs,std::function<bool(JavaVM* vm, JNIEnv* env)> const& f)
{
	JavaVMInitArgs vm_args;
	vm_args.version = JNI_VERSION_1_6;
	{ //初期化する
		jint ret = JNI_GetDefaultJavaVMInitArgs(&vm_args);
		if (ret != JNI_OK) {
			CERR("withJava", "Failed to open jvm: %d", ret);
			return false;
		}
	}
	COUT("withJava", "vm_args.version:%x\n", vm_args.version);

	std::vector<JavaVMOption> opts;
	for( std::string const& e : vmArgs ) {
		JavaVMOption op;
		op.optionString = const_cast<char*>(e.c_str());
		opts.push_back( op );
	}

	vm_args.nOptions = static_cast<jint>(opts.size());
	vm_args.options  = opts.data();

	JavaVM* vm;
	JNIEnv* env;

	{ //vmとenvを取得する
		jint ret = JNI_CreateJavaVM(&vm, (void**)&env, &vm_args);
		if (ret != JNI_OK) {
			printf("create vm error:%d\n", ret);
			return false;
		}
	}
	bool const r = f(vm, env);
	vm->DestroyJavaVM();
	COUT("withJava", "VM destroyed", vm_args.version);
	return r;
}
