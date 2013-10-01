#include "stdafx.h"
bool withJava(std::vector<std::string> vmArgs, std::vector<std::wstring> progArgs,std::function<bool(JavaVM* vm, JNIEnv* env)> const& f);
