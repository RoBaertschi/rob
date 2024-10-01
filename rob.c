#define ROB_IMPL
#include "rob.h"

int main(void) {
    rob_rebuild_self();
    rob_command_builder builder = rob_create_command_builder("gcc");
    rob_command_append(&builder, "-std=c11", "-Wall", "-Wextra", "test.c", "-o", "test");

    rob_command_builder echo = rob_create_command_builder("sh");
    rob_command_append(&echo, "-c", "echo test > test.txt");

    rob_target echot = rob_target_create(echo, "test.txt");


    rob_target t = rob_target_create(builder, "test");
    rob_target_inputs_append(&t, "test.c");
    rob_target_inputs_append_target(&t, &echot);

    rob_sync_target(&t);
}
