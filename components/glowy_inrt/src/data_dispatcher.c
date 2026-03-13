#include "glowy_common/os/mutex.h"

// implements
#include "glowy_inrt/data_dispatcher.h"
#include "data_dispatcher_priv.h"

static const int velocity_multiplier = 1000;

static struct {
    glowy_inrt_vel_t vert_velocity;
} public_state;

static os_mutex_handle_t public_state_mutex;

void glowy_inrt_dispatcher_init(void) {
    public_state_mutex = os_mutex_create();
}

void glowy_inrt_dispatcher_update(glowy_inrt_state_t *state) {
    os_mutex_take(public_state_mutex, os_max_timeout);
    public_state.vert_velocity = state->z_velocity * velocity_multiplier;
    os_mutex_give(public_state_mutex);
}

glowy_inrt_vel_t glowy_inrt_get_vertical_velocity(void) {
    os_mutex_take(public_state_mutex, os_max_timeout);
    glowy_inrt_vel_t vel = public_state.vert_velocity;
    os_mutex_give(public_state_mutex);

    return vel;
}
