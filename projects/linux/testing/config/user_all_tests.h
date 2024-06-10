#ifndef USER_ALL_TESTS_H_
#define USER_ALL_TESTS_H_

/** Testing configuration for AMI PLDM Firmware Update project. 
 * 
 * Skips all but the PLDM Firmware Update tests.
 * 
*/

//#define TESTING_SKIP_ALL_TESTS


#define TESTING_RUN_PLDM_FWUP_PROTOCOL_UA_COMMANDS_SUITE
#define TESTING_SKIP_PLDM_FWUP_PROTOCOL_FD_COMMANDS_SUITE

#define TESTING_RUN_PLDM_FWUP_HANDLER_UA_SUITE
#define TESTING_SKIP_PLDM_FWUP_HANDLER_FD_SUITE


#endif /* USER_ALL_TESTS_H_ */