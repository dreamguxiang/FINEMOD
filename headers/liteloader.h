#pragma once
#include <string>
#ifdef LITELOADER_EXPORTS
#    define LIAPI __declspec(dllexport)
#else
#    define LIAPI __declspec(dllexport)
#endif
typedef unsigned char uchar;
#define LiteLoaderVersion "1.1.1"
#define LiteLoaderVersionNum (unsigned short)11

namespace loaderapi {
LIAPI std::string getLoaderVersion();
LIAPI unsigned short getLoaderVersionNum();
};  // namespace loaderapi

#define LiteLoaderVersionGithub LiteLoaderVersion"-Ci-29028f2_1627276036"
