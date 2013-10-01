#include "stdafx.h"

typedef std::basic_string<_TCHAR> tstring;
#define CERR(tag, fmt, ...) fprintf(stderr, "[" tag "] " fmt, __VA_ARGS__);
#define COUT(tag, fmt, ...) fprintf(stderr, "[" tag "] " fmt, __VA_ARGS__);

bool withJava(std::vector<std::string> vmArgs, std::vector<tstring> progArgs,std::function<bool(JavaVM* vm, JNIEnv* env)> const& f);
