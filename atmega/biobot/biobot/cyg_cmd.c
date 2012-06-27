#include "biobot_config.h"
#include "cyg_cmd.h"

static const char ROMSTR_VER[] EEMEM = "\r\n*VER:21\r\n";

static const char* STR_ERROR = "\r\nERROR\r\n";
static const char* STR_OK = "\r\nOK\r\n";
static const char* STR_CYQD = "\r\n*CYQD:%d,%d,%d,%d,%d,%d\r\n";

struct AT_CMMD g_user_rtcmmd;

static void (*reset)(void) = 0x0000;

int at_ver(void)
{
  fprintf_P(gfile_shell, ROMSTR_VER);
  return TRUE;
}

int at_reset(void)
{
  reset();
  return TRUE;
}

int at_at(void)
{
	return TRUE;
}



static struct AT_CMMD_HANDLES g_cmmd_handles[] =
{					
    { "AT", &at_at},       // test AT support or not		
	{ "AT*VER", &at_ver }, // kernel version
    { "AT*RESET", &at_reset }, // system reset
}; // debug on/off			

int
cygParserCmd(char * cmd_buffer)
{ 
 
  char* p = cmd_buffer;
  char* buf_ptr = cmd_buffer;

  while (TRUE)
    {
      if (*p == '=')
        {
          *p++ = '\0';
          g_user_rtcmmd.cmmd = buf_ptr;
          buf_ptr = p;
        }
      else if (*p == ',')
        {
          *p++ = '\0';
          g_user_rtcmmd.params[g_user_rtcmmd.param_count++] = buf_ptr;
          buf_ptr = p;
        }
      else if (*p == '\0' || *p == '\r' || *p == 0xff)
        {
		  *p = '\0';
          if (buf_ptr != p)
            {
              if (NULL == g_user_rtcmmd.cmmd)
                {
                  g_user_rtcmmd.cmmd = buf_ptr;
                }
              else
                {
                  g_user_rtcmmd.params[g_user_rtcmmd.param_count++] = buf_ptr;
                }
            }

          break;
        }
      else 
        {
          p++;
        }
    }

  return TRUE;
}

/**
 * AT-Command main handle
 */
int
cygCmdMainProc(FILE* stream)
{
  if (stream == NULL)
    return FALSE; 

  int i;
  for (i = 0; i < SIZEOF(g_cmmd_handles); ++i)
  {
	  //DbgPrint("$%s, %s\r\n", g_cmmd_handles[i].cmmd, g_user_rtcmmd.cmmd);
      if (0 == strcmp(g_cmmd_handles[i].cmmd, g_user_rtcmmd.cmmd))
        {
          if (g_cmmd_handles[i].cmmd_handle)
            {
              if (g_cmmd_handles[i].cmmd_handle())
                {
                  fprintf(gfile_shell, STR_OK);
                }
              else
                {
                  fprintf(gfile_shell, STR_ERROR);
                }
            };
          return TRUE;
        }
    }

  if (i >= SIZEOF(g_cmmd_handles))
    {
      fprintf(gfile_shell, STR_ERROR);
      return FALSE;
    }

  return FALSE;
}

char* cygCmdClean(const char* buf, uint16_t len)
{
	const char *p = buf;	
	while(len--){
		if(*p != EOF && *p != '\0' && *p != '\r' && *p != '\n' && *p != ' ') return p;
		p++;
	}
	
	return NULL;
}