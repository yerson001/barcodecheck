
set(EXTRA_COMPONENT_DIR $ENV{IDF_PATH}/examples/common_components/protocol_examples_common)
idf_component_register(SRCS "connectivity.c"
                      INCLUDE_DIRS "."
                      PRIV_REQUIRES esp_wifi nvs_flash)






