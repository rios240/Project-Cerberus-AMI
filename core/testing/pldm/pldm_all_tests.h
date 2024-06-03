#ifndef PLDM_FWUP_ALL_TESTS_H_
#define PLDM_FWUP_ALL_TESTS_H_


#include "testing.h"
#include "platform_all_tests.h"
#include "common/unused.h"
#include "fwup_fd/pldm_fwup_fd_all_tests.h"
#include "fwup_ua/pldm_fwup_ua_all_tests.h"

/**
 * Add all tests for components in the 'pldm' directory.
 *
 * Be sure to keep the test suites in alphabetical order for easier management.
 *
 * @param suite Suite to add the tests to.
 */
static void add_all_pldm_tests (CuSuite *suite)
{
    /* This is unused when no tests will be executed. */
    UNUSED (suite);

#if (defined TESTING_RUN_PLDM_FWUP_FD_SUITE || \
        (!defined TESTING_SKIP_PLDM_FWUP_FD_SUITE))
    add_all_pldm_fwup_fd_tests(suite);
#endif

#if (defined TESTING_RUN_PLDM_FWUP_UA_SUITE || \
        (!defined TESTING_SKIP_PLDM_FWUP_UA_SUITE))
    add_all_pldm_fwup_ua_tests(suite);
#endif

}





#endif /* PLDM_FWUP_ALL_TESTS_H_ */