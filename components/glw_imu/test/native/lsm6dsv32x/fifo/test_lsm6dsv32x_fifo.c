#include <assert.h>

#include "glw_imu/imu_data.h"
#include "native_test_common.h"

#include "bus_mock.h"
#include "config.h"
#include "glw_imu/errors.h"
#include "glw_imu/fifo.h"
#include "unity.h"

#define TEST_ASSERT_OK(x) TEST_ASSERT_EQUAL(0, x)

// ---- Helpers for encoding FIFO data as it would appear on the bus

#define GY_TAG_SENSOR 0x01
#define XL_TAG_SENSOR 0x02

#define TAG_BYTE(sensor, cnt) (uint8_t)(((sensor) << 3) | ((cnt) << 1))

#define FIFO_ENTRY(sensor, cnt, x, y, z)                                                                     \
    TAG_BYTE(sensor, cnt), (uint8_t)((x) & 0xFF), (uint8_t)(((x) >> 8) & 0xFF), (uint8_t)((y) & 0xFF),       \
        (uint8_t)(((y) >> 8) & 0xFF), (uint8_t)((z) & 0xFF), (uint8_t)(((z) >> 8) & 0xFF)

// ---- Bus read fake --------------------------------------------------------

#define FAKE_FIFO_SIZE GLW_IMU_FIFO_BUFFER_SIZE

static uint8_t fake_fifo[FAKE_FIFO_SIZE * 7]; // fake FIFO content
static uint8_t *fake_fifo_end;
static uint8_t *fake_fifo_ptr;
static uint8_t fake_fifo_level; // value returned for FIFO_STATUS1

static glw_imu_error_t bus_read_fake_for_fifo(uint8_t addr, uint8_t *buffer, uint16_t len) {
    if (addr == 0x1B) {
        // FIFO_STATUS1 + FIFO_STATUS2  (read in one 2-byte transfer)
        if (len >= 1)
            buffer[0] = fake_fifo_level;
        if (len >= 2)
            buffer[1] = 0;
    }

    if (addr == 0x78) {
        TEST_ASSERT(len % 7 == 0); // Otherwise would break the fake implementation
        for (int i = 0; i < len; i++) {
            if (fake_fifo_ptr != fake_fifo_end)
                buffer[i] = *(fake_fifo_ptr++);
            else
                buffer[i] = 0;
        }
    }

    return GLW_IMU_OK;
}

static void setup_fifo(const uint8_t fifo[], int count) {
    int fifo_bytes = count * 7;
    TEST_ASSERT(fifo_bytes <= sizeof(fake_fifo));

    fake_fifo_level = count;
    fake_fifo_ptr = fake_fifo;
    fake_fifo_end = fake_fifo + fifo_bytes;

    memcpy(fake_fifo, fifo, fifo_bytes);
}

// ---------------------------------------------------------------

void setUp(void) {
    reset_all_fakes();
    FFF_RESET_HISTORY();

    glw_imu_bus_read_fake.custom_fake = bus_read_fake_for_fifo;
}

void tearDown(void) {}

#define ASSERT_FRAME(frame, exp_flags, exp_gx, exp_gy, exp_gz, exp_ax, exp_ay, exp_az)                       \
    do {                                                                                                     \
        TEST_ASSERT_EQUAL(exp_flags, (frame).flags);                                                         \
        TEST_ASSERT_EQUAL(exp_gx, (frame).gyro[0]);                                                          \
        TEST_ASSERT_EQUAL(exp_gy, (frame).gyro[1]);                                                          \
        TEST_ASSERT_EQUAL(exp_gz, (frame).gyro[2]);                                                          \
        TEST_ASSERT_EQUAL(exp_ax, (frame).acc[0]);                                                           \
        TEST_ASSERT_EQUAL(exp_ay, (frame).acc[1]);                                                           \
        TEST_ASSERT_EQUAL(exp_az, (frame).acc[2]);                                                           \
    } while (0)

// ======================== test cases ====================================

TEST_CASE(fifo_read_empty) {
    setup_fifo(NULL, 0);

    glw_imu_fifo_fetch_result_t fetch_result;
    TEST_ASSERT_OK(glw_imu_fifo_fetch(&fetch_result));

    TEST_ASSERT_EQUAL(0, fetch_result.available_entries);
    TEST_ASSERT_EQUAL(0, fetch_result.fetched_entries);
    TEST_ASSERT_EQUAL(GLW_IMU_FIFO_BUFFER_SIZE, fetch_result.buffer_size);

    glw_imu_data_frame_t frame;
    for (int i = 0; i < 10; i++) {
        glw_imu_fifo_read_frame(&frame);
        TEST_ASSERT_EQUAL(0, frame.flags);
    }
}

