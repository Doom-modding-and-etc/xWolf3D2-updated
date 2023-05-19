#ifdef _WIN32

/*
    SDL_main.c, placed in the public domain by Sam Lantinga  4/13/98

    Modified to write stdout/stderr to a message box at shutdown by Ripper  2007-12-27

    The WinMain function -- calls your program's main() function
*/

#include <stdio.h>
#include <stdlib.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifdef _WIN32_WCE
# define DIR_SEPERATOR TEXT("\\")
# undef _getcwd
# define _getcwd(str,len)	wcscpy(str,TEXT(""))
# define setbuf(f,b)
# define setvbuf(w,x,y,z)
# define fopen		_wfopen
# define freopen	_wfreopen
# define remove(x)	DeleteFile(x)
#else
# define DIR_SEPERATOR TEXT("/")
# include <direct.h>
#endif

/* Include the SDL main definition header */
#include "SDL.h"
#include "SDL_main.h"

#ifdef main
# ifndef _WIN32_WCE_EMULATION
#  undef main
# endif /* _WIN32_WCE_EMULATION */
#endif /* main */

/* The standard output files */
#define STDOUT_FILE	TEXT("stdout.txt")
#define STDERR_FILE	TEXT("stderr.txt")

#ifndef NO_STDIO_REDIRECT
# ifdef _WIN32_WCE
  static wchar_t stdoutPath[MAX_PATH];
  static wchar_t stderrPath[MAX_PATH];
# else
  static char stdoutPath[MAX_PATH];
  static char stderrPath[MAX_PATH];
# endif
#endif

#if defined(_WIN32_WCE) && _WIN32_WCE < 300
/* seems to be undefined in Win CE although in online help */
#define isspace(a) (((CHAR)a == ' ') || ((CHAR)a == '\t'))
#endif /* _WIN32_WCE < 300 */

/* Parse a command line buffer into arguments */
static int ParseCommandLine(char *cmdline, char **argv)
{
	char *bufp;
	int argc;

	argc = 0;
	for ( bufp = cmdline; *bufp; ) {
		/* Skip leading whitespace */
		while ( isspace(*bufp) ) {
			++bufp;
		}
		/* Skip over argument */
		if ( *bufp == '"' ) {
			++bufp;
			if ( *bufp ) {
				if ( argv ) {
					argv[argc] = bufp;
				}
				++argc;
			}
			/* Skip over word */
			while ( *bufp && (*bufp != '"') ) {
				++bufp;
			}
		} else {
			if ( *bufp ) {
				if ( argv ) {
					argv[argc] = bufp;
				}
				++argc;
			}
			/* Skip over word */
			while ( *bufp && ! isspace(*bufp) ) {
				++bufp;
			}
		}
		if ( *bufp ) {
			if ( argv ) {
				*bufp = '\0';
			}
			++bufp;
		}
	}
	if ( argv ) {
		argv[argc] = NULL;
	}
	return(argc);
}

/* Show an error message */
static void ShowError(const char *title, const char *message)
{
/* If USE_MESSAGEBOX is defined, you need to link with user32.lib */
#ifdef USE_MESSAGEBOX
	MessageBox(NULL, message, title, MB_ICONEXCLAMATION|MB_OK);
#else
	fprintf(stderr, "%s: %s\n", title, message);
#endif
}

/* Pop up an out of memory message, returns to Windows */
static BOOL OutOfMemory(void)
{
	ShowError("Fatal Error", "Out of memory - aborting");
	return FALSE;
}

/* SDL_Quit() shouldn't be used with atexit() directly because
   calling conventions may differ... */
static void cleanup(void)
{
	SDL_Quit();
}

/* Remove the output files if there was no output written */
static void cleanup_output(void)
{
#if 1
#ifndef NO_STDIO_REDIRECT
	FILE *file;
#endif
#endif

	/* Flush the output in case anything is queued */
	fclose(stdout);
	fclose(stderr);

#if 1
#ifndef NO_STDIO_REDIRECT
	/* See if the files have any output in them */
	if ( stdoutPath[0] ) {
		file = fopen(stdoutPath, TEXT("r"));
		if ( file ) {
            char buf[16384];
            size_t readbytes = fread(buf, 1, 16383, file);
            fclose(file);

            if(readbytes != 0)
            {
                buf[readbytes] = 0;     // cut after last byte (<=16383)
                MessageBox(NULL, buf, "Wolf4SDL", MB_OK);
            }
            else
                remove(stdoutPath);     // remove empty file
		}
	}
	if ( stderrPath[0] ) {
		file = fopen(stderrPath, TEXT("rb"));
		if ( file ) {
            char buf[16384];
            size_t readbytes = fread(buf, 1, 16383, file);
            fclose(file);

            if(readbytes != 0)
            {
                buf[readbytes] = 0;     // cut after last byte (<=16383)
                MessageBox(NULL, buf, "Wolf4SDL", MB_OK);
            }
            else
                remove(stderrPath);     // remove empty file
		}
	}
#endif
#endif
}

//#if defined(_MSC_VER) && !defined(_WIN32_WCE)
///* The VC++ compiler needs main defined */
//#define console_main main
//#endif

#endif  // _WIN32
