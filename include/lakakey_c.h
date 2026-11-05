#ifndef LAKAKEY_C_H
#define LAKAKEY_C_H
#include <stdint.h>
#if defined(_WIN32) && defined(LAKAKEY_IMPORT_DLL)
#define LK_API __declspec(dllimport)
#elif defined(_WIN32) && defined(LAKAKEY_BUILD_DLL)
#define LK_API __declspec(dllexport)
#else
#define LK_API
#endif
#ifdef __cplusplus
extern "C" {
#endif
// Opaque handle, one per focused input context; results valid until next call on handle.
typedef struct lk_handle lk_handle;
typedef struct lk_result { const char* commit; const char* preedit; int consumed; } lk_result;
// scheme 0=Telex, 1=VNI, 2=Plain; tone_style 0=Modern, 1=Traditional
LK_API lk_handle* lk_create(int scheme, int tone_style);
LK_API void lk_destroy(lk_handle* handle);
LK_API lk_result lk_feed(lk_handle* handle, uint32_t unicode_scalar);
LK_API lk_result lk_backspace(lk_handle* handle);
LK_API lk_result lk_commit(lk_handle* handle);
LK_API void lk_reset(lk_handle* handle);
LK_API void lk_configure(lk_handle* handle, int scheme, int tone_style, int enabled);
#ifdef __cplusplus
}
#endif
#endif