TEST_CASE(fifo_read_single_pair) {
    const uint8_t entries[] = {
        FIFO_ENTRY(GY_TAG_SENSOR, 0, 10, -20, 30),
        FIFO_ENTRY(XL_TAG_SENSOR, 0, 100, -200, 300),
    };
    setup_fifo(entries, 2);

    glw_imu_fifo_fetch_result_t fetch_result;
    TEST_ASSERT_OK(glw_imu_fifo_fetch(&fetch_result));

    TEST_ASSERT_EQUAL(2, fetch_result.available_entries);
    TEST_ASSERT_EQUAL(2, fetch_result.fetched_entries);
    TEST_ASSERT_EQUAL(GLW_IMU_FIFO_BUFFER_SIZE, fetch_result.buffer_size);

    glw_imu_data_frame_t frame;
    glw_imu_fifo_read_frame(&frame);
    ASSERT_FRAME(frame, GLW_IMU_DATA_CONTAINS_GYRO | GLW_IMU_DATA_CONTAINS_ACC, 10, -20, 30, 100, -200, 300);

    glw_imu_fifo_read_frame(&frame);
    TEST_ASSERT_EQUAL(0, frame.flags);
}

TEST_CASE(fifo_read_two_frames) {
    // Two consecutive (gyro, acc) pairs with different cnt values
    const uint8_t entries[] = {
        FIFO_ENTRY(GY_TAG_SENSOR, 0, 10, -20, 30),
        FIFO_ENTRY(XL_TAG_SENSOR, 0, 100, -200, 300),
        FIFO_ENTRY(GY_TAG_SENSOR, 1, 40, -50, 60),
        FIFO_ENTRY(XL_TAG_SENSOR, 1, 400, -500, 600),
    };
    setup_fifo(entries, 4);

    glw_imu_fifo_fetch_result_t fetch_result;
    TEST_ASSERT_OK(glw_imu_fifo_fetch(&fetch_result));

    TEST_ASSERT_EQUAL(4, fetch_result.available_entries);
    TEST_ASSERT_EQUAL(4, fetch_result.fetched_entries);
    TEST_ASSERT_EQUAL(GLW_IMU_FIFO_BUFFER_SIZE, fetch_result.buffer_size);

    glw_imu_data_frame_t frame;

    // First frame
    glw_imu_fifo_read_frame(&frame);
    ASSERT_FRAME(frame, GLW_IMU_DATA_CONTAINS_GYRO | GLW_IMU_DATA_CONTAINS_ACC, 10, -20, 30, 100, -200, 300);

    // Second frame
    glw_imu_fifo_read_frame(&frame);
    ASSERT_FRAME(frame, GLW_IMU_DATA_CONTAINS_GYRO | GLW_IMU_DATA_CONTAINS_ACC, 40, -50, 60, 400, -500, 600);

    // No more frames
    glw_imu_fifo_read_frame(&frame);
    TEST_ASSERT_EQUAL(0, frame.flags);
}

TEST_CASE(fifo_read_gyro_only) {
    const uint8_t entries[] = {
        FIFO_ENTRY(GY_TAG_SENSOR, 0, 10, -20, 30),
    };
    setup_fifo(entries, 1);

    glw_imu_fifo_fetch_result_t fetch_result;
    TEST_ASSERT_OK(glw_imu_fifo_fetch(&fetch_result));

    TEST_ASSERT_EQUAL(1, fetch_result.available_entries);
    TEST_ASSERT_EQUAL(1, fetch_result.fetched_entries);
    TEST_ASSERT_EQUAL(GLW_IMU_FIFO_BUFFER_SIZE, fetch_result.buffer_size);

    glw_imu_data_frame_t frame = {0};
    glw_imu_fifo_read_frame(&frame);
    TEST_ASSERT_EQUAL(GLW_IMU_DATA_CONTAINS_GYRO, frame.flags);
    TEST_ASSERT_EQUAL(10, frame.gyro[0]);
    TEST_ASSERT_EQUAL(-20, frame.gyro[1]);
    TEST_ASSERT_EQUAL(30, frame.gyro[2]);

    // No more frames
    glw_imu_fifo_read_frame(&frame);
    TEST_ASSERT_EQUAL(0, frame.flags);
}

TEST_CASE(fifo_read_acc_only) {
    const uint8_t entries[] = {
        FIFO_ENTRY(XL_TAG_SENSOR, 0, 100, -200, 300),
    };
    setup_fifo(entries, 1);

    glw_imu_fifo_fetch_result_t fetch_result;
    TEST_ASSERT_OK(glw_imu_fifo_fetch(&fetch_result));

    TEST_ASSERT_EQUAL(1, fetch_result.available_entries);
    TEST_ASSERT_EQUAL(1, fetch_result.fetched_entries);
    TEST_ASSERT_EQUAL(GLW_IMU_FIFO_BUFFER_SIZE, fetch_result.buffer_size);

    glw_imu_data_frame_t frame = {0};
    glw_imu_fifo_read_frame(&frame);
    TEST_ASSERT_EQUAL(GLW_IMU_DATA_CONTAINS_ACC, frame.flags);
    TEST_ASSERT_EQUAL(100, frame.acc[0]);
    TEST_ASSERT_EQUAL(-200, frame.acc[1]);
    TEST_ASSERT_EQUAL(300, frame.acc[2]);

    // No more frames
    glw_imu_fifo_read_frame(&frame);
    TEST_ASSERT_EQUAL(0, frame.flags);
}

