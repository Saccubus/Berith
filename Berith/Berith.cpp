// Berith.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "JVM.h"
#include "Util.h"

std::string const MainKlass = std::string("saccubus/Saccubus");
std::string const JarFilename = std::string("Saccubus.jar");


int _tmain(int argc, wchar_t* argv[])
{

	std::vector<std::string> vmArgs;
	std::vector<std::wstring> progArgs;
	std::string progDir(toMultiByte(getDirname(argv[0])));
	for( int i=0; i<argc; ++i ) {
		fwprintf(stdout, (L"[main] [%d] \"%s\"\n"), i, argv[i]);
		if(i>=1){
			progArgs.emplace_back(argv[i]);
		}
	}
	std::string jarFilename = progDir+JarFilename;
	if(!fileExists(jarFilename)){
		CERR("main","Jar not found!: %s\n", jarFilename.c_str());
		return -1;
	}
	vmArgs.push_back(std::string("-Djava.class.path=")+jarFilename);
	vmArgs.push_back("-Djava.compiler=NONE");
	vmArgs.push_back("-verbose:jni");
	withJava(vmArgs, progArgs, [&progArgs](JavaVM* vm, JNIEnv* env)->bool{
		jclass klass = env->FindClass( MainKlass.c_str() );
		if(!klass) {
			CERR("main", "Klass %s not found.\n", MainKlass.c_str());
			return false;
		}
		jmethodID method = env->GetStaticMethodID(klass, "main", "([Ljava/lang/String;)V");
		if(!method) {
			CERR("main", "method main not found.\n");
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
	getc(stdin);
	return 0;
}

