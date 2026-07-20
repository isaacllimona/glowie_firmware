#include "native_test_common.h"

#include "config.h"
#include "driver_mock.h"
#include "glw_led.h"
#include "unity.h"
#include "unity_internals.h"

#define NUM_PIXELS CONFIG_GLW_LED_NUM_PIXELS

void glw_led_change_config(unsigned int new_num_pixels, const int *new_pixel_mapping);
void glw_led_change_config_default(void);

// ======= custom asserts and utils ========

typedef struct {
    unsigned int r, g, b;
    int idx;
} pixel_set_expect_t;

void assert_pixels_set(const pixel_set_expect_t *pixels, unsigned int npixels) {
    // Get the maximum pixel index
    int maxidx = pixels[0].idx;
    for (int i = 0; i < npixels; i++)
        maxidx = pixels[i].idx > maxidx ? pixels[i].idx : maxidx;

    // Transpose the given list of pixels into a map indexed by the pixel index
    struct pixel_map_element_t {bool should_be_set; unsigned int r, g, b;} pixels_map[maxidx + 1];
    memset(pixels_map, 0, sizeof(pixels_map));
    for (int i = 0; i < npixels; i++)
        pixels_map[pixels[i].idx] = (struct pixel_map_element_t){.should_be_set = true, pixels[i].r, pixels[i].g, pixels[i].b};

    // Create an is_set map
    bool is_set[maxidx + 1];
    for (int i = 0; i <= maxidx; i++)
        is_set[i] = false;

    // Assert pixels have been set to the right color
    TEST_ASSERT_EQUAL(npixels, driver_set_pixel_color_fake.call_count);
    for (int i = 0; i < npixels; i++) {
        int idx = driver_set_pixel_color_fake.arg3_history[i];
        TEST_ASSERT_FALSE_MESSAGE(is_set[idx], "A pixel is set multiple times");
        TEST_ASSERT_TRUE_MESSAGE(pixels_map[idx].should_be_set, "A pixel has been set which shouldn't be set");

        TEST_ASSERT_EQUAL(pixels_map[idx].r, driver_set_pixel_color_fake.arg0_history[i]);
        TEST_ASSERT_EQUAL(pixels_map[idx].g, driver_set_pixel_color_fake.arg1_history[i]);
        TEST_ASSERT_EQUAL(pixels_map[idx].b, driver_set_pixel_color_fake.arg2_history[i]);

        is_set[idx] = true;
    }

    // Assert all pixels have been set
    for (int i = 0; i <= maxidx; i++) {
        if (pixels_map[i].should_be_set) {
            TEST_ASSERT_TRUE(is_set[i]);
        }
    }
}

#define ASSERT_PIXELS_SET(pixels) assert_pixels_set(pixels, sizeof(pixels) / sizeof(pixels[0]))

void assert_pixels_set_all(unsigned int r, unsigned int g, unsigned int b, const int *idxs,
                           unsigned int npixels) {
    pixel_set_expect_t expect_pixels[npixels];
    for(int i = 0; i < npixels; i++)
        expect_pixels[i] = (pixel_set_expect_t){r, g, b, .idx = idxs[i]};

    assert_pixels_set(expect_pixels, npixels);
}

#define ASSERT_PIXELS_SET_ALL(r, g, b, set_pixels)                                                           \
    assert_pixels_set_all(r, g, b, set_pixels, sizeof(set_pixels) / sizeof(set_pixels[0]))

const int set_pixels_default[] = CONFIG_GLW_LED_PIXEL_MAPPING;

// ===================================

void setUp(void) {
    glw_led_change_config_default();

    glw_led_set_enabled(true);
    glw_led_set_brightness(255);

    reset_all_fakes();
    FFF_RESET_HISTORY();
}

void tearDown(void) {}

// ===== init =====

TEST_CASE(led_init) {
    glw_led_init();
    ASSERT_CALLED(driver_init);
}

// ===== set_enabled =====

TEST_CASE(led_set_enabled_false_turns_off_all_pixels) {
    // First set a color so LEDs are non-zero
    glw_led_set_color(255, 128, 64);

    RESET_FAKE(driver_set_pixel_color);
    RESET_FAKE(driver_refresh);

    glw_led_set_enabled(false);

    // All pixels should be set to (0, 0, 0)
    ASSERT_PIXELS_SET_ALL(0, 0, 0, set_pixels_default);
    ASSERT_CALLED(driver_refresh);
}

TEST_CASE(led_set_enabled_true_after_disable) {
    // Set a color, disable, then re-enable
    glw_led_set_color(100, 200, 50);
    glw_led_set_enabled(false);

    RESET_FAKE(driver_set_pixel_color);
    RESET_FAKE(driver_refresh);

    glw_led_set_enabled(true);

    // Should re-apply the stored color with current brightness (255)
    ASSERT_PIXELS_SET_ALL(100, 200, 50, set_pixels_default);
    ASSERT_CALLED(driver_refresh);
}

// ===== set_brightness =====

TEST_CASE(led_set_brightness_max) {
    glw_led_set_color(255, 128, 64);

    RESET_FAKE(driver_set_pixel_color);
    RESET_FAKE(driver_refresh);

    glw_led_set_brightness(255);

    ASSERT_PIXELS_SET_ALL(255, 128, 64, set_pixels_default);
    ASSERT_CALLED(driver_refresh);
}

