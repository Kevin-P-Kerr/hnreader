// this is a special-case xml parser for xml returned from hacker news
// if you use this for anything else it probably wont' work
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#define MAXVAL 10000
#define TRUE 1
#define FALSE 0
#define INVAL 100000
#define L_ARROW 128
#define R_ARROW 129
#define QUOTE 130


char input[INVAL];

typedef struct token {
	int *type;
	char *value;
} Token;
	

struct Lexer {
	char *start;
	char *end;
};

struct Lexer lexer; 

int junk(char c) {
	if (isspace(c)) 
		return 1;
	else
		return 0;
}

void init_lexer(void) {
	lexer.start = input;
	if (junk(*lexer.start)) {
		while (junk(*lexer.start))
			++lexer.start;
	}
	lexer.end = lexer.start;
}


void GetInput(void) {
	
	int i=0;
	char c;

	while (((c=getchar())!=EOF) && (i < INVAL)) {
		input[i] = c;
		++i;
}
	if (c!=EOF) { // the input is too long
		fprintf(stderr, "INPUT ERROR: INPUT TOO LONG\n");
		exit(0);
}
	else {
		input[i] = '\0'; // this will help us exit gracefully
}		 
}
	
int GetDifference(void) {
	int diff;

	while((!junk(*lexer.end)) && (*lexer.end!='\0')) 
		++lexer.end;
	diff = lexer.end - lexer.start;
	return diff;
}
	
void resetInput(void) {	
	lexer.start = lexer.end;
	if (*lexer.end == '\0')
		;
	else {
		while ((*lexer.start!='\0') && junk(*lexer.start))		
			++lexer.start;
		lexer.end = lexer.start;
}
}

Token *GetToken(void) {
	int diff = GetDifference();
	Token *p = malloc(sizeof(Token));
	
	if (*lexer.start == '<') {
		p->type = malloc(sizeof(int));
		*p->type = L_ARROW;
		p->value = malloc(2 * sizeof(char));
		strcpy(p->value, "<");
} 
	resetInput();
	return p;			
}	

	
	

main()  {
GetInput();
init_lexer();
while(*lexer.start!='\0') {

	Token *p = GetToken();
	printf("%d\n", *p->type);
	printf("%c\n", *p->value);
}
printf("END OF STREAM\n");
}

	




