#ifndef PLDM_ALL_TESTS_H_
#define PLDM_ALL_TESTS_H_


#include "testing.h"
#include "platform_all_tests.h"
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

    add_all_pldm_fwup_fd_tests(suite);

    add_all_pldm_fwup_ua_tests(suite);

}


#endif /* PLDM_ALL_TESTS_H_ */