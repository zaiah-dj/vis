/*main.c*/
#include <stdio.h>
#include <math.h>
#include <SDL/SDL.h>
#include "canvas.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include "opt.h"

/*Other settings*/
#define NAME "vis"
#define WHEIGHT 700
#define WWIDTH 1024
#define BPP    32 
#define VISUAL(name,a,b) 
#define AUTHOR "Antonio R. Collins II"

/*Shorthand*/
//Increment the current position of audio buffer by samples received and some other incrementor (i)
#define cp(i) *(v->ap + v->sp + i) 

/*Globals*/
const int8_t DEFAULT_FRAME_RATE = 20;
static int j=0;
static int64_t current_tv_sec=0;
char *dirname=NULL;
int buffsz = 64;
double samprt = 44100;
static FILE *wo=NULL;
int interval=0;
#if 0
static uint8_t *audio_pos;
static uint32_t audio_len;
#endif

/*Simple error handling*/
#define err(stat, ...) \
	while (1) { fprintf(stderr, "ERR: "); fprintf(stderr, __VA_ARGS__); return stat; } 

/*Simple logging*/
#ifdef ENABLE_LOG
 #define log(f, ...) fprintf((f == NULL) ? stderr : f, __VA_ARGS__);
#else
#endif

/*Version and details*/
#ifdef ENABLE_VERSION_BANNER
 #define print_version() \
	fprintf(stderr, "%s - v%.2f - compiled %s at %s\n", NAME, VERSION, __DATE__, __TIME__);
#else
 #define print_version()
#endif


/*Data structure for that userdata audio*/
typedef struct AudioAv AudioAv;
typedef struct AudioData AudioData;
struct AudioAv {
	void (*fp)(Surface *win, SDL_Event *ev, void *ad); 
	void *data;
};


/*Data structure to load and hold audio*/
typedef struct Av Av;
struct Av {
	/*Audio tracking*/
	uint8_t      *ap;   /*Buffer as modified (but updates slowly)*/
	uint32_t      al;	  /*Length as modified by audio_callback*/
	SDL_AudioSpec as;   
	uint32_t      sp;   /*Position within the current buffer of samples*/

	/*Time and frame tracking*/
	struct timeval tv;

	/*General audio info*/
	char          *filename;
	char          *dirname;

	/*Information passed by visual functions*/
	void         *opaque;
	_Bool         opqset;
	SDL_Event		  event;
};


/*Data structure to hold visual montage info*/
typedef struct Vv Vv;
struct Vv {
	_Bool       p_epilepsy;
	const char *name;
	char       *author;
	void      (*fp)(Surface *win, Av *av);
};


/*Some settings*/
static int32_t sampRate = 44100;	
static int re=100;

/*This function can be greatly modified later.*/
void print_audiofile (SDL_AudioSpec *as) {
	const char *sfmt = "%-30s %s\n";
	const char *nfmt = "%-30s %d\n";
	//fprintf(wo, sfmt, "Filename",  *as->userdata)->filename); 
	fprintf(stderr, nfmt, "Frequency", as->freq); 
	fprintf(stderr, nfmt, "Format"   , as->format); 
	fprintf(stderr, nfmt, "Channels" , as->channels); 
	fprintf(stderr, nfmt, "Silence"  , as->silence); 
	fprintf(stderr, nfmt, "Samples"  , as->samples); 
	fprintf(stderr, nfmt, "Size"     , as->size); 
#if 0
#endif
}


/*Print the contents of audio structure*/
void print_av (Av *av) {
	fprintf(stderr, "Audio pointer:    %p\n", (void *)av->ap);
	fprintf(stderr, "Audio length:     %d\n", av->al);
	fprintf(stderr, "Audio spec (SDL): %p\n", (void *)&av->as);
	print_audiofile (&av->as);
}



/*Log time difference.*/
#ifdef LOG_FPS
 #define diff(name) logdiff(name, __FILE__, __LINE__)
 static void logdiff(char *name, char *file, int line) {
 	j++;
 	struct timeval tv;
 	gettimeofday(&tv, NULL);
 	/*Write the times...*/
 	if (tv.tv_sec > current_tv_sec) {
 		current_tv_sec = tv.tv_sec;
 		fprintf(stderr, "[%d, %s, line: %d] time: %ld\n", j, !name ? file : name, line, (int64_t)tv.tv_sec);
 		j=0;
 	}
 }
