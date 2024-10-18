#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "libtop.h"

int main() {
	uint8_t buff[TOP_PACKET_MAX_SIZE] = {
		TOP_ACTION_NOTIFY,
		TOP_WATCH_REM,
		0x0C,
		'h',
		'e',
		'l',
		'l',
		'o',
		'\0',
		'w',
		'o',
		'r',
		'l',
		'd',
		'\0',
	};

	char *lol = "hello buddy";
	char *jej = "do not reply it to me!";

	char *testData[2] = {
		lol,
		jej
	};

	uint8_t cereal[1024] = {0};

	struct serialize_result* result = (struct serialize_result*)malloc(sizeof(struct serialize_result));
	serialize_result_factory(result);

	struct top_msg *message = (struct top_msg*)malloc(sizeof(struct top_msg));
	top_msg_factory(message);

	top_option replyType = TOP_REPLY_UNSET;

	deserialize(buff, message, result);

	if (result->reply != TOP_REPLY_VALID) {
		fprintf(stderr, "GOT %d\n", result->reply);
		//return 1;
	}

	print_packet(message);
	serialize(cereal, message, result);

	for (int i = 0; i < 3 + cereal[3]; i++) {
		printf("%x ", cereal[i]);
	}
	printf("\n");

	message->data = testData;
	message->data_len = 2;

	serialize(cereal, message, result);
	if (result->reply != TOP_REPLY_VALID) {
		fprintf(stderr, "GOT %d\n", result->reply);
		return 1;
	}

	for (int i = 0; i < 3 + cereal[3]; i++) {
		printf("%x ", cereal[i]);
	}
	printf("\n");

	return 0;
}