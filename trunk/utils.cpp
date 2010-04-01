/***************************************************************************
 *   Copyright (C) 2007-2010 by Mayank Lahiri                              *
 *   mlahiri@gmail.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#ifdef LINUX
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
#endif


/* Returns total sys memory in kb */
int getSystemMemory()	{
	int musage = 0;
#ifdef LINUX
	FILE *fp;
	char buf[100];
	char *p;

	if((fp = fopen("/proc/meminfo", "r")))	{
		while(fgets(buf, 99, fp))	{
			if(strncmp(buf, "MemTotal:", 9) == 0)	{
				p = strchr(buf, ':')+1;
				while(*p == ' ' || *p == '\t')
					p++;
				musage = atoi(p);
				break;
			}
		}
		fclose(fp);
	}
#endif
	return musage;
}

/* Returns free sys memory in kb */
int getSystemFreeMemory()	{
	int musage = 0;
#ifdef LINUX
	FILE *fp;
	char buf[100];
	char *p;

	if((fp = fopen("/proc/meminfo", "r")))	{
		while(fgets(buf, 99, fp))	{
			if(strncmp(buf, "MemFree:", 8) == 0)	{
				p = strchr(buf, ':')+1;
				while(*p == ' ' || *p == '\t')
					p++;
				musage = atoi(p);
				break;
			}
		}
		fclose(fp);
	}
#endif
	return musage;
}


int getMemoryPeak()	{
	int musage = 0;
#ifdef LINUX
	FILE *fp;
	char buf[100];
	char *p;

	sprintf(buf, "/proc/%d/status", getpid());
	if((fp = fopen(buf, "r")))	{
		while(fgets(buf, 99, fp))	{
			if(strncmp(buf, "VmPeak:", 7) == 0)	{
				p = strchr(buf, ':')+1;
				while(*p == ' ' || *p == '\t')
					p++;
				musage = atoi(p);
				break;
			}
		}
		fclose(fp);
	}
#endif
	return musage;
}

int getMemoryCurrent()	{
	int musage = 0;

#ifdef LINUX
	FILE *fp;
	char buf[100];
	char *p;

	sprintf(buf, "/proc/%d/status", getpid());
	if((fp = fopen(buf, "r")))	{
		while(fgets(buf, 99, fp))	{
			if(strncmp(buf, "VmSize:", 7) == 0)	{
				p = strchr(buf, ':')+1;
				while(*p == ' ' || *p == '\t')
					p++;
				musage = atoi(p);
				break;
			}
		}
		fclose(fp);
	}
#endif
	return musage;
}


/*
 * USERSPACE CPU USAGE (LINUX ONLY)
 */
TIMER::TIMER()	{
	user_start = 0;
	system_start = 0;
}
void TIMER::start()	{
#ifdef LINUX
	struct rusage rbf;
	getrusage(RUSAGE_SELF, &rbf);
	user_start = ((double)rbf.ru_utime.tv_sec * 1e6) +  rbf.ru_utime.tv_usec;
	system_start = ((double)rbf.ru_stime.tv_sec * 1e6) +  rbf.ru_stime.tv_usec;
#else
#warning "-DLINUX not specified, no timing code included."
#endif
}
void TIMER::stop()	{
#ifdef LINUX
	struct rusage rbf;
	getrusage(RUSAGE_SELF, &rbf);
	user_used = ((double)rbf.ru_utime.tv_sec * 1e6) +  rbf.ru_utime.tv_usec - user_start;
	system_used = ((double)rbf.ru_stime.tv_sec * 1e6) +  rbf.ru_stime.tv_usec - system_start;
#endif
}



unsigned long hash_djb2(unsigned char *str)
{
	unsigned long hash = 5381;
	int c;

	while((c = *str++))
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}





/*
 * COMMAND LINE PARSING
 */
static void showUsage(CMDLINE_PARAMETERS *parameters, char *argv0)	{
	CMDLINE_PARAMETERS *p;

	printf("usage: %s <options>\noptions:\n", argv0);
	for(p = parameters; p->option[0]; p++)	{
		printf("  %s ", p->option);
		switch(p->arg_type) {
			case CMDLINE_INTEGER: printf("<integer in [%d,%d]>", p->min, p->max); break;
			case CMDLINE_DOUBLE:  printf("<real>              "); break;
			case CMDLINE_STRING:  printf("<string>            "); break;
			case CMDLINE_HELP:
			case CMDLINE_BINARY:  printf("                    "); break;
		}
		printf("\t(required: %s)  \t %s\n", p->required?"yes":"no", p->description);
	}
}

int ParseCommandLine(int argc, char **argv, CMDLINE_PARAMETERS *parameters)	{
	int a, t;
	double d;
	CMDLINE_PARAMETERS *p;

	for(a=1; a<argc; a++)	{
		/* find argv[a] in parameters */
		for(p=parameters; p->option[0]; p++)
			if(strcasecmp(p->option, argv[a])==0)
				break;
		if(!p->option[0])	{
			fprintf(stderr, "%s: Unknown parameter \"%s\".\n", argv[0], argv[a]);
			return 0;
		}

		/* type CMDLINE_BINARY has no argument */
		if(p->arg_type == CMDLINE_BINARY || p->arg_type == CMDLINE_HELP)	{
			if(p->arg_type == CMDLINE_BINARY)
				*((int *)p->where_to) = 1;
			else	{
				showUsage(parameters, argv[0]);
				return 0;
			}
		}
		else {
			/* the others do */
			if(++a == argc)	{
				fprintf(stderr, "%s: Parameter \"%s\" requires an argument.\n", argv[0], argv[--a]);
				return 0;
			}
			switch(p->arg_type)	{
				case CMDLINE_INTEGER:
					t = atoi(argv[a]);
					if(t < p->min || (t > p->max && p->max > p->min))	{
						fprintf(stderr, "%s: Parameter \"%s\" is out of range.\n", argv[0], argv[--a]);
						return 0;
					}
					*((int *)p->where_to) = t;
					break;

				case CMDLINE_STRING:
					if((int)strlen(argv[a]) < p->min || (int)strlen(argv[a]) > p->max)	{
						fprintf(stderr, "%s: Argument for parameter \"%s\" is either too long or too short.\n", argv[0], argv[--a]);
						return 0;
					}
					strncpy((char *)p->where_to, argv[a], strlen(argv[a]));
					*((char *)p->where_to+strlen(argv[a])) = 0;
					break;

				case CMDLINE_DOUBLE:
					d = strtod(argv[a], NULL);
					if(d < (double)p->min || d > (double)p->max)	{
						fprintf(stderr, "%s: Argument for parameter \"%s\" is out of range.\n", argv[0], argv[--a]);
						return 0;
					}
					*((double *)p->where_to) = d;
					break;

				default:
					fprintf(stderr, "%s: Program error.\n", argv[0]);
					break;
			}

			/* made it through this option successfully */
			if(p->required)
				p->required = 0;
		}
	}

	for(p=parameters; p->option[0]; p++)
		if(p->required)	{
			fprintf(stderr, "%s: Option \"%s\" not specified: %s\n", argv[0], p->option, p->description);
			return 0;
		}

	return 1;
}



/*
 * TEXT PROCESSING
 */
bool chomp(char *s)	{
	s--;
	while(*(++s))
		if(*s=='\n' || *s=='\r')	{
			*s = 0;
			return true;
		}
	return false;
}



