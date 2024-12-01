#define CAN_INVALID_ID (uint8_t)0

#define CAN_E_SUCCESS (uint32_t)0

#define CAN_E_NOTUPDATED (uint32_t)1
#define CAN_E_I_EOF (uint32_t)2
#define CAN_E_I_NO_SUCH_MSG (uint32_t)3
#define CAN_E_THREAD_ALREADY_RUNNING (uint32_t)4

#define CAN_E_WARNING_MAX = (uint32_t)1023

/// Returned from CanDatabase::parse
#define CAN_E_FILE_OPEN_ERRNO (uint32_t)1024
#define CAN_E_FILE_FS_MALFORM (uint32_t)1025
#define CAN_E_FILE_READ_ERRNO (uint32_t)1026
#define CAN_E_FILE_NO_DBC_VERSION (uint32_t)1027
#define CAN_E_FILE_DBC_VERSION_ERRNO (uint32_t)1028
#define CAN_E_FILE_UNEXPECTED_EOF (uint32_t)1029
#define CAN_E_FILE_NUM_READ_ERRNO (uint32_t)1030
#define CAN_E_SIGNAL_POS_OB (uint32_t)1031

/// Returned from CanDatabase::decode
#define CAN_E_WRONG_DBC_VERSION (uint32_t)1032
#define CAN_E_UNKNOWN_MSG_ID (uint32_t)1033
#define CAN_E_PARTIAL_MSG (uint32_t)1034



#define CAN_E_WRAPPER_NOT_INITIALIZED (uint32_t)1040

#define CAN_E_THREAD_LAUNCH_FAIL (uint32_t)2000

#define CAN_E_FW_IF_ERR(func) {res = func; if(res != CAN_E_SUCCESS) return res;}