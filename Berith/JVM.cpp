#include "stdafx.h"
#include "JVM.h"

bool withJava(std::vector<std::string> vmArgs, std::vector<tstring> progArgs,std::function<bool(JavaVM* vm, JNIEnv* env)> const& f)
{
	JavaVMInitArgs vm_args;
	vm_args.version = JNI_VERSION_1_6;
	{ //èâä˙âªÇ∑ÇÈ
		jint ret = JNI_GetDefaultJavaVMInitArgs(&vm_args);
		if( ret < 0 ){
		}
	}
	COUT("withJava", "vm_args.version:%x\n", vm_args.version);

	std::vector<JavaVMOption> opts;
	for( std::string const& e : vmArgs ) {
		JavaVMOption op;
		op.optionString = const_cast<char*>(e.c_str());
		opts.push_back( op );
	}

	vm_args.nOptions = opts.size();
	vm_args.options  = opts.data();

	JavaVM* vm;
	JNIEnv* env;

	{ //vmÇ∆envÇéÊìæÇ∑ÇÈ
		jint ret = JNI_CreateJavaVM(&vm, (void**)&env, &vm_args);
		if (ret != JNI_OK) {
			printf("create vm error:%d\n", ret);
			return ret;
		}
	}
	vm->AttachCurrentThread((void**)env, nullptr);
	bool const r = f(vm, env);
	vm->DetachCurrentThread();
	vm->DestroyJavaVM();
	return r;
}
