#include "glw_os/time.h"

// implements
#include "sync_clock.h"

static sync_clock_t offset;

static sync_clock_t get_system_time(void) { return os_time(); }

sync_clock_t sync_clock_get(void) { return get_system_time() + offset; }

void sync_clock_set(sync_clock_t time) { offset = time - get_system_time(); }
