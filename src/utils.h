#ifndef UTILS_H_
#define UTILS_H_

#include <math.h>
// #include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#define FPATH_MAX 4096
#define SECOND 1000

// ------------------------------------------------------------------------------------------------
//  Types
//

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;

// ------------------------------------------------------------------------------------------------
//  Logging
//

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

#define util_error(fmt, ...) util_err("ERROR [%s:%d]: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define util_info(fmt, ...) util_inf("INFO: " fmt "\n", ##__VA_ARGS__)
#define util_warn(fmt, ...) util_inf("WARN [%s:%d]: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define util_debug(fmt, ...) util_inf("DEBUG [%s:%d]: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define util_fatal(fmt, ...) util_fat("FATAL [%s:%d]: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#pragma clang diagnostic pop

void util_inf(const char* fmt, ...);
void util_err(const char* fmt, ...);
void util_fat(const char* fmt, ...);
void* util_malloc(size_t size, const char* fname, unsigned int lnum);
void util_free(void* ptr, const char* fname, unsigned int lnum);

// ------------------------------------------------------------------------------------------------
//  Memory
//

#define MAX_MEM_LOGS 1024

typedef struct {
    void* ptr;
    char fname[FPATH_MAX];
    unsigned int lnum;
    int used;
} MemLog;

#define DBG_MALLOC(sz) debug_malloc((sz), __FILE__, __LINE__)
#define DBG_FREE(p) debug_free((p), __FILE__, __LINE__)

// ------------------------------------------------------------------------------------------------
//  Math
//

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static inline f32 clamp_f(f32 v, f32 lo, f32 hi)
{
    return fminf(fmaxf(v, lo), hi);
}

// ------------------------------------------------------------------------------------------------
//  Timer
//

typedef struct {
    f64 start_time;
    f64 lifetime;
} Timer;

static void timer_start(Timer* t, const f64 start_time, const f64 lifetime)
{
    if (t->start_time != 0.0f || t->lifetime != 0.0f) {
        return;
    }
    t->start_time = start_time;
    t->lifetime = lifetime;
}

static void timer_stop(Timer* t)
{
    t->start_time = 0.0f;
    t->lifetime = 0.0f;
}

static bool timer_done(Timer* t, const f64 now)
{
    return now - t->start_time >= t->lifetime;
}

static f64 timer_get_elapsed(Timer* t, const f64 now)
{
    f64 elapsed = now - t->start_time;
    return elapsed < 0 ? 0 : elapsed;
}

#endif // UTILS_H_
