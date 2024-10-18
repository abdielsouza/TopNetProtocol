#ifndef D4D383B1_C1E0_4F5B_AC42_75BC7334EE2F
#define D4D383B1_C1E0_4F5B_AC42_75BC7334EE2F

#pragma once

#include <stdint.h>

typedef uint8_t top_action;
typedef uint8_t top_option;

#define TOP_ACTION_WATCH                    0x00
#define TOP_ACTION_QUIT                     0x01
#define TOP_ACTION_NOTIFY                   0x02
#define TOP_ACTION_REPLY                    0x03
#define TOP_ACTION_STATUS                   0x04
#define TOP_ACTION_UNSET                    0xFF

#define TOP_WATCH_ADD                       0x00
#define TOP_WATCH_REM                       0x01

#define TOP_QUIT_USER                       0x00
#define TOP_QUIT_ERROR                      0x01

#define TOP_NOTIFY_CREATE                   0x00
#define TOP_NOTIFY_DELETE                   0x01
#define TOP_NOTIFY_ACCESS                   0x02
#define TOP_NOTIFY_CLOSE                    0x03
#define TOP_NOTIFY_MODIFY                   0x04
#define TOP_NOTIFY_MOVE                     0x05

#define TOP_REPLY_VALID                     0x00
#define TOP_REPLY_BAD_SIZE                  0x01
#define TOP_REPLY_BAD_ACTION                0x02
#define TOP_REPLY_BAD_OPTION                0x03
#define TOP_REPLY_BAD_PATH                  0x04
#define TOP_REPLY_INVALID_DATA              0x05
#define TOP_REPLY_UNSET                     0xFF

#define TOP_STATUS_SUCCESS                  0x00
#define TOP_STATUS_ERR_INIT_INOTIFY         0x01
#define TOP_STATUS_ERR_ADD_WATCH            0x02
#define TOP_STATUS_ERR_READ_INOTIFY         0x03

#define TOP_UNSET_UNSET                     0x00

#define TOP_PACKET_MIN_SIZE                 3
#define TOP_PACKET_MAX_SIZE                 255

#define TOP_DLEN_WATCH                      1
#define TOP_DLEN_QUIT                       0
#define TOP_DLEN_NOTIFY                     2
#define TOP_DLEN_REPLY                      0
#define TOP_DLEN_STATUS                     0
#define TOP_DLEN_UNSET                      0

struct top_msg {
    top_action action;
    top_option option;
    uint8_t size;

    char **data;
    int data_len;
};

struct serialize_result {
    int size;
    top_option reply;
};

void print_packet(struct top_msg *msg);
void serialize_result_factory(struct serialize_result *result);
void top_msg_factory(struct top_msg *msg);
void top_msg_reset(struct top_msg *msg);
void serialize(uint8_t buffer[TOP_PACKET_MAX_SIZE], struct top_msg *msg, struct serialize_result *result);
void deserialize(uint8_t buffer[TOP_PACKET_MAX_SIZE], struct top_msg *msg, struct serialize_result *result);

#endif /* D4D383B1_C1E0_4F5B_AC42_75BC7334EE2F */
