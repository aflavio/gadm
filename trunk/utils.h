/* utils.h
 *
 * General purpose, reusable routines 
 *
 * (c) 2008 Mayank Lahiri
 *
 */
#ifndef __UTILS_H
#define __UTILS_H


/* 
 * COMMAND LINE PARSING
 */
enum { CMDLINE_BINARY, CMDLINE_INTEGER, CMDLINE_DOUBLE, CMDLINE_STRING, CMDLINE_HELP };	/* arg types */
typedef struct CMDLINE_PARAMETERS	{
	char option[20];
	int arg_type;
	void *where_to;
	int min, max;
	int required;
	char description[100];
} CMDLINE_PARAMETERS;

int ParseCommandLine(int argc, char **argv, CMDLINE_PARAMETERS *parameters);


/*
 * MEMORY USAGE QUERYING FOR LINUX
 */

int getSystemMemory();
int getSystemFreeMemory();
int getMemoryPeak();
int getMemoryCurrent();

/*
 * USERSPACE CPU USAGE IN SECONDS (LINUX ONLY)
 */
class TIMER	{
	public:
		TIMER();
		void start();
		void stop();
		double system() { return system_used; };
		double user()   { return user_used; };
		double total()	{ return system_used+user_used; };

	private:
		double system_start;
		double user_start;
		double system_used;
		double user_used;
};

/*
 * TEXT ROUTINES
 */
bool chomp(char *s);


#endif
