#if defined(__GNUC__)
#pragma GCC system_header
#endif

// to avoid any clash if PROJ users have another version of nlohmann/json.hpp
#define nlohmann proj_nlohmann

#if !defined(DOXYGEN_ENABLED)
#include "nlohmann/json.hpp"
#endif
