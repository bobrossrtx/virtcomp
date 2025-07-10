#include "test_framework.hpp"

int main() {
    TestContext ctx;

    // Test SHL instruction
    ctx.load_program({
        0x01, 0x00, 0x08,  // LOAD_IMM R0, 8
        0x18, 0x00, 0x02,  // SHL R0, 2 (immediate) (8 << 2 = 32)
        0xFF               // HALT
    });

    ctx.execute_program();

    std::cout << "R0 value: " << ctx.get_register(0) << std::endl;
    std::cout << "Expected: 32" << std::endl;

    return 0;
}
