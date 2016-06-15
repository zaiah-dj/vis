typedef struct Value Value;
struct Value {
	#if 0
	union { int32_t n; char *s; char c; void *v; } value;
	#endif
	int32_t n; char *s; char c; void *v; 
};

typedef struct Opt Opt;
struct Opt {
	_Bool set;  /*If set is not bool, it can define the order of demos*/
	char  *sht, *lng; 
	char  *description;
	_Bool arg;
	Value v;
	//union v { int32_t n; char *s; char c; void *v; } value;
	void  (*callback)(char **av, Value *v);
	_Bool sentinel;
};

/*Define a simpler way to check set options.*/
#define opt(sht,lng) \
	(sht && strcmp(*argv, sht) == 0) || (lng && strcmp(*argv, lng) == 0) 

/*Check that option is set and cause the screen to refresh via 'co'*/
#define optset(longName) \
	((strcmp(eval->lng, longName) == 0) && (co = eval->set))

/*This is here in lieu of 'lite.c'.  This problem is solved within that library*/
#define eval_opts() \
	if (argc < 2) \
		usage(opts, 0); \
	while (*argv) { \
		Opt *o=opts; \
		while (!o->sentinel) { \
			if ((o->sht && strcmp(*argv, o->sht) == 0) || (o->lng && strcmp(*argv, o->lng) == 0)) { \
				o->set=1; \
				if (o->callback) \
					o->callback(argv, &o->v);  \
			} \
			o++; \
		} \
		argv++; \
	} \
	Opt *all=opts;

#define dump_opts() \
	while (!all->sentinel) { \
		fprintf(stderr,"%20s: %s\n", all->lng, all->set ? "true" : "false"); \
		all++; \
	}

/*...*/
void usage (Opt *opts, int status);
void opt_set_filename (char **av, Value *v);
void opt_set_dir (char **av, Value *v);
void opt_set_fps (char **av, Value *v);
void opt_set_switch (char **av, Value *v);
