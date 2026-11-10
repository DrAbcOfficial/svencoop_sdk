#ifndef SERVER_PARSE_H
#define SERVER_PARSE_H

enum { MAX_COM_TOKEN_LENGTH = 1500 };

extern char com_token[MAX_COM_TOKEN_LENGTH];
char *COM_Parse(char *data);

#endif
