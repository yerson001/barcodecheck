#include "unity.h"
#include "gm75_scanner.h"

TEST_CASE("GM75 init no debe fallar", "[gm75]") {
    gm75_scanner_init();
    TEST_ASSERT_TRUE_MESSAGE(1, "Init ejecutado");
}
