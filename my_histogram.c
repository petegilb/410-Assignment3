// #!/bin/sh
// # test.cgi simple test

/*
*	Student: Alex Thomas
*	Assignment: 3 (Fourth assignment)
*	Date Created: 12/17/2020
*
* Resources:
*   1. APUE Steven's Textbook
*/

// Includes
#include "apue.h"
#include <dirent.h>
#include <limits.h>
#include <math.h>
// #include <stdlib.h>
// #include <stdio.h>

/* function type that is called for each filename */
typedef	int	Myfunc(const char *, const struct stat *, int);

static Myfunc	myfunc;
static int		myftw(char *, Myfunc *);
static int		dopath(Myfunc *);

#define NUM_POINTS 5
#define NUM_COMMANDS 10
#define NUM_FILE_TYPES 7

static long	nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;

/* Helper Functions */

// int bin(float x, float width) {
//     return width*floor(x/width);
// }

/* Main Entry */

int main(int argc, char *argv[])
{

  /* Default Values */
  char * DEFAULT_PATH = ".";

  /* Variables */
	int		ret;
  char* start_path;

  /* Error - Then go to default */
	if (argc != 2) {
    // err_quit("usage:  ftw  <starting-pathname>");
    // printf("%s\nsetting start path to current directory...\n","usage:  ftw  <starting-pathname>");
    start_path = DEFAULT_PATH;
  } else {
    start_path = argv[1];
  }

	ret = myftw(start_path, myfunc);		/* does it all */

	ntot = nreg + ndir + nblk + nchr + nfifo + nslink + nsock;
	if (ntot == 0)
		ntot = 1;		/* avoid divide by 0; print 0 for all counts */
	// printf("regular files  = %7ld, %5.2f %%\n", nreg,
	//   nreg*100.0/ntot);
	// printf("directories    = %7ld, %5.2f %%\n", ndir,
	//   ndir*100.0/ntot);
	// printf("block special  = %7ld, %5.2f %%\n", nblk,
	//   nblk*100.0/ntot);
	// printf("char special   = %7ld, %5.2f %%\n", nchr,
	//   nchr*100.0/ntot);
	// printf("FIFOs          = %7ld, %5.2f %%\n", nfifo,
	//   nfifo*100.0/ntot);
	// printf("symbolic links = %7ld, %5.2f %%\n", nslink,
	//   nslink*100.0/ntot);
	// printf("sockets        = %7ld, %5.2f %%\n", nsock,
	//   nsock*100.0/ntot);

  /* GNU Plot - https://stackoverflow.com/questions/3521209/making-c-code-plot-a-graph-automatically */
  // open (GNUPLOT, "|gnuplot"); // Notice the vertical bar for a pipe

  /* GNU Commands - (Histogram): https://stackoverflow.com/questions/10462546/gnuplot-histogram-line-0-too-many-columns-in-using-specification */
  int binwidth=5;
  char* y_label = "frequency";
  char* x_labels[NUM_FILE_TYPES] = {"regular", "directory", "link", "fifo", "socket", "block", "character"};
  char * commandsForGnuplot[] = {"set title \"File Count\"",
                                 "set xtics rotate out",
                                 "set key off",
                                 "set ylabel \"frequency\"",
                                 "set boxwidth 0.9 relative",
                                 "set style data histograms",
                                 "set style fill solid 1.0 border -1",
                                 // "set term x11 persist",
                                 "set term jpeg",
                                 "set output 'plot.jpg'",
                                 "plot 'data.temp' using 2:xticlabels(3)"};

  double xvals[NUM_FILE_TYPES] = {1,2,3,4,5,6,7};
  double yvals[NUM_FILE_TYPES] = {nreg, ndir, nslink, nfifo, nsock, nblk, nchr};
  FILE * temp = fopen("data.temp", "w");

  /*Opens an interface that one can use to send commands as if they were typing into the
  *     gnuplot command line.  "The -persistent" keeps the plot open even after your
  *     C program terminates.
  */
  FILE * gnuplotPipe = popen ("gnuplot -persistent", "w");
  int i;
  for (i=0; i < NUM_FILE_TYPES; i++) {
    fprintf(temp, "%lf %lf %s \n", xvals[i], yvals[i], x_labels[i]); //Write the data to a temporary file
  }

  for (i=0; i < NUM_COMMANDS; i++) {
    fprintf(gnuplotPipe, "%s \n", commandsForGnuplot[i]); //Send commands to gnuplot one by one.
  }

  /* Pretty Print as HTML */
  printf("Content-type: text/html\n\n");
  printf("<html>\n");
  printf("<img src=\"plot.jpg\">\n");
  printf("</html>\n");

	exit(ret);
}

