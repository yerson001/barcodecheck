idf_component_register(SRCS "gm75_scanner.c"
                       INCLUDE_DIRS "."
                       REQUIRES driver freertos
                       PRIV_REQUIRES cipher
                       PRIV_REQUIRES queue logger
                       )
message(STATUS "GM75 component loaded")