#else
 #define diff(n)
#endif


/*Write video file*/
#define WRITEFFMPEG \
	"ffmpeg -an -f rawvideo -pix_fmt bgra -s %dx%d -r 60 -i - -aspect %d/%d -crf 2 -preset fast -y file1.avi"
#if 0
void write_video (Surface *win) {
	char buf[512];
	//sprintf(buf, WRITEFFMPEG, win->w, win->h, win->w, win->h);
	sprintf(buf, WRITEFFMPEG, 640, 480, win->w, win->h);
	static FILE *fh=NULL;
	if (!fh) 
		fh = /*fopen("file1.bin", "wb")*/popen(buf, "w");
	fwrite(win->win->pixels, win->w*win->h, 4, fh);
	fflush(fh);
}
#else
void write_video (Surface *win) {
	static int i;
	char filename[512];
	dirname = !dirname ? "." : dirname;
	sprintf(filename, "%s/img-%.5d.bmp", dirname, ++i);
	SDL_SaveBMP(win->win, filename);
}
#endif


/*Play that audio for real*/
void audio_callback (void *userdata, uint8_t *stream, int len) {
	Av *av=(Av *)userdata;
	//print_av(av);
	if (av->al == 0)
		return;
	len = (len > (int)av->al ? (int)av->al : len ); // never seen this...
	SDL_MixAudio(stream, av->ap, len, SDL_MIX_MAXVOLUME);
	av->ap += len;
	av->al -= len;
}


/*Dummy for testing audio playback.*/
void none (Surface *win, Av *v) { ; }


/*More squares*/
void more_squares (Surface *win, Av *v) {
	;
	/*Randomly choose whether you want a normal square or another one...*/
	/*Draw at least 10 squares at a time and just start moving stuff*/
/*Draw an outline*/
#if 0
	line(win, x, y, x, y+len, color, 0xff); 
	line(win, x, y, x+len, y, color, 0xff); 
	line(win, x+len, y, x+len, y+len, color, 0xff); 
	line(win, x, y+len, x+len, y+len, color, 0xff); 
#endif
}



/*yeah...*/
void bars (Surface *win, Av *v) 
{
	/*The basics to get me started*/
	const int squares=100;
	int space=40;
	const int len = space;
	const int hgt = 90;
	int y = win->h;
	int32_t lim = win->w - space; 

	/*Clear the screen*/ 
	fill_display(win, 0x000000);

	/*Draw a square at the top row and exit.*/
	for (int x=10; x < lim; x += space) {
		int32_t c=urwr(0,0xffffff);
		for (int j=0; j<len; j++) {
			int y=win->h; line(win, x+j, y-1, x+j, (y-1) - (cp(x) << 1), c, 0xff); 
			//int y=0; line(win, x+j, y, x+j, (y - cp(x)) << 1, c, 0xff);  /*Draw from the top*/
		}
	}

	/*Update*/
	SDL_UpdateRect(win->win, 0, 0, 0, 0);
}





/*radial*/
void radial (Surface *win, Av *v) {
	int32_t diameter=2;
	//static int p[2][]={{1,1},{
	//int32_t x=urwr(0, win->w);
	//int32_t y=urwr(0, win->w);
	Pt o={.y=win->h/2, .x=win->w/2};
	//Pt o={.y=y, .x=x};
	//a2 + b2 = c2
	int64_t lim = (int64_t)sqrt((win->w * win->w) + (win->h * win->h));
	while (diameter < lim) {
		int32_t c=urwr(0x222222, 0xffffff);
		for (int t=0;t<10;t++) {
			circle(win, o.x, o.y, diameter/2, c, 0xff);
			diameter += 2;
		}
	}
}



/*Reading from memory is cool...*/
void read_from_mem () {
	/*Probably not a bad idea to check that the wavdata is evenly dividable before you start*/

	/*Increment two pointers at a time.*/
	;
}


/*Pause with audio*/
void pause_with_audio() {
	//SDL_PauseAudio(1);
	//getchar();
	//SDL_PauseAudio(0);
}



