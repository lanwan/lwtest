#ifndef _CYG_CMD_H_
#define _CYG_CMD_H_

/**
 * AT-COMMAND support functions and definition
 */

#define MAX_COMMAND_PARAMS_COUNT 	20
struct AT_CMMD_HANDLES
{
	const char* cmmd;
	int (*cmmd_handle)(void);
};

struct AT_CMMD
{
	char* cmmd;
	int8_t param_count;
	char* params[MAX_COMMAND_PARAMS_COUNT];
};

int cygParserCmd(char * cmd_buffer);
int cygCmdMainProc(FILE* stream);
char* cygCmdClean(const char* buf, uint16_t len);

#endif // _CYG_CMD_H_
