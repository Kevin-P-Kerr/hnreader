// this is a special-case xml parser for xml returned from hacker news
// if you use this for anything else it probably wont' work
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#define MAXVAL 10000
#define HASHSIZE 101
#define TRUE 1
#define FALSE 0
#define INVAL 100000
#define L_ARROW 128
#define R_ARROW 129
#define QUOTE 130
#define STRING 131
#define LINK 132
#define CLINK 133
#define TITLE 134
#define CTITLE 135
#define DESCRIPTION 136
#define CDESCRIPTION 137
#define COMMENT 138
#define CCOMENT 139
#define OTHER 140

char input[INVAL];

struct nlist { // the symbol table
	struct nlist *next;
	char *name;
	char *defn;
};

static struct nlist *hashtab[HASHSIZE];

// hash forms a hash value for look up and install
unsigned hash(char *s) {
	unsigned hashval;
	
	for (hashval = 0; *s != '\0'; s++) 
		hashval = *s + 31 * hashval;
	return hashval % HASHSIZE;
}

struct nlist  *lookup(char *s) {
	struct nlist *np;

	for (np = hashtab[hash(s)]; np!=NULL; np = np->next)
		if (strcmp(s, np->name) == 0)
			return np; 
		return NULL;
}

struct nlist *install(char *name, char *defn) {
	
	struct nlist *np;
	unsigned hashval;

	if ((np=lookup(name))==NULL) {
		np = (struct nlist *) malloc(sizeof(*np));
		if (np == NULL || (np->name = strdup(name)) == NULL)
			return NULL;
		hashval = hash(name);
		np->next = hashtab[hashval];
		hashtab[hashval] = np;
	} else if ((np=lookup(name))!=NULL) 
		free ((void *) np->defn);
	if ((np->defn = strdup(defn)) == NULL)
		return NULL;
	return np;
}

void inithash(void) {
	struct nlist *hlink;
	struct nlist *htitle;
	struct nlist *hdescription;
	struct nlist *hclink;    // c means closed, i.e we want a "closed title" token type.
	struct nlist *hctitle;
	struct nlist *hcdescription;
	struct nlist *hcomment;
	struct nlist *hccomment;
	
	char *link = "132";
	char *title = "134";
	char *description = "136";
	char *clink = "133";
	char *ctitle = "135";
	char *cdescription = "137";
	char *comment = "138";
	char *ccomment = "139";


	hlink = install("<link>", link);
	htitle = install("<title>", title);
	hdescription = install("<description>", description);
	hclink = install("</link>", clink);
	hctitle = install("</title>", ctitle);
	hcdescription = install("</description>", cdescription);	
	hcomment = install("<comments>", comment);
	hccomment = install("</comments>", ccomment);
}

typedef struct token {
	int *type;
	char *value;
} Token;
	

struct Lexer {
	char *start;
	char *end;
};

struct Lexer lexer; 

int issymbol(char c) {
	if ((c == '<') || (c == '>') || (c == '"')) 		
		return 1;
	else
		return 0;
}

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

	if (*lexer.end=='<') {
		while (*lexer.end!='>')
			++lexer.end;
		diff = lexer.end - lexer.start;
		return diff;
	} else if (*lexer.end!='<') {
		while ((*lexer.end!='<') && (*lexer.end!=EOF))
			++lexer.end;
		diff = lexer.end - lexer.start;
		return diff;
    } else 
		fprintf(stderr, "GET DIFFERENCE ERROR!\n");
}
	
	
void resetInput(void) {	
 
	if (*lexer.end == '\0')
		;
	else if (*lexer.end == '\n') {
		++lexer.end;
		lexer.start = lexer.end;
  } else if (*lexer.end == '<')
		lexer.start = lexer.end;
	else if (*lexer.end == '>') { //move lexer.end onto the first character of the next string
		++lexer.end;
		if (*lexer.end == '\n') {
			++lexer.end;
			lexer.start = lexer.end;
			}
		else
			lexer.start = lexer.end;
}
		
	else {
		while ((*lexer.start!='\0') && junk(*lexer.start))		
			++lexer.start;
		lexer.end = lexer.start;
}
}  // is this necessary anymore? 

Token *GetToken(void) {
	int diff = GetDifference();
	Token *p = malloc(sizeof(Token));
	struct nlist *look;

	if (*lexer.start == '<') {
		p->type = malloc(sizeof(int));
		p->value = malloc((sizeof(char) * diff) + 2);
		strncpy(p->value, lexer.start, (++diff));
		p->value[diff + 1] = '\0';
		if((look=lookup(p->value))!=NULL)
			*p->type = atoi(look->defn);
		else
			*p->type = OTHER;
    }
		
		
	
/*	if (*lexer.start == '<') {
		p->type = malloc(sizeof(int));
		*p->type = L_ARROW;
		p->value = malloc((sizeof(char) * diff) + 1);
		strncpy(p->value, lexer.start, diff);
		p->value[diff + 1] = '\0'; */

	else if (*lexer.start == '>') {
		p->type = malloc(sizeof(int));
	    *p->type = R_ARROW;
		p->value = malloc((sizeof(char) * diff) +1);
		strncpy(p->value, lexer.start, diff);
		p->value[diff + 1] = '\0';
}

	else if (*lexer.start == '"') {
		p->type = malloc(sizeof(int));
		*p->type = QUOTE;
		p->value = malloc((sizeof(char) * diff) + 1);
		strncpy(p->value, lexer.start, diff);
		p->value[diff + 1] = '\0';
}
	else if ((isalnum(*lexer.start)) || (!issymbol(*lexer.start))) {
		p->type = malloc(sizeof(int));
		*p->type = STRING;
		p->value = malloc((sizeof(char) * diff) + 1);
		strncpy(p->value, lexer.start, diff);
		p->value[diff + 1] = '\0';
} 
	else {
		fprintf(stderr, "GET TOKEN ERROR");
}		

	 
	resetInput();
	return p;			
}	

void printString(Token *p) {
	int i=0;
	while (p->value[i]!='\0') { 
		printf("%c", p->value[i]);
		++i;
}
	printf("\n");
}
			
	

main()  {
GetInput();
inithash();
init_lexer();
while(*lexer.start!='\0') {

	Token *p = GetToken();
	printf("%d\n", *p->type);
	printString(p);
}
printf("END OF STREAM\n");
}

	




