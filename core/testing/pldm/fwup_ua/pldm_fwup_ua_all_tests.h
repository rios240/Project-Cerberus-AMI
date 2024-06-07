#ifndef PLDM_FWUP_UA_ALL_TESTS_H_
#define PLDM_FWUP_UA_ALL_TESTS_H_

#include "testing.h"
#include "platform_all_tests.h"
#include "common/unused.h"

/**
 * Add all tests for pldm fwup when operating as ua.
 *
 * Be sure to keep the test suites in alphabetical order for easier management.
 *
 * @param suite Suite to add the tests to.
 */
static void add_all_pldm_fwup_ua_tests (CuSuite *suite)
{
    /* This is unused when no tests will be executed. */
    UNUSED (suite);
    

#if (defined TESTING_RUN_PLDM_FWUP_PROTOCOL_UA_COMMANDS_SUITE || \
	    defined TESTING_RUN_ALL_TESTS || defined TESTING_RUN_ALL_CORE_TESTS || \
		(!defined TESTING_SKIP_ALL_TESTS && !defined TESTING_SKIP_ALL_CORE_TESTS)) && \
	!defined TESTING_SKIP_PLDM_FWUP_PROTOCOL_UA_COMMANDS_SUITE
	TESTING_RUN_SUITE (pldm_fwup_protocol_ua_commands);
#endif

#if (defined TESTING_RUN_PLDM_FWUP_HANDLER_UA_SUITE || \
	    defined TESTING_RUN_ALL_TESTS || defined TESTING_RUN_ALL_CORE_TESTS || \
		(!defined TESTING_SKIP_ALL_TESTS && !defined TESTING_SKIP_ALL_CORE_TESTS)) && \
	!defined TESTING_SKIP_PLDM_FWUP_HANDLER_UA_SUITE
	TESTING_RUN_SUITE (pldm_fwup_handler_ua);
#endif

}

#endif /* PLDM_FWUP_UA_ALL_TESTS_H_ */