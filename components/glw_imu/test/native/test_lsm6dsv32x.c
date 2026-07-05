#include "native_test_common.h"

#include "glw_imu.h"
#include "glw_imu/errors.h"
#include "bus_mock.h"

#define DEVICE_ID 0x70U

static uint8_t fake_device_id = DEVICE_ID;
static glw_imu_error_t bus_read_fake(uint8_t addr, uint8_t *buffer, uint16_t len) {
    if (addr == 0x0F && len == 1) *buffer = fake_device_id;
    return 0;
}

void setUp() {
    reset_all_fakes();
    FFF_RESET_HISTORY();
}

void tearDown() {}

TEST_CASE(imu_init_failed_bus_init) {
    glw_imu_bus_init_fake.return_val = GLW_IMU_BUS_ERROR;
    glw_imu_error_t ret = glw_imu_init();
    TEST_ASSERT_EQUAL(GLW_IMU_BUS_ERROR, ret);
}

TEST_CASE(imu_init_failed_bus_read) {
    glw_imu_bus_init_fake.return_val = 0;
    glw_imu_bus_read_fake.return_val = GLW_IMU_BUS_ERROR;
    glw_imu_error_t ret = glw_imu_init();
    TEST_ASSERT_EQUAL(GLW_IMU_BUS_ERROR, ret);
}

TEST_CASE(imu_init_wrong_device_id) {
    fake_device_id = 0x60U;
    glw_imu_bus_read_fake.return_val = 0x60;
    glw_imu_bus_read_fake.custom_fake = bus_read_fake;
    glw_imu_error_t ret = glw_imu_init();
    fake_device_id = 0x70U;
    TEST_ASSERT_EQUAL(GLW_IMU_DEVICE_ERROR, ret);
}

TEST_CASE(imu_init_success) {
    glw_imu_bus_init_fake.return_val = 0;
    fake_device_id = 0x70U;
    glw_imu_bus_read_fake.custom_fake = bus_read_fake;
    glw_imu_error_t ret = glw_imu_init();
    TEST_ASSERT_EQUAL(GLW_IMU_OK, ret);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_imu_init_success);
    RUN_TEST(test_imu_init_failed_bus_init);
    RUN_TEST(test_imu_init_failed_bus_read);
    RUN_TEST(test_imu_init_wrong_device_id);

    return UNITY_END();
}
