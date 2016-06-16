# vis - <description>
include config.mk

NAME = vis
SRC = canvas.c opt.c main.c
OBJ = ${SRC:.c=.o}

DBGCHK = gdb 
DBGFLAGS = -ex run --args

LEAKCHK = valgrind
LEAKFLAGS = --leak-check=full 

FLAGS = -p dte.wav -l -f 60 

IGNORE = $(ALIAS) archive/* bin/* tools/* vendor/* wk/*
ARCHIVEDIR = ..
ARCHIVEFMT = gz
ARCHIVEFILE = $(NAME).`date +%F`.`date +%H.%M.%S`.tar.${ARCHIVEFMT}

$(NAME): ${OBJ}
	@echo CC -o $@ ${OBJ} ${CFLAGS}
	@${CC} -o $@ ${OBJ} ${CFLAGS}

embed: DFLAGS = -DENABLE_VERSION_BANNER -DVERSION="$(VERSION)" -DPLAY_EMBEDDED
embed: $(NAME)

debug:
	$(DBGCHK) $(DBGFLAGS) ./$(NAME) $(FLAGS)

leak:
	$(LEAKCHK) $(LEAKFLAGS) ./$(NAME) $(FLAGS)

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

clean:
	@echo Cleaning
	@rm -f *.o *.so *.dll

veryclean:
	@echo Cleaning up version control files...
	-find -type f -name "*.swo" | xargs rm
	-find -type f -name "*.swp" | xargs rm
	-find -type f -name "*.swn" | xargs rm
	-find -type f -name "*.swl" | xargs rm

permissions:
	@find | grep -v './tools' | grep -v './examples' | grep -v './.git' | sed '1d' | xargs stat -c 'chmod %a %n' > PERMISSIONS

restore-permissions:
	chmod 744 PERMISSIONS
	./PERMISSIONS
	chmod 644 PERMISSIONS

backup:
	@echo tar chzf $(ARCHIVEDIR)/${ARCHIVEFILE} --exclude-backups \
		`echo $(IGNORE) | sed '{ s/^/--exclude=/; s/ / --exclude=/g; }'` ./*
	@tar chzf $(ARCHIVEDIR)/${ARCHIVEFILE} --exclude-backups \
		`echo $(IGNORE) | sed '{ s/^/--exclude=/; s/ / --exclude=/g; }'` ./*

archive: ARCHIVEDIR = archive
archive: backup

.PHONY: all options clean dist install uninstall permissions archive
