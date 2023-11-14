#ifndef PLDM_FIRMWARE_UPDATE_FSM_H_
#define PLDM_FIRMWARE_UPDATE_FSM_H_



#include <stdint.h>
#include "pldm_firmware_update_interface.h"

#define MAX_ROW 10

typedef int (*F_FSM_TRIGGER_T)(struct pldm_firmware_update_interface *);
typedef int (*F_FSM_RESPONSE_T)(struct pldm_firmware_update_interface *);
typedef bool B_FSM_UPDATE_MODE_T;


typedef enum {
    IDLE,
    LEARN_COMPONENTS,
    READY_XFER,
    DOWNLOAD,
    VERIFY,
    APPLY,
    ACTIVATE,
} E_FSM_STATE_T;

typedef struct {
    E_FSM_STATE_T currentState;
    F_FSM_TRIGGER_T trigger;
    F_FSM_RESPONSE_T response;
    E_FSM_STATE_T nextState;
} S_FSM_ROW_T;

typedef struct {
    E_FSM_STATE_T currentState;
    B_FSM_UPDATE_MODE_T updateMode;
    S_FSM_ROW_T sstRow[MAX_ROW];
} S_FSM_STT_T;


#endif /* PLDM_FIRMWARE_UPDATE_FSM_H_ */