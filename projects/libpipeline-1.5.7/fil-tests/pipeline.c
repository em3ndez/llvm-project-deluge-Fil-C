#include <pipeline.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    pipeline *p;
    int status;

    // Create a new pipeline
    p = pipeline_new();

    // Add commands to the pipeline
    pipeline_command_args(p, "ls", "-1", NULL);
    pipeline_command_args(p, "grep", "pipeline\\.c$", NULL);
    pipeline_command_args(p, "wc", "-l", NULL);

    // Run the pipeline
    status = pipeline_run(p);

    // Check the status
    if (status == 0) {
        printf("Pipeline executed successfully.\n");
    } else {
        printf("Pipeline failed with status: %d\n", status);
        return 1;
    }

    return 0;
}
