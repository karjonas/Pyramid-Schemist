#include "paths.h"

#include <cstdlib>

#ifndef DATA_DIR
#define DATA_DIR "../data"
#endif

std::string get_full_data_path(std::string filename)
{
    auto appDir = getenv("APPDIR");
    const std::string prefix = appDir != nullptr ? appDir : "";
    const auto result = prefix + DATA_DIR + "/" + filename;
    return result;
}
