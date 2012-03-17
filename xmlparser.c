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

void init_lexer(void) {
lexer.start = input;
lexer.end = input;
}

void GetInput(void) {
	
	int i=0;
	char c;

	while (((c=getchar())!=EOF) && (c < INVAL)) {
		input[i] = c;
		++i;
}
	
	if (i <= INVAL) {
		--i;
		if ((!isalnum(input[i])) && (input[i]!='<'))
			input[i] = '\0';
		else
			input[i+1] = '\0';
}

	else {
		fprintf(stderr, "INPUT ERROR, STIRNG TO LONG");
}
}
int GetDifference(void) {
	int diff;

	lexer.start = lexer.end;
	while ((!isalnum(*lexer.start)) && ((*lexer.start != '<'))) 
		++lexer.start;
	lexer.end = lexer.start;
	while ((isalnum(*lexer.end)) || (*lexer.end == '<'))
		++lexer.end;
	diff = lexer.start - lexer.end;
	return diff;
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
	return p;			
}	

	
	

main()  {
GetInput();
init_lexer();
while(*lexer.end!='\0') {

	Token *p = GetToken();
	printf("%d\n", *p->type);
	printf("%c\n", *p->value);
}
printf("END OF STREAM\n");
}

	




