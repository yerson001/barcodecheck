idf_component_register(SRCS "gm75_scanner.c"
                       INCLUDE_DIRS "."
                       REQUIRES driver freertos
                       PRIV_REQUIRES cipher
                       )
message(STATUS "GM75 component loaded")
