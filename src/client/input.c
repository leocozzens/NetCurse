#include <input.h>

static void clear_input(void);

static void clear_input(void) {
    int c;
    while((c = fgetc(stdin)) != '\n' && c != EOF);
}

void get_input(char *buffer, int bufferSize) {
    if(!fgets(buffer, bufferSize, stdin)) exit(0);

    char *endLine = strchr(buffer, '\n');
    if(endLine != NULL) *endLine = '\0';
    else clear_input();
}