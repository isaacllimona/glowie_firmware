#include <unity.h>

#include "test_glw_imu.h"
#include "test_glw_script.h"

int app_main(void) {
    UNITY_BEGIN();

    UnityPrint("Running glw_imu tests...");
    UNITY_PRINT_EOL();
    glw_imu_test_run_all();

    UnityPrint("Running glw_script tests...");
    UNITY_PRINT_EOL();
    glw_script_test_run_all();

    UNITY_END();

    return 0;
}