/*Simple waveform*/
void drawWavform (Surface *win, Av *v) {
	int r=1;
	int pp=sampRate/win->w;
	fill_display(win, 0x000000);
	for (int i=0; i < win->w; i += 1, r += pp) {
		line(win, i, ((win->h/2) - (cp(r)/2)), i, ((win->h/2) + (cp(r)/2)), 0xffffff, 0xff); 
	}
	SDL_UpdateRect(win->win, 0, 0, 0, 0);
}





/*Draw some pixels*/
void drawPoints (Surface *win, Av *v) {
	int i=0; 
	uint32_t clr = urwr(0, 0xffffff);  /*Random is EXTREMELY slow...*/
	for (int x=0; x<win->w; x++, i += interval )
		plot(win, x, v->ap[i] << 2, clr, 0xff);
}




/*Useless polygon*/
void uselessPolygon (Surface *win, Av *v) {
	/*Define home, and iterate through a given set of points*/
	/*44100 / second?*/
	/*depending on framerate (which should be in Av *v))*/
	/*move through a set of points*/
	int m=sampRate/10;

	/*To make a "useless polygon", you'll want to plot (or save) the points and create 
	some other constant to define the quadrant*/
	/*For example, there are 4410 endpoints in our polygon when m = (44100 / 10)*/
	/*Of those 4410, we'll want to give at least 1100 some other quadrant OR
		let each point move along the points of a circle*/
	/*What is the equation of a circle?*/
	/*(x - j)^2 + (y - k)^2 = r^2*/
	/*where (j,k) is the center and r = radius*/
	for (int i=0; i<m; i++) {
		;

	}
		
}



void drawLines (Surface *win, Av *v) {
	int i=0; 
	//fill_display(win, 0x000000);
	uint32_t clr = urwr(0, 0xffffff);  /*Random is EXTREMELY slow...*/
	for (int x=0; x<win->w; x++, i += interval )
		line(win, x, win->h - 10, x, (win->h - cp(i)) >> 2, clr, 0xff);
	SDL_UpdateRect(win->win, 0, 0, 0, 0);
}







void drawRects (Surface *win, Av *v) {
	int i=0; 
	uint32_t clr = urwr(0, 0xffffff);  /*Random is EXTREMELY slow...*/
	for (int x=0; x<win->w; x++, i += interval ) {
		int32_t h=(clr >> 10);
		//SDL_Rect r={.x=(v->ap[i] << 2), .y=v->ap[i+1], .h=h, .w=30 };
		SDL_Rect r={.x=(cp(i) << 2), .y=cp(i+1), .h=h, .w=30 };
		SDL_FillRect(win->win, &r, (clr += (clr > 0xffffff) ? -clr : 0x333)); 
	}
}





void mills2 (Surface *win, Av *v) {
	/*Draw lines in a windmill pattern, and fade them*/
	//for (int x=0; x<10; x++)
	//	line(win, ((win->w/2)-(*(v->ap)/2)), ((win->h/2)-(*(v->ap)/2)), ((win->w/2)-(*(v->ap)/2)), ((win->h/2)-(*(v->ap)/2)), 0xffffff, 0xff);

	/*I need a queue for this....*/
	const int lines=400;
	const int spacing=1;
	typedef struct { Pt start, end; int32_t clr; } Ptpp;
	//typedef struct { Pt center; Ptpp pts[100]; } Info;
	typedef struct { Pt center; Ptpp *pts; } Info;
	Info *p=(!v->opqset) ? NULL : (Info *)v->opaque;

	/*Create the right structure*/
	if (!v->opqset && !v->opaque) {
		v->opqset = 1;
		#if 0
		if (!(p = malloc(sizeof(Info) + (sizeof(Ptpp) * lines))))
			return;
		#else
		if (!(p = malloc(sizeof(Info))))
			return;
		if (!(p->pts = malloc(sizeof(Ptpp) * lines))) {
			free(p);
			return;
		}
		memset(p->pts, 0, sizeof(Ptpp) * lines);
		#endif
		p->center.x=win->w/2; 
		p->center.y=win->h/2;
		v->opaque = p;

		/*Initialize this*/
		for (int i=0; i<lines; i++) {
			Ptpp *cur=&p->pts[i];	
			cur->start.x = 0;
			cur->start.y = 0;
			cur->end.x = 0;
			cur->end.y = 0;
		}
		return;
	}

	/*Draw simple horizontal lines*/
	int c=urwr(0x333333, 0xdddddd);
	int y=100;
	for (int i=0; i<lines; i++) {
		//int64_t lim = (int64_t)sqrt((win->w * win->w) + (win->h * win->h));
		//fprintf(stderr, "%d - ", cp(i)); //*(v->ap + v->sp + i)); 
		Ptpp *cur=&p->pts[i];	
		cur->start.x = p->center.x - (cp(i) / 2);
		cur->start.y = cur->end.y = y += spacing;
		cur->end.x = p->center.x + (cp(i) / 2);
		cur->clr = c;
		line(win, cur->start.x, cur->start.y, cur->end.x, cur->end.y, cur->clr, 0xff);

		#if 0
		if (i > 0) {
			Ptpp *prv=&p->pts[i - 1];	
			line(win, prv->start.x, prv->start.y, prv->end.x, prv->end.y, (prv->clr -= 0x111111), 0xff);
			// fprintf(stderr, "Dropping last color to: %d\n", prv->clr);
			// you could update only the areas that have changed...
		}
		#endif
	}
}