TEST_CASE(fifo_read_invalid_tag) {
    const uint8_t entries[] = {
        FIFO_ENTRY(GY_TAG_SENSOR, 0, 10, -20, 30),
        TAG_BYTE(0x03, 0),
        0,
        0,
        0,
        0,
        0,
        0, // unknown sensor tag
        FIFO_ENTRY(XL_TAG_SENSOR, 0, 100, -200, 300),
    };
    setup_fifo(entries, 3);

    glw_imu_fifo_fetch_result_t fetch_result;
    TEST_ASSERT_OK(glw_imu_fifo_fetch(&fetch_result));

    TEST_ASSERT_EQUAL(3, fetch_result.available_entries);
    TEST_ASSERT_EQUAL(3, fetch_result.fetched_entries);

    glw_imu_data_frame_t frame = {0};
    glw_imu_fifo_read_frame(&frame);
    // Gyro and acc values should be present; the invalid entry is skipped.
    ASSERT_FRAME(frame, GLW_IMU_DATA_CONTAINS_GYRO | GLW_IMU_DATA_CONTAINS_ACC, 10, -20, 30, 100, -200, 300);

    glw_imu_fifo_read_frame(&frame);
    TEST_ASSERT_EQUAL(0, frame.flags);
}

// ---- FIFO with more data than internal buffer can hold ----------------

TEST_CASE(fifo_read_more_than_buffer) {
    int buf_entries = GLW_IMU_FIFO_BUFFER_SIZE;
    int sensor_level = buf_entries + 10;

    // Build entries: gyro(cnt) + acc(cnt) pairs cycling cnt 0..3
    uint8_t entries[buf_entries * 7];
    uint8_t *p = entries;

    // Frame 1: cnt=0 with known values
    const uint8_t frame1_gyro[7] = {FIFO_ENTRY(GY_TAG_SENSOR, 0, 10, -20, 30)};
    const uint8_t frame1_acc[7] = {FIFO_ENTRY(XL_TAG_SENSOR, 0, 100, -200, 300)};
    memcpy(p, frame1_gyro, 7);
    p += 7;
    memcpy(p, frame1_acc, 7);
    p += 7;

    // Frame 2: cnt=1 with known values
    const uint8_t frame2_gyro[7] = {FIFO_ENTRY(GY_TAG_SENSOR, 1, 40, -50, 60)};
    const uint8_t frame2_acc[7] = {FIFO_ENTRY(XL_TAG_SENSOR, 1, 400, -500, 600)};
    memcpy(p, frame2_gyro, 7);
    p += 7;
    memcpy(p, frame2_acc, 7);
    p += 7;

    // Fill the rest with dummy pairs cycling cnt 0..3
    int remaining = buf_entries - 4;
    for (int i = 0; i < remaining; i++) {
        int entry_idx = 4 + i;
        int cnt = (entry_idx / 2) % 4;
        uint8_t sensor = (entry_idx % 2 == 0) ? GY_TAG_SENSOR : XL_TAG_SENSOR;
        p[0] = TAG_BYTE(sensor, cnt);
        memset(&p[1], 0, 6);
        p += 7;
    }

    // Setup the fake sensor FIFO
    fake_fifo_level = sensor_level;
    fake_fifo_ptr = fake_fifo;
    fake_fifo_end = fake_fifo + sizeof(entries);
    memcpy(fake_fifo, entries, sizeof(entries));

    // Fetch
    glw_imu_fifo_fetch_result_t fetch_result;
    TEST_ASSERT_OK(glw_imu_fifo_fetch(&fetch_result));
    TEST_ASSERT_EQUAL(sensor_level, fetch_result.available_entries);
    TEST_ASSERT_EQUAL(buf_entries, fetch_result.fetched_entries);
    TEST_ASSERT_EQUAL(GLW_IMU_FIFO_BUFFER_SIZE, fetch_result.buffer_size);

    glw_imu_data_frame_t frame;

    // Verify first frame
    glw_imu_fifo_read_frame(&frame);
    ASSERT_FRAME(frame, GLW_IMU_DATA_CONTAINS_GYRO | GLW_IMU_DATA_CONTAINS_ACC, 10, -20, 30, 100, -200, 300);

    // Verify second frame
    glw_imu_fifo_read_frame(&frame);
    ASSERT_FRAME(frame, GLW_IMU_DATA_CONTAINS_GYRO | GLW_IMU_DATA_CONTAINS_ACC, 40, -50, 60, 400, -500, 600);

    // Read remaining frames and verify they are consumed
    int frames_read = 2;
    do {
        glw_imu_fifo_read_frame(&frame);
        if (frame.flags == 0)
            break;
        frames_read++;
    } while (1);

    TEST_ASSERT_EQUAL((buf_entries + 1) / 2, frames_read);
}

// ========================================================================

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_fifo_read_empty);
    RUN_TEST(test_fifo_read_single_pair);
    RUN_TEST(test_fifo_read_two_frames);
    RUN_TEST(test_fifo_read_gyro_only);
    RUN_TEST(test_fifo_read_acc_only);
    RUN_TEST(test_fifo_read_invalid_tag);
    RUN_TEST(test_fifo_read_more_than_buffer);

    return UNITY_END();
}
