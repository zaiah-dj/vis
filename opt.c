#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <ctype.h>
#include "opt.h"

void usage (Opt *opts, int status) {
	while (!opts->sentinel) { 
		if (!opts->arg) 
			fprintf(stderr, "%-2s, %-20s %s\n", opts->sht ? opts->sht : " " , opts->lng, opts->description);
		else {
			char argn[1024]; memset(&argn, 0, 1024);
			sprintf(argn, "%s <arg>", opts->lng);
			fprintf(stderr, "%-2s, %-20s %s\n", opts->sht ? opts->sht : " " , argn, opts->description);
		}
		opts++;
	}
	exit(status);
}


void opt_set_filename (char **av, Value *v) { av++; v->s = *av; }
void opt_set_dir (char **av, Value *v) { av++; v->s = *av; }
void opt_set_fps (char **av, Value *v) { 
	av++; 
	char *a=*av; 
	fprintf(stderr, "%s\n", a);
	while (*a) {
		if (!isdigit(*a)) {
			fprintf(stderr, "ERR: Flag --refresh requires a numerical argument.\n");
			exit(1);
		}
		a++;
	}

	v->n=atoi(*av);
	if (v->n < 1 || v->n > 100) {
		fprintf(stderr, "ERR: Flag --fps requires a value from 1 - 100.\n");
		exit(1);
	} 
}

void opt_set_switch (char **av, Value *v) { 
	av++; 
	char *a=*av; 
	fprintf(stderr, "%s\n", a);
	while (*a) {
		if (!isdigit(*a)) {
			fprintf(stderr, "ERR: Flag --hold requires a numerical argument.\n");
			exit(1);
		}
		a++;
	}

	v->n=atoi(*av);
	if (v->n < 1 || v->n > 1000) {
		fprintf(stderr, "ERR: Flag --hold requires a value from 1 - 1000.\n");
		exit(1);
	} 
}
