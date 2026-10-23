#pragma once

#if defined(_WIN32)
  #define WHEEL_OF_FEAR_MODULE_EXPORT extern "C" __declspec(dllexport)
#else
  #define WHEEL_OF_FEAR_MODULE_EXPORT                                          \
    extern "C" __attribute__((visibility("default")))
#endif
