#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "sti.h"

enum LexState {
	LST_INVALID,
	
	#include "./parser_example_enums.h"
	
	LST_MAXVALUE
};

char* state_names[] = {
	[LST_INVALID] = "LST_INVALID",
	#include "./parser_example_enum_names.h"
	[LST_MAXVALUE] = "LST_MAXVALUE",
};


// this is for the incremental lexing of each token, not the whole stream
struct lexer_state {
	enum LexState state;
	char* buffer;
	int blen;
	int balloc;
	
	int linenum;
	int charnum;
	
	enum LexState tokenState;
	int tokenFinished; // buffer should be consumed and cleaned at this point 
};




static int eatchar(struct lexer_state* st, int c) {

	#include "./parser_example_csets.c"
	
#define push_char_id(_state) \
do { \
	st->state = _state; \
	goto PUSH_CHAR_RET; \
} while(0)


#define discard_char_id(_state) \
do { \
	st->state = _state; \
	return 1; \
} while(0)


#define retry_as(_state) \
do { \
	st->state = _state; \
	goto RETRY; \
} while(0);

#define done_zero_move(_state) \
do { \
	st->state = _state; \
	goto TOKEN_DONE; \
} while(0);

#define push_char_done(_state) \
do { \
	st->state = _state; \
	goto PUSH_CHAR_DONE; \
} while(0);

#define charset_has(cs, c) (c <= cs##_len && !!cs[c])

	// hopefully this works
	st->charnum++;
	if(c == '\n') {
		st->linenum++;
		st->charnum = 0;
	}
	

RETRY:
	switch(st->state) {

		#include "./parser_example_switch.c"
		
		
		default: 
			printf("Lexer reached default: %d\n", st->state);
			st->state = LST_NULL; 
			return 0;
	}
	
	assert(0);
	// never gets here
ERROR:
	printf("Lexer error: %d(%s) %d '%c' \n", st->state, state_names[st->state], c, c);
	st->state = LST_NULL; 
	st->blen = 0;
	return 1;

TOKEN_DONE:
	st->tokenFinished = 1;
	st->tokenState = st->state;
	return 0;
	
PUSH_CHAR_RET: 
	st->buffer[st->blen] = c; 
	st->blen++;
	return 1;
	
PUSH_CHAR_DONE: 
	st->buffer[st->blen] = c; 
	st->blen++;
	
	st->tokenFinished = 1;
	st->tokenState = st->state;
	return 1; 


}




int main(int argc, char* argv[]) {
	
// 	char* source = "func(int x, 123 -123 0.123 .123 0123 0x123 1.23e4 1.23E-45 /*yd*fg*/) // error z;\nfoo";
	
	if(argc < 2) {
		fprintf(stderr, "test file expected as first argument.\n");
		exit(1);
	}
	
	char* source = readWholeFile(argv[1], NULL);
	
	struct lexer_state ls = {
		.state = 0,
		.balloc = 256,
		.blen = 0,
		.buffer = calloc(1, 256),
		
		.state = LST_NULL,
		.linenum = 0,
		.charnum = 0,
		
		.tokenState = 0,
		.tokenFinished = 0,
	};
	
	for(int i = 0; source[i];) {
		int ret;

// 				printf(" eating char: %c\n", line[i]);
		ret = eatchar(&ls, source[i]);
		
// 			printf(" post-state: %s\n", stateNames[ls.state]);
		
		if(ls.tokenFinished) { 
			// token is ready
			
			printf("got token: #%d (%s) '%.*s'\n", ls.tokenState, state_names[ls.tokenState], ls.blen, ls.buffer);
			
			// reset the lex state when done reading
			ls.tokenFinished = 0;
			ls.state = LST_NULL;
			ls.blen = 0;
			
		}
		
		if(ret) {
			i++; // advance on ret == 1
		}
	}

	printf("last token: #%d (%s) '%.*s'\n", ls.tokenState, state_names[ls.tokenState], ls.blen, ls.buffer);

	
	return 0;
}


