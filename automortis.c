#include <stdlib.h>
#include <stdio.h>

#include <sys/stat.h>
#include <error.h>
#include <sched.h>
#include <unistd.h>

#include <X11/Xlib.h>

static char s_rcpath[FILENAME_MAX+1];

int death_pill(Display* unused)
{
	execl(s_rcpath, s_rcpath, NULL);
}

int main(int argc, char** argv)
{
	int error;
	char* home;
	Display* display;
	struct stat rcstat;
	
	/* Check our environment. */
	home = getenv("HOME");
	if (home == NULL)
	{
		errx(-1, "HOME must be set");
	}
	
	/* Check our RC file. */
	snprintf(s_rcpath, FILENAME_MAX + 1, "%s/.automortis", home);
	error = stat(s_rcpath, &rcstat);
	if (error)
	{
		err(-1, "Error stating ~/.automortis");
	}
	
	if (!(rcstat.st_mode & S_IXUSR))
	{
		errx(-1, "~/.automortis must be executable");
	}

	/* Connect to the X display. */
	display = XOpenDisplay(NULL);
	if (display == NULL)
	{
		errx(-1, "Could not open display");
	}

	/* Set up the error handler. */
	XSetErrorHandler(death_pill);
	XSetIOErrorHandler(death_pill);
	
	/* Background self. */
	if (fork())
	{
		exit(0);
	}
	setpgrp();

	/* Sleeeeeeeeeeeep. */
	while (1)
	{
		XNoOp(display);
		XSync(display, 0);
		sleep(10);
	}	
}