void rain (Surface *win, Av *v) {

}


void mills (Surface *win, Av *v) {
	/*Draw bunches of lines*/
	int x=urwr(0,win->w);
	//int c=urwr(0x333333, 0xdddddd);
	int c=0xffffff;
	for (int n=0;n<100;n++) {
		int x=urwr(0, win->w);
		line(win, x, cp(n), x, cp(n+2) << 2, urwr(0x111111, 0xffffff), 0xff);
	}
}


void drawCircs (Surface *win, Av *v) {
	/*Define data types*/
	typedef struct { Pt pos;int rad, clr, crad, set, nset, vset, hh, ww; } Cir;
	Cir *c=(!v->opqset) ? NULL : (Cir *)v->opaque;

	/*Run a "constructor"*/	
	if (!v->opqset && !v->opaque) {
		c=NULL;
		v->opqset = 1;
		if (!(c = malloc(sizeof(Cir))))
			return;
		c->pos.x = urwr(0, win->w);
		c->pos.y = urwr(0, win->h);
		c->clr   = urwr(0, 0xffffff);
		c->rad   = urwr(0, 300);
		v->opaque = c;
	}
	
	/*Define short names.*/	
	Pt  *p=&c->pos;
	int r=c->rad;
	int32_t *cc=&c->clr;
	*cc += (*cc > 0) ? -2048: 0xffffff;

	if (p->x < 700 && !c->nset)
		p->x++;
	else {
		p->x--;
		c->nset = (p->x > 300) ? 1 : 0;
	}

	if (p->y < 600 && !c->vset)
		p->y++;
	else {
		p->y--;
		c->vset = (p->y > 200) ? 1 : 0;
	}

	if (!c->set && c->crad < 100) {
		c->crad += 10;
		circle(win, p->x, p->y, c->crad, *cc, 0xff);
		c->set = (c->crad == 100) ? 1 : 0;
		return;
	}
	else {
		c->crad -= 10;
		circle(win, p->x, p->y, c->crad, *cc, 0xff);
		c->set = (c->crad == 0) ? 0 : 1;
		return;
	}
}


static int xp=0, xe=0; 
/*Transitions ought to be in a chain of some sort*/
void trans1 (Surface *win, Av *v) {
	/*Right now, just move Star Wars style over what's there...*/
	if (xe > win->w) 
		return;
	int xe = xp+10;
	for (int x=xp;x<xe;x++)
		line(win, x, 0, x, win->h, 0xaaaaaa, 0xff);
}




/*Function pointers to different visuals*/
Vv fp[] = {
#ifdef PLAY_AUDIO_ONLY
	{ 0, "dummy",   AUTHOR, none       },
#else
	{ 1, "ColorBath",   AUTHOR, drawLines  },
	{ 1, "rects",   AUTHOR, drawRects  },
	{ 1, "bars",    AUTHOR, bars       },
	{ 0, "points",  AUTHOR, drawPoints },
	{ 1, "radial",  AUTHOR, radial     },
	{ 0, "circles", AUTHOR, drawCircs  },
	{ 0, "Mills",   AUTHOR, mills      },
	{ 1, "Neon WaveForm", AUTHOR, mills2     },
	{ 0, "WavForm", AUTHOR, drawWavform},
#endif
};



