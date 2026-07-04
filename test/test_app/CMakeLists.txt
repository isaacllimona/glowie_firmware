add_library(test_app_main STATIC main.c)

link_idf_components(test_app_main PRIVATE unity)
