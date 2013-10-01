// Berith.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "JVM.h"

std::string const mainKlass = std::string("");

int _tmain(int argc, wchar_t* argv[])
{
	std::vector<std::string> vmArgs;
	std::vector<std::wstring> progArgs;
	for( int i=0; i<argc; ++i ) {
		progArgs.emplace_back(argv[i]);
	}
	withJava(vmArgs, progArgs, [&progArgs](JavaVM* vm, JNIEnv* env)->bool{
		jclass klass = env->FindClass( mainKlass.c_str() );
		if(!klass) {
			return false;
		}
		jmethodID method = env->GetMethodID(klass, "main", "([Ljava/lang/String;)V");
		if(!method) {
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
	return 0;
}

