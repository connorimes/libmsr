#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "cpuid.h"
#include "msr_core.h"
#include "msr_rapl.h"
#include "msr_thermal.h"
#include "msr_counters.h"
#include "msr_clocks.h"
#include "msr_misc.h"
#include "msr_turbo.h"
#include "csr_core.h"
#include "csr_imc.h"
#include "libmsr_error.h"
#ifdef MPI
#include <mpi.h>
#endif

struct rapl_limit l1, l2;

void rapl_r_test(struct rapl_data **rd)
{
    poll_rapl_data();
    fprintf(stdout, "Sample #1:\n");
    dump_rapl_data(stdout);

    poll_rapl_data();
    fprintf(stdout, "\nSample #2:\n");
    dump_rapl_data(stdout);
}

#define PT_INC 1500

int repeated_poll_test()
{
    usleep(1000);
    poll_rapl_data();
    fprintf(stdout, "Sample #1:\n");
    dump_rapl_data(stdout);
    usleep(PT_INC);
    poll_rapl_data();
    fprintf(stdout, "\nSample #2:\n");
    dump_rapl_data(stdout);
    usleep(PT_INC);
    poll_rapl_data();
    fprintf(stdout, "\nSample #3:\n");
    dump_rapl_data(stdout);
    usleep(PT_INC);
    poll_rapl_data();
    fprintf(stdout, "\nSample #4:\n");
    dump_rapl_data(stdout);
    usleep(PT_INC);
    poll_rapl_data();
    fprintf(stdout, "\nSample #5:\n");
    dump_rapl_data(stdout);
    usleep(PT_INC);
    poll_rapl_data();
    fprintf(stdout, "\nSample #6:\n");
    dump_rapl_data(stdout);
    return 0;
}

int main(int argc, char **argv)
{
    struct rapl_data *rd = NULL;
    uint64_t *rapl_flags = NULL;
    uint64_t cores = 0;
    uint64_t threads = 0;
    uint64_t sockets = 0;
    int ri_stat = 0;

    if (!sockets)
    {
        core_config(&cores, &threads, &sockets, NULL);
    }
    if (init_msr())
    {
        libmsr_error_handler("Unable to initialize libmsr", LIBMSR_ERROR_MSR_INIT, getenv("HOSTNAME"), __FILE__, __LINE__);
        return -1;
    }
    fprintf(stdout, "\n===== MSR Init Done =====\n");

    ri_stat = rapl_init(&rd, &rapl_flags);
    if (ri_stat < 0)
    {
        libmsr_error_handler("Unable to initialize rapl", LIBMSR_ERROR_RAPL_INIT, getenv("HOSTNAME"), __FILE__, __LINE__);
        return -1;
    }
    fprintf(stdout, "\n===== RAPL Init Done =====\n");

    fprintf(stdout, "\n===== POWER INFO =====\n");
    dump_rapl_power_info(stdout);

    fprintf(stdout, "\n===== Poll RAPL Data 2X =====\n");
    rapl_r_test(&rd);

    fprintf(stdout, "\n===== Repeated RAPL Polling Test =====\n");
    repeated_poll_test();

    finalize_msr();
    fprintf(stdout, "===== MSR Finalized =====\n");

    fprintf(stdout, "\n===== Test Finished Successfully =====\n");

    return 0;
}