/*Defaults*/
struct Settings {
	char    *filename;
	int8_t  fps;
	int16_t change;
	_Bool   loop;
	_Bool   no_epilepsy;
	int8_t  res;
}	settings = {
	.filename   = NULL,
	.fps        = 0,
	.change = 5,
	.loop       = 0,
	.no_epilepsy = 1,
	.res = 1,
};


struct Resolutions { char *name; int32_t w, h; } resolutions[] = {
	{ "320x240",    320, 240 },
	{ "640x480",    640, 480 },
	{ "800x600",    800, 600 },
	{ "1024x600",  1024, 600 },
	{ "1280x1000", 1280, 700 },
	{ NULL,           0,   0 }
};



void opt_set_resolution (char **av, Value *v) {
	/*...*/

	int c=0;
	av++;
	struct Resolutions *r=resolutions;
	while (r->name) {
		if (strcmp(*av, r->name)) {
			v->n = c;
			return; 
		}
		r++; c++;
	}

	/*We must not have chosen a supported resolution*/
	fprintf(stderr, "ERR: Resolution %s not supported.\n", *av);
	v->n=0; /*Select a default*/
}


/*Options array*/
Opt opts[] = {
	#if 0 /*Consider leaving these in a config.h.*/
	{ 0, "-b", "--buffer-size",  "Set an alternate buffer size."             },
	{ 0,"-r", "--render",   "Render a session to file.", 1, .callback=opt_set_filename },
	{ 0,"-d", "--dir",      "Save video files to <dir>.", 1, .callback=opt_set_dir  },
	{ 0,NULL, "--log",      "Log files somewhere.", 1, .callback=opt_set_filename },
	#endif
	{ 0,"-l", "--loop",     "Enable looping.", },
	{ 0,NULL, "--describe", "Describe an audio stream."                 },
	{ 0,"-p", "--play",     "Choose a file to play.", 1, .callback=opt_set_filename },
	{ 0,"-s", "--switch",   "Switch animations every <arg> seconds.", 1, .callback=opt_set_switch   },
	{ 0,"-f", "--framerate","Use <arg> fps when rendering.", 1, .callback=opt_set_fps },
	{ 0,"-n", "--resolution","Use <arg> resolution.", 1, .callback=opt_set_resolution },
	{ 0,"-e", "--enable-flashes","Use <arg> resolution." },
	{ .sentinel=1 }
};


