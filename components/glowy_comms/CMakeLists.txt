idf_component_register(SRCS "src/glowy_comms.c" "src/gap/gap.c" "src/gatt/gatt.c"
                       INCLUDE_DIRS "include"
                       PRIV_INCLUDE_DIRS "src"
                       PRIV_REQUIRES bt glowy_common)
