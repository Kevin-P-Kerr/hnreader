// this is a special-case xml parser for xml returned from hacker news
// if you use this for anything else it probably wont' work
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#define MAXVAL 10000
#define HASHSIZE 101
#define PAGESIZ 22
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
#define COMMENTS 138
#define CCOMENTS 139
#define ITEM 140
#define CITEM 141
#define OTHER 142

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
	struct nlist *hitem;
	struct nlist *hcitem;
	
	char *link = "132";
	char *title = "134";
	char *description = "136";
	char *clink = "133";
	char *ctitle = "135";
	char *cdescription = "137";
	char *comment = "138";
	char *ccomment = "139";
	char *item = "140";
	char *citem = "141";


	hlink = install("<link>", link);
	htitle = install("<title>", title);
	hdescription = install("<description>", description);
	hclink = install("</link>", clink);
	hctitle = install("</title>", ctitle);
	hcdescription = install("</description>", cdescription);	
	hcomment = install("<comments>", comment);
	hccomment = install("</comments>", ccomment);
	hitem = install("<item>", item);
	hcitem = install("</item>", citem);
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
		exit(1);
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

FILE *efopen(char *file, char *mode) {

	FILE *fp;
//	extern char *progname;

	if ((fp=fopen(file, mode))!=NULL)
		return fp;
	fprintf(stderr, "%s: can't open file mode %s\n", file, mode);
	exit(1);
}

char ttyin(void) {
	
	char buf[BUFSIZ];
	static FILE *tty = NULL;

	if (tty == NULL) 
		tty = efopen("/dev/tty", "r");
	if ((fgets(buf, BUFSIZ, tty)) == NULL || buf[0] =='q')
		exit(0);
	else
		return buf[0];
}

void resetbuf(char *bp) {

	int i = 0;

	while ((bp[i]!='\0') && (i < BUFSIZ)) {
		bp[i] = '\0';
		++i;
	}
}

void getcomments(char *cbp) {
	Token *tk = GetToken();

	while (*tk->type!=COMMENTS)
		tk = GetToken();
	tk = GetToken(); // get one more token for the link to comments
	strcpy(cbp, tk->value);
}
	
void getlink(char *lbp, char *cbp) {
	
	Token *tk = GetToken();
	
	while (*tk->type!=LINK) 
		tk = GetToken();
	tk = GetToken(); // get one more token for the actual link
	strcpy(lbp, tk->value);
	getcomments(cbp);
}

void parse(char *tbp, char *lbp, char *cbp) {

	Token *tk = GetToken();

	while (*tk->type!=TITLE) {
		free(tk);
		tk = GetToken();
	}
	free(tk);
	tk = GetToken(); // get one more token for the actual title
	resetbuf(tbp);
	strcpy(tbp, tk->value);
	getlink(lbp, cbp);
}
	
void w3m(char *bp) {
	char com[50]; // a command of fifty characters
	char fn[50] = "tmp.XXXXXX";
	char buf[BUFSIZ]; // IO goes here
	char bn; // ttyin back or next
	FILE *fp, *fpntr;
	int i, f, lines;
	bn = 'n';

	lines = 0; 
	
	if ((f=mkstemp(fn))<0) {
		fprintf(stderr, "W3M ERROR: COULD NOT MAKE TMP FILE\n");
		exit(1);
	}

	sprintf(com, "curl %s > tmp.html", bp);
	system(com);
	sprintf(com, "w3m tmp.html > %s", fn);
	system(com);
	fp = efopen(fn, "r");

	while((fgets(buf, sizeof buf, fp)!=NULL) && (bn!='b')) {
		if(++lines < PAGESIZ)
			fputs(buf, stdout);
		else {
			buf[strlen(buf)-1] = '\0';
			fputs(buf, stdout); 
			lines = 0;
			bn = ttyin();
		}
	}
}

getrss(void) {
	
	char fn[BUFSIZ] = "hnreader.XXXXXX";
	char com[BUFSIZ] = "hcurl.XXXXXX";
	int i=0;
	int f; // file descriptor I don't care about
	if ((f=mkstemp(com))<0 || (f=mkstemp(fn))<0)
		fprintf(stderr, "GET RSS FILE\nERROR MAKING FILE\n");
	FILE *fp = efopen(com, "w");
	fprintf(fp, "curl news.ycombinator.com/rss > %s", fn);
	fclose(fp);
	fp = efopen(com, "r");
	char cmd[BUFSIZ];
	while ((cmd[i]=fgetc(fp))!=EOF)
		++i;
	cmd[i] = '\0';
	i = 0; //reset i for later use
	fclose(fp);
	system(cmd);
	fp = efopen(fn, "r");
	while ((input[i]=fgetc(fp))!=EOF) 
		++i;
	input[i] = '\0';
	fclose(fp);
	unlink(fn);
	unlink(com);
}
	
	
	
	
	
int main(int argc, char *argv[])  {

   // char *progname = argv[0];
	int i = 0;
	char tbuf[BUFSIZ], lbuf[BUFSIZ], cbuf[BUFSIZ], yn; 
	
	inithash();
	getrss();
	init_lexer();
	while (*lexer.end!='\0') {
		parse(tbuf, lbuf, cbuf);
		fprintf(stderr, "%s\n", tbuf);
		yn = ttyin();
		if (yn == 's') 
			w3m(lbuf);
		else if (yn == 'c')
			w3m(cbuf);
		else if (yn == 'n')
				; // do nothing
		else {
			fprintf(stderr, "HNREADER: ARGUEMENTES n, c, or s\n");
			exit(1);
		}
	}
	return 0; 
}	