/*main*/
int main (int argc, char *argv[])
{
	/*Print banner and process options*/
	print_version();
	eval_opts();
#ifdef DUMP_OPTIONS
	dump_opts();
#endif

	/*Then go through and set each.*/
	Opt *eval=opts;
	while (!eval->sentinel) {
		/*Option evaluation*/
		int co=0;
		if      (optset("--play"))
			settings.filename = eval->v.s;
		else if (optset("--framerate"))
			settings.fps      = eval->v.n;
		else if (optset("--loop"))
			settings.loop     = 1; 
		else if (optset("--switch"))
			settings.change = eval->v.n;
		else if (optset("--resolution"))
			settings.res = eval->v.n;
		else if (optset("--enable-flashes"))
			settings.no_epilepsy = 0;
		else if (optset("--log")) {
			fprintf(stderr, "Logging output to file: %s\n", eval->v.s);
			wo=fopen(eval->v.s, "w+");
			if (!wo) {
				fprintf(stderr, "Failed to open log file.");
				return 0;
			}
		}
	#if 0
		else if (optset("--dir")) {
			dirname = eval->v.s;
			struct stat buf;
			if (stat(dirname, &buf) == -1) 
				if (mkdir(dirname, 0755) == -1) {
					fprintf(stderr, "Failed to create non-existent directory: %s\n", dirname);
				}
		}
	#endif
		eval++;
	}


#if 0
	/*Please make Engine datatype*/
#else
	/*Display/Main window setup*/
	if (SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0)
		err(0, "Couldn't initialize audio abstraction layer.\n")
	Surface Win = { .h = resolutions[settings.res].h, .w = resolutions[settings.res].w, .bpp = BPP}; 
	Win.win = SDL_SetVideoMode(Win.w, Win.h, Win.bpp, SDL_HWSURFACE | SDL_DOUBLEBUF);
	signal(SIGINT, SIG_DFL);
	SDL_Event event = {0};
#endif

	/*Set defaults and others.*/
	int16_t delay_rate = (1000/(!settings.fps ? DEFAULT_FRAME_RATE : settings.fps));
	interval = ((20000-20)/Win.w);

	/*Set the parameters of userdata*/
	Av mv={.ap /*audio_pos*/ = NULL, .al /*audio_len*/= 0, .event=event};

	/*Load and initialize the audio*/
#ifdef PLAY_EMBEDDED
	uint8_t wav[] = {
		#include "audio.c"
	};
	//mv.as = &wav; // sample rate, etc.
	mv.al = sizeof(wav)/sizeof(uint8_t);
	mv.ap = wav;  /*This is a guess of where the audio would start*/
	mv.as.freq = 44100;
	mv.as.channels = 2;
	mv.as.format = AUDIO_S16;
	//mv.as.samples= 2048;
	//mv.as.size = 8192;
	fprintf(stderr, "PLAY_EMBEDDED is enabled in build flags. Please disable it to play other files.\n");
#else
	/*Die if there are no files in the buffer (for now)*/
	if (!settings.filename) {
		err(0, "No audio file supplied! (Try the --play flag to select one.)\n");
	}

	if (!SDL_LoadWAV(settings.filename, &(mv.as), &(mv.ap), &(mv.al))) 
		err(0, "Failed to load audio file: %s\n", settings.filename);
#endif

	/*Populate unknown data and describe the audio stream.*/
	mv.as.callback = audio_callback;
	mv.as.userdata = &mv;

	/*Open a device and play...*/
	if (SDL_OpenAudio(&(mv.as), NULL) < 0)
		err(0, "Failed to open audio stream: %s\n", SDL_GetError());

	/*Show the dump*/
	print_av(&mv);	

	/*Play and control playback.*/
	SDL_PauseAudio(0);
	int cts=0, m=0, gl = mv.al;
	while (mv.al) {
		/*Control framerate and show diagnostics*/
		SDL_Delay(delay_rate);
		diff(NULL);

		/*Draw frame and update display*/
		fp[m].fp(&Win, &mv);	
		SDL_UpdateRect(Win.win, 0, 0, 0, 0);
		gettimeofday(&mv.tv, NULL);

		/*Easily increase the number of samples*/
		mv.sp += (mv.sp > 44100) ? -mv.sp : 100;

		/*Shift to next animation*/
		if (mv.tv.tv_sec > cts) {
			cts = mv.tv.tv_sec + settings.change;
			
			/*Skip the ones that might cause epileptic seizures*/
			if (!settings.no_epilepsy)
				m += (m == ((sizeof(fp)/sizeof(Vv)) - 1) ? -m : 1);
			else {
				do {
					m += (m == ((sizeof(fp)/sizeof(Vv)) - 1) ? -m : 1);
				#ifdef PRINT_ANIMATION_INFO	
					/*Print animation info to the console.*/
					if (fp[m].p_epilepsy)
						fprintf(stdout, "Skipping animation: %s by %s\n.", fp[m].name, fp[m].author);
				#endif
				} while (fp[m].p_epilepsy);
			}

			/*Call transition*/
			SDL_Rect r={0,0,Win.w,Win.h};
			SDL_FillRect(Win.win, &r, 0x000000);
			SDL_UpdateRect(Win.win, 0, 0, 0, 0);
		
		#ifdef PRINT_ANIMATION_INFO	
			/*Print animation info to the console.*/
			fprintf(stdout, "Loaded animation: %s by %s\n.", fp[m].name, fp[m].author);
		#endif

			/*Free any data that was there previously*/	
			if (mv.opqset) {
				free(mv.opaque);
				mv.opaque=NULL;
				mv.opqset=0;
			}

			/*Set this to prevent getting data from the wrong frame*/
			mv.sp = 0;
		}

		/*If we are about to reach the end, loop?*/
		if (settings.loop && (mv.al < 5))	{
			mv.ap -= (gl - re);
			mv.al = (gl - re); 
		}
	}

	/*Always destroy the audio buffer*/
	#if 0
	SDL_FreeWAV(mv.ap);
	#else
	SDL_CloseAudio();
	#endif

	/*Also destroy the SDL instance*/
	SDL_Quit();
	return 0;
}