/*
 * Descend through the hierarchy, starting at "pathname".
 * The caller's func() is called for every file.
 */
#define	FTW_F	1		/* file other than directory */
#define	FTW_D	2		/* directory */
#define	FTW_DNR	3		/* directory that can't be read */
#define	FTW_NS	4		/* file that we can't stat */

static char	*fullpath;		/* contains full pathname for every file */

static int					/* we return whatever func() returns */
myftw(char *pathname, Myfunc *func)
{
	int len;
	fullpath = path_alloc(&len);	/* malloc's for PATH_MAX+1 bytes */
									/* ({Prog pathalloc}) */
	strncpy(fullpath, pathname, len);	/* protect against */
	fullpath[len-1] = 0;				/* buffer overrun */

	return(dopath(func));
}

/*
 * Descend through the hierarchy, starting at "fullpath".
 * If "fullpath" is anything other than a directory, we lstat() it,
 * call func(), and return.  For a directory, we call ourself
 * recursively for each name in the directory.
 */
static int					/* we return whatever func() returns */
dopath(Myfunc* func)
{
	struct stat		statbuf;
	struct dirent	*dirp;
	DIR				*dp;
	int				ret;
	char			*ptr;

	if (lstat(fullpath, &statbuf) < 0)	/* stat error */
		return(func(fullpath, &statbuf, FTW_NS));
	if (S_ISDIR(statbuf.st_mode) == 0)	/* not a directory */
		return(func(fullpath, &statbuf, FTW_F));

	/*
	 * It's a directory.  First call func() for the directory,
	 * then process each filename in the directory.
	 */
	if ((ret = func(fullpath, &statbuf, FTW_D)) != 0)
		return(ret);

	ptr = fullpath + strlen(fullpath);	/* point to end of fullpath */
	*ptr++ = '/';
	*ptr = 0;

	if ((dp = opendir(fullpath)) == NULL)	/* can't read directory */
		return(func(fullpath, &statbuf, FTW_DNR));

	while ((dirp = readdir(dp)) != NULL) {
		if (strcmp(dirp->d_name, ".") == 0  ||
		    strcmp(dirp->d_name, "..") == 0)
				continue;		/* ignore dot and dot-dot */

		strcpy(ptr, dirp->d_name);	/* append name after slash */

		if ((ret = dopath(func)) != 0)		/* recursive */
			break;	/* time to leave */
	}
	ptr[-1] = 0;	/* erase everything from slash onwards */

	if (closedir(dp) < 0)
		err_ret("can't close directory %s", fullpath);

	return(ret);
}

static int
myfunc(const char *pathname, const struct stat *statptr, int type)
{
	switch (type) {
	case FTW_F:
		switch (statptr->st_mode & S_IFMT) {
		case S_IFREG:	nreg++;		break;
		case S_IFBLK:	nblk++;		break;
		case S_IFCHR:	nchr++;		break;
		case S_IFIFO:	nfifo++;	break;
		case S_IFLNK:	nslink++;	break;
		case S_IFSOCK:	nsock++;	break;
		case S_IFDIR:
			err_dump("for S_IFDIR for %s", pathname);
					/* directories should have type = FTW_D */
		}
		break;

	case FTW_D:
		ndir++;
		break;

	case FTW_DNR:
		err_ret("can't read directory %s", pathname);
		break;

	case FTW_NS:
		err_ret("stat error for %s", pathname);
		break;

	default:
		err_dump("unknown type %d for pathname %s", type, pathname);
	}

	return(0);
}
