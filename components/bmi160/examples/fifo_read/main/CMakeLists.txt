set(TARGET ${COMPONENT_TARGET})

add_library(${TARGET} STATIC
    main.c
)

target_link_libraries(${TARGET} PRIVATE bmi160)

link_idf_components(${TARGET} PRIVATE freertos log)
