#pragma once

#include <SDL2/SDL_log.h>

#if defined(ANDROID)
#define TAG "Game"
#include <android/log.h>
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#else

#ifndef LOG_CATEGORY
#define LOG_CATEGORY SDL_LOG_CATEGORY_APPLICATION
#endif

#define LOGI(...) SDL_LogInfo(LOG_CATEGORY, __VA_ARGS__)
#define LOGW(...) SDL_LogWarn(LOG_CATEGORY, __VA_ARGS__)
#define LOGE(...) SDL_LogError(LOG_CATEGORY, __VA_ARGS__)
#define LOGD(...) SDL_LogDebug(LOG_CATEGORY, __VA_ARGS__)

#endif