// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "session_manager_mock.h"


static int session_manager_mock_add_session (struct session_manager *session, uint8_t eid, 
	const uint8_t *device_nonce, const uint8_t *cerberus_nonce)
{
	struct session_manager_mock *mock = (struct session_manager_mock*) session;

	if (mock == NULL) {
		return MOCK_INVALID_ARGUMENT;
	}

	MOCK_RETURN (&mock->mock, session_manager_mock_add_session, session, MOCK_ARG_CALL (eid),
		MOCK_ARG_CALL (device_nonce), MOCK_ARG_CALL (cerberus_nonce));
}

static int session_manager_mock_establish_session (struct session_manager *session, uint8_t eid, 
	const uint8_t *pub_key, uint32_t pub_key_len, bool pair_key)
{
	struct session_manager_mock *mock = (struct session_manager_mock*) session;

	if (mock == NULL) {
		return MOCK_INVALID_ARGUMENT;
	}

	MOCK_RETURN (&mock->mock, session_manager_mock_establish_session, session, MOCK_ARG_CALL (eid),
		MOCK_ARG_CALL (pub_key), MOCK_ARG_CALL (pub_key_len), MOCK_ARG_CALL (pair_key));
}

static int session_manager_mock_decrypt_message (struct session_manager *session, uint8_t eid, 
	uint8_t *msg, size_t msg_len, size_t buffer_len)
{
	struct session_manager_mock *mock = (struct session_manager_mock*) session;

	if (mock == NULL) {
		return MOCK_INVALID_ARGUMENT;
	}

	MOCK_RETURN (&mock->mock, session_manager_mock_decrypt_message, session, MOCK_ARG_CALL (eid), 
		MOCK_ARG_CALL (msg), MOCK_ARG_CALL (msg_len), MOCK_ARG_CALL (buffer_len));
}

static int session_manager_mock_encrypt_message (struct session_manager *session, uint8_t eid, 
	uint8_t *msg, size_t msg_len, size_t buffer_len)
{
	struct session_manager_mock *mock = (struct session_manager_mock*) session;

	if (mock == NULL) {
		return MOCK_INVALID_ARGUMENT;
	}

	MOCK_RETURN (&mock->mock, session_manager_mock_encrypt_message, session, 
		MOCK_ARG_CALL (eid), MOCK_ARG_CALL (msg), MOCK_ARG_CALL (msg_len), 
		MOCK_ARG_CALL (buffer_len));
}

static int session_manager_mock_is_session_established (struct session_manager *session, 
	uint8_t eid)
{
	struct session_manager_mock *mock = (struct session_manager_mock*) session;

	if (mock == NULL) {
		return MOCK_INVALID_ARGUMENT;
	}

	MOCK_RETURN (&mock->mock, session_manager_mock_is_session_established, session, 
		MOCK_ARG_CALL (eid));
}

static int session_manager_mock_func_arg_count (void *func)
{
	if (func == session_manager_mock_is_session_established) {
		return 1;
	}
	else if (func == session_manager_mock_add_session) {
		return 3;
	}
	else if ((func == session_manager_mock_establish_session) || 
			(func == session_manager_mock_decrypt_message) || 
			(func == session_manager_mock_encrypt_message)) {
		return 4;
	}
	else {
		return 0;
	}
}

static const char* session_manager_mock_func_name_map (void *func)
{
	if (func == session_manager_mock_add_session) {
		return "add_session";
	}
	else if (func == session_manager_mock_decrypt_message) {
		return "decrypt_message";
	}
	else if (func == session_manager_mock_encrypt_message) {
		return "encrypt_message";
	}
	else if (func == session_manager_mock_is_session_established) {
		return "is_session_established";
	}
	else if (func == session_manager_mock_establish_session) {
		return "establish_session";
	}
	else {
		return "unknown";
	}
}

static const char* session_manager_mock_arg_name_map (void *func, int arg)
{
	if (func == session_manager_mock_add_session) {
		switch (arg) {
			case 0:
				return "eid";

			case 1:
				return "device_nonce";

			case 2:
				return "cerberus_nonce";
		}
	}
	if (func == session_manager_mock_establish_session) {
		switch (arg) {
			case 0:
				return "eid";

			case 1:
				return "pub_key";

			case 2:
				return "pub_key_len";

			case 3:
				return "pair_key";
		}
	}
	else if ((func == session_manager_mock_decrypt_message) ||
			(func == session_manager_mock_encrypt_message))  {
		switch (arg) {
			case 0:
				return "eid";

			case 1:
				return "msg";

			case 2:
				return "msg_len";

			case 3:
				return "buffer_len";
		}
	}
	else if (func == session_manager_mock_is_session_established) {
		switch (arg) {
			case 0:
				return "eid";
		}
	}

	return "unknown";
}

/**
 * Initialize a mock instance for a session_manager.
 *
 * @param mock The mock to initialize.
 *
 * @return 0 if the mock was initialized successfully or an error code.
 */
int session_manager_mock_init (struct session_manager_mock *mock)
{
	int status;

	if (mock == NULL) {
		return MOCK_INVALID_ARGUMENT;
	}

	memset (mock, 0, sizeof (struct session_manager_mock));

	status = mock_init (&mock->mock);
	if (status != 0) {
		return status;
	}

	mock_set_name (&mock->mock, "session_manager");

	mock->base.add_session = session_manager_mock_add_session;
	mock->base.establish_session = session_manager_mock_establish_session;
	mock->base.decrypt_message = session_manager_mock_decrypt_message;
	mock->base.encrypt_message = session_manager_mock_encrypt_message;
	mock->base.is_session_established = session_manager_mock_is_session_established;

	mock->mock.func_arg_count = session_manager_mock_func_arg_count;
	mock->mock.func_name_map = session_manager_mock_func_name_map;
	mock->mock.arg_name_map = session_manager_mock_arg_name_map;

	return 0;
}

/**
 * Free the resources used by a session_manager mock instance.
 *
 * @param mock The mock to release.
 */
void session_manager_mock_release (struct session_manager_mock *mock)
{
	if (mock != NULL) {
		mock_release (&mock->mock);
	}
}

/**
 * Validate that the session_manager mock instance was called as expected and release it.
 *
 * @param mock The mock instance to validate.
 *
 * @return 0 if the mock was called as expected or 1 if not.
 */
int session_manager_mock_validate_and_release (struct session_manager_mock *mock)
{
	int status = 1;

	if (mock != NULL) {
		status = mock_validate (&mock->mock);
		session_manager_mock_release (mock);
	}

	return status;
}
