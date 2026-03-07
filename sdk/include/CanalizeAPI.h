#pragma once

#if defined(_WIN32)
  #if defined(CANALIZE_EXPORTS)
    #define CANALIZE_API __declspec(dllexport)
  #else
    #define CANALIZE_API __declspec(dllimport)
  #endif
#else
  #define CANALIZE_API __attribute__((visibility("default")))
#endif
