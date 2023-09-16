#include <stdio.h>
#include "stream.h"

int main() {
	stream input = stream_open_file("data/sample.nl");
	char c; 
	
	while (stream_get_char(input, &c)) {
		printf("%c", c);
	}
	
	stream_close(input);
	
	return 0;
}