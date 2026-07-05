#pragma once

// ==== logging

#include "glw_log.h" // IWYU pragma: export
SET_LOG_TAG("glw_imu");

// ==== error checking and assertions

/**
 * PROPAGATE_ERROR(x)
 * PROPAGATE_ERROR_AS(x, err_code)
 *
 * These macros can be used to wrap function calls that return an integer error code.
 * If the function returns a non-zero code, logs an error with the code location and
 * returns an error code from the current function.
 * PROPAGATE_ERROR returns the same error code from the wrapped function.
 * PROPAGATE_ERROR_AS returns the given error code
 */
#define PROPAGATE_ERROR(x)                                                                                   \
    do {                                                                                                     \
        int ret_ = x;                                                                                        \
        if (ret_ != 0) {                                                                                     \
            LOGE_LOC("Call to " #x " failed with error code: %i", ret_);                                     \
            return ret_;                                                                                     \
        }                                                                                                    \
    } while (0)

#define PROPAGATE_ERROR_AS(x, err_code)                                                                      \
    do {                                                                                                     \
        int ret_ = x;                                                                                        \
        if (ret_ != 0) {                                                                                     \
            LOGE_LOC("Call to " #x " failed with error code: %i", ret_);                                     \
            return err_code;                                                                                 \
        }                                                                                                    \
    } while (0)

/**
 * ASSERT_OR_RETURN(x, err_code)
 * If the expression is false, log an error and return the given error code
 * */
#define ASSERT_OR_RETURN(x, err_code)                                                                        \
    do {                                                                                                     \
        if (!(x)) {                                                                                          \
            LOGE_LOC("Assertion error: " #x);                                                                \
            return err_code;                                                                                 \
        }                                                                                                    \
    } while (0)
