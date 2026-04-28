#include "glw_common/os/mutex.h"

// implements
#include "glw_inrt/data_dispatcher.h"
#include "data_dispatcher_priv.h"

static const int velocity_multiplier = 1000;

static struct {
    glw_inrt_vel_t vert_velocity;
} public_state;

static os_mutex_handle_t public_state_mutex;

void glw_inrt_dispatcher_init(void) {
    public_state_mutex = os_mutex_create();
}

void glw_inrt_dispatcher_update(glw_inrt_state_t *state) {
    os_mutex_take(public_state_mutex, os_max_timeout);
    public_state.vert_velocity = state->z_velocity * velocity_multiplier;
    os_mutex_give(public_state_mutex);
}

glw_inrt_vel_t glw_inrt_get_vertical_velocity(void) {
    os_mutex_take(public_state_mutex, os_max_timeout);
    glw_inrt_vel_t vel = public_state.vert_velocity;
    os_mutex_give(public_state_mutex);

    return vel;
}
