#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static MemLog mlog[MAX_MEM_LOGS];
static size_t mlogCount = 0;
// static pthread_mutex_t mlogMutex = PTHREAD_MUTEX_INITIALIZER;

static void add_log(void* ptr, const char* fname, unsigned int lnum);
static void remove_log(void* ptr);

void* util_malloc(size_t size, const char* fname, unsigned int lnum)
{
    void* p = malloc(size);
    if (!p) {
        util_error("malloc failed at %s:%u (size=%zu)\n", fname, lnum, size);
        return NULL;
    }

    if (getenv("DEBUG")) {
        // pthread_mutex_lock(&mlogMutex);
        add_log(p, fname, lnum);
        util_info("malloc at %s:%u [%zu live allocs]\n", fname, lnum, mlogCount);
        // pthread_mutex_unlock(&mlogMutex);
    }

    return p;
}

void util_free(void* ptr, const char* fname, unsigned int lnum)
{
    if (!ptr) {
        util_warn("debug_free called with NULL at %s:%u\n", fname, lnum);
        return;
    }

    free(ptr);

    if (getenv("DEBUG")) {
        // pthread_mutex_lock(&mlogMutex);
        remove_log(ptr);
        util_info("free at %s:%u [%zu live allocs]\n", fname, lnum, mlogCount);
        // pthread_mutex_unlock(&mlogMutex);
    }
}

void util_inf(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    vprintf(fmt, ap);

    va_end(ap);
}

void util_err(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    vprintf(fmt, ap);

    va_end(ap);
}

void util_fat(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    vprintf(fmt, ap);

    va_end(ap);

    exit(1);
}

// ------------------------------------------------------------------------------------------------

static void add_log(void* ptr, const char* fname, unsigned int lnum)
{
    if (mlogCount >= MAX_MEM_LOGS) {
        util_warn("mlog overflow – dropping allocation log for %s:%u\n", fname, lnum);
        return;
    }

    MemLog* e = &mlog[mlogCount];
    e->ptr = ptr;
    e->lnum = lnum;
    e->used = 1;

    snprintf(e->fname, sizeof(e->fname), "%s", fname);

    ++mlogCount;
}

static void remove_log(void* ptr)
{
    for (size_t i = 0; i < mlogCount; ++i) {
        if (mlog[i].used && mlog[i].ptr == ptr) {
            /* Compact the array – shift everything left */
            memmove(&mlog[i], &mlog[i + 1], (mlogCount - i - 1) * sizeof(MemLog));
            --mlogCount;
            return;
        }
    }
    util_warn("debug_free: pointer %p not found in log\n", ptr);
}
