#include "stream.h"

stream stream_open_file(const char *filename) {
	stream input;
	
	input.file = fopen(filename, "r");
	
	return input;
}

int stream_is_open(stream s) {
	return s.file != NULL;
}

int stream_close(stream s) {
	if (stream_is_open(s) && s.file) {
		fclose(s.file);
		s.file = NULL;
		return 1;
	} else {
		return 0;
	}
}

int stream_get_char(stream s, char *c) {
	if (!c) { return 0; }
	if (!stream_is_open(s)) { return 0; }
	
	*c = fgetc(s.file);
	
	return *c != EOF;
}
