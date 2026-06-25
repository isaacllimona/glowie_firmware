#pragma once

/** Callbacks for receiving data from the host */
typedef struct {
    void (*command_cb)(char *input); //< get the received command as a null-terminated string.
} glw_comms_callbacks_t;

typedef struct comms_init_t {
    glw_comms_callbacks_t callbacks;
} glw_comms_init_t;

void glw_comms_init(glw_comms_init_t init);

void glw_comms_stop(void);