TEST_CASE(led_set_brightness_half) {
    glw_led_set_color(255, 128, 64);

    RESET_FAKE(driver_set_pixel_color);
    RESET_FAKE(driver_refresh);

    glw_led_set_brightness(128);

    ASSERT_PIXELS_SET_ALL(128, 64, 32, set_pixels_default);
    ASSERT_CALLED(driver_refresh);
}

TEST_CASE(led_set_brightness_zero) {
    glw_led_set_color(255, 128, 64);

    RESET_FAKE(driver_set_pixel_color);
    RESET_FAKE(driver_refresh);

    glw_led_set_brightness(0);

    ASSERT_PIXELS_SET_ALL(0, 0, 0, set_pixels_default);
    ASSERT_CALLED(driver_refresh);
}

// ===== set_color =====

TEST_CASE(led_set_color_applies_to_all_pixels) {
    glw_led_set_color(10, 20, 30);

    ASSERT_PIXELS_SET_ALL(10, 20, 30, set_pixels_default);
    ASSERT_CALLED(driver_refresh);
}

TEST_CASE(led_set_color_with_zero) {
    // Set a non-zero color first
    glw_led_set_color(255, 255, 255);
    RESET_FAKE(driver_set_pixel_color);
    RESET_FAKE(driver_refresh);

    glw_led_set_color(0, 0, 0);

    ASSERT_PIXELS_SET_ALL(0, 0, 0, set_pixels_default);
    ASSERT_CALLED(driver_refresh);
}

TEST_CASE(led_set_color_uses_brightness) {
    glw_led_set_brightness(128);

    RESET_FAKE(driver_set_pixel_color);
    RESET_FAKE(driver_refresh);

    glw_led_set_color(255, 128, 64);

    ASSERT_PIXELS_SET_ALL(128, 64, 32, set_pixels_default);
    ASSERT_CALLED(driver_refresh);
}

TEST_CASE(led_set_color_weird_mapping) {
    const int pixel_mapping[] = {2, 4, 6};
    glw_led_change_config(3, pixel_mapping);

    glw_led_set_color(10, 20, 30);

    ASSERT_PIXELS_SET_ALL(10, 20, 30, pixel_mapping);
    ASSERT_CALLED(driver_refresh);
}

// ===== set_pixel_color + update =====

TEST_CASE(led_set_pixel_color_does_not_call_driver) {
    glw_led_set_pixel_color(255, 128, 64, 0);

    // set_pixel_color should not trigger any driver calls
    ASSERT_NO_CALL(driver_set_pixel_color);
    ASSERT_NO_CALL(driver_refresh);
}

TEST_CASE(led_set_pixel_color_then_update) {
    glw_led_set_color(0, 0, 0);
    glw_led_set_pixel_color(10, 20, 30, 5);

    RESET_FAKE(driver_set_pixel_color);
    RESET_FAKE(driver_refresh);

    glw_led_update();

    // All pixels should be zero except pixel 5
    pixel_set_expect_t expect[NUM_PIXELS];
    for (int i = 0; i < NUM_PIXELS; i++) {
        int idx = set_pixels_default[i];
        if (idx == 5) expect[i] = (pixel_set_expect_t){10, 20, 30, .idx = idx};
        else expect[i] = (pixel_set_expect_t){0, 0, 0, .idx = idx};
    }

    ASSERT_PIXELS_SET(expect);
    ASSERT_CALLED(driver_refresh);
}

TEST_CASE(led_set_pixel_color_weird_mapping) {
    const int pixel_mapping[] = {6, 4, 2, 9};
    glw_led_change_config(4, pixel_mapping);

    glw_led_set_color(0, 0, 0);
    glw_led_set_pixel_color(110, 120, 130, 0);
    glw_led_set_pixel_color(10, 20, 30, 2);

    RESET_FAKE(driver_set_pixel_color);
    RESET_FAKE(driver_refresh);

    glw_led_update();

    pixel_set_expect_t expect[] = {
        {10, 20, 30, .idx = 2},
        {0, 0, 0, .idx = 4},
        {110, 120, 130, .idx = 6},
        {0, 0, 0, .idx = 9}
    };

    ASSERT_PIXELS_SET(expect);
    ASSERT_CALLED(driver_refresh);
}

// ===== main =====

int main(void) {

    UNITY_BEGIN();

    // init
    RUN_TEST(test_led_init);

    // set_enabled
    RUN_TEST(test_led_set_enabled_false_turns_off_all_pixels);
    RUN_TEST(test_led_set_enabled_true_after_disable);

    // set_brightness
    RUN_TEST(test_led_set_brightness_max);
    RUN_TEST(test_led_set_brightness_half);
    RUN_TEST(test_led_set_brightness_zero);

    // set_color
    RUN_TEST(test_led_set_color_applies_to_all_pixels);
    RUN_TEST(test_led_set_color_with_zero);
    RUN_TEST(test_led_set_color_uses_brightness);
    RUN_TEST(test_led_set_color_weird_mapping);

    // set_pixel_color
    RUN_TEST(test_led_set_pixel_color_does_not_call_driver);
    RUN_TEST(test_led_set_pixel_color_then_update);
    RUN_TEST(test_led_set_pixel_color_weird_mapping);

    return UNITY_END();
}
