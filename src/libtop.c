#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

#include "libtop.h"

const uint8_t OptionRanges[5] = {
    TOP_WATCH_REM,
    TOP_QUIT_ERROR,
    TOP_NOTIFY_MOVE,
    TOP_REPLY_INVALID_DATA,
    TOP_STATUS_ERR_READ_INOTIFY
};

const int ActionSizes[5] = {
    TOP_DLEN_WATCH,
    TOP_DLEN_QUIT,
    TOP_DLEN_NOTIFY,
    TOP_DLEN_REPLY,
    TOP_DLEN_STATUS
};

int LastActionIndex = sizeof(OptionRanges) / sizeof(OptionRanges[0]) - 1;

void print_packet(struct top_msg *msg) {
    printf("--PACKET--\n");
    printf("cmd: %x\ntype: %x\nsize: %x\n", msg->action, msg->option, msg->size);

    for (int i = 0; i < msg->data_len; i++) {
        printf("data %d: %s\n", i, msg->data[i]);
    }
}

void serialize_result_factory(struct serialize_result *result) {

    result->size = -1;
    result->reply = TOP_REPLY_UNSET;

}

void top_msg_factory(struct top_msg *msg) {
    msg->action = TOP_ACTION_UNSET;
    msg->option = TOP_UNSET_UNSET;
    msg->size = 0;

    msg->data = NULL;
    msg->data_len = 0;
}

void top_msg_reset(struct top_msg *msg) {
    if (msg->data == NULL) {
        top_msg_factory(msg);
        return;
    }

    for (int i = 0; i < msg->data_len; i++) {
        if (msg->data[i] != NULL) {
            free(msg->data[i]);
        }
    }

    free(msg->data);
    top_msg_factory(msg);
}

void serialize(uint8_t buffer[TOP_PACKET_MAX_SIZE], struct top_msg *msg, struct serialize_result *result) {

    int validDataLength = -1;
    int dataSize = 0;
    int runningDataSize = 0;

    serialize_result_factory(result);

    if (msg->action <= LastActionIndex) {
        buffer[0] = msg->action;
        validDataLength = ActionSizes[msg->action];
    } else {
        result->reply = TOP_REPLY_BAD_ACTION;
        return;
    }

    if (validDataLength != msg->data_len) {
        result->reply = TOP_REPLY_BAD_SIZE;
        return;
    }

    if (msg->option > OptionRanges[msg->action]) {
        result->reply = TOP_REPLY_BAD_OPTION;
        return;
    }

    buffer[1] = msg->option;

    if (validDataLength != 0 && msg->data == NULL) {
        result->reply = TOP_REPLY_INVALID_DATA;
        return;
    }

    for (int i = 0; i < msg->data_len; i++) {
        if (msg->data[i] == NULL) {
            result->reply = TOP_REPLY_INVALID_DATA;
            return;
        }

        for (int j = 0; j < TOP_PACKET_MAX_SIZE - TOP_PACKET_MIN_SIZE - dataSize; j++) {
            runningDataSize += 1;
            buffer[TOP_PACKET_MIN_SIZE + dataSize + j] = msg->data[i][j];

            if (msg->data[i][j] == '\0') {
                break;
            }
        }

        dataSize += runningDataSize;
        runningDataSize = 0;
    }

    buffer[2] = dataSize;
    result->size = TOP_PACKET_MIN_SIZE + dataSize;
    result->reply = TOP_REPLY_VALID;
    return;

}

void deserialize(uint8_t buffer[TOP_PACKET_MAX_SIZE], struct top_msg *msg, struct serialize_result *result) {
    int validDataLength = -1;
    int dataSizeIndex = 0;
    int dataSize = 0;
    int *dataSizes = NULL;
    int dataOffset = TOP_PACKET_MIN_SIZE;

    serialize_result_factory(result);
    top_msg_reset(msg);

    if (buffer[0] <= LastActionIndex) {
        msg->action = buffer[0];
        validDataLength = ActionSizes[msg->action];
    }
    else {
        result->reply = TOP_REPLY_BAD_ACTION;
        return;
    }

    if (buffer[1] > OptionRanges[msg->action]) {
        result->reply = TOP_REPLY_BAD_OPTION;
        return;
    }

    msg->option = buffer[1];

    if (validDataLength == 0) {
        if (buffer[2] != 0) {
            result->reply = TOP_REPLY_BAD_SIZE;
        }
        else {
            result->reply = TOP_REPLY_VALID;
        }

        return;
    }

    msg->size = buffer[2];
    dataSizes = (int*) malloc(sizeof(int) * validDataLength);

    for (int i = TOP_PACKET_MIN_SIZE; i < TOP_PACKET_MAX_SIZE; i++) {
        
        if (i > msg->size + TOP_PACKET_MIN_SIZE) {
            free(dataSizes);
            result->reply = TOP_REPLY_BAD_SIZE;
            return;
        }

        if (dataSizeIndex == validDataLength) {
            if (dataSize == 1) {
                free(dataSizes);
                result->reply = TOP_REPLY_BAD_SIZE;
            }
            break;
        }

        if (buffer[i] == '\0') {
            dataSizes[dataSizeIndex] = ++dataSize;
            dataSizeIndex++;
            dataSize = 0;
            continue;
        }

        if (buffer[i] < '!' || buffer[i] > '~') {
            free(dataSizes);
            result->reply = TOP_REPLY_INVALID_DATA;
            return;
        }

        dataSize++;
    }

    msg->data = (char**) malloc(sizeof(char*) * validDataLength);
    msg->data_len = validDataLength;

    for (int i = 0; i < validDataLength; i++) {
        msg->data[i] = (char*) malloc(sizeof(char) * dataSizes[i]);

        for (int j = 0; j < dataSizes[i]; j++) {
            msg->data[i][j] = buffer[j + dataOffset];
        }

        dataOffset += dataSizes[i];
    }

    result->reply = TOP_REPLY_VALID;
    result->size = TOP_PACKET_MIN_SIZE + msg->size;
    free(dataSizes);

    return;
}