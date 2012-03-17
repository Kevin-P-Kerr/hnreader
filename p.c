// this is mostly drawn from Pike and Kernighan's "The Unix Programming Environment"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PAGESIZE 22
char *progname;

char ttyin(void) {
	char buf[BUFSIZ];
	FILE *efopen();
	static FILE *tty = NULL;

	if (tty == NULL) {
		tty = efopen("/dev/tty", "r");
}
	else if (fgets(buf, BUFSIZ, tty) == NULL || buf[0] =='q') {
		exit(0);
}
	else {
		return buf[0];
}
}

FILE *efopen(char *file, char *mode) {
	FILE *fp;
	extern char *progname;

	if ((fp = fopen(file, mode)) != NULL) {
		return fp;
}
	fprintf(stderr, "%s: can't open file %s mode %s\n", progname, file, mode);
	exit(1);
}

void print(FILE *fp, int pagesize) {
	static int lines = 0;
	char buf[BUFSIZ];

	while (fgets(buf, sizeof buf, fp) != NULL)
		if (++lines < pagesize) {
			fputs(buf, stdout);
}
		else {
			buf[strlen(buf)-1] = '\0';
			fputs(buf, stdout);
		// not sure how this works	fflush(buf);
			ttyin();
			lines = 0;
	}
}

main(int argc, char *argv[]) {
	int i;
	FILE  *fp;

	progname = argv[0];
	if (argc == 1) {
		print(stdin, PAGESIZE);
}
	else {
		for (i = 1; i < argc; i++) {
		fp = efopen(argv[i], "r");
		print(fp, PAGESIZE);
		fclose(fp);
}
}
	exit(0);
}
