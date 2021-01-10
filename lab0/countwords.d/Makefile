# $Id: Makefile,v 1.1 2019-12-17 14:53:52-08 - - $


GARN    = -Wall -Wextra -Wpedantic -Wshadow -Wold-style-cast
GOPTS   = ${GWARN} -fdiagnostics-color=never
GPP     = g++ -std=gnu++2a -g -O0 ${GOPTS}
GRIND   = valgrind --leak-check=full --show-reachable=yes
NODEPS  = ${filter ci clean spotless tar, ${MAKECMDGOALS}}
MKTAR   = gtar --create --verbose --gzip

H_FILES =
C_FILES = countwords.cpp
OBJECTS = ${C_FILES:.cpp=.o}
EXECBIN = countwords
SOURCES = ${H_FILES} ${C_FILES} Makefile


all : ${EXECBIN}

${EXECBIN} : ${OBJECTS}
	${GPP} -o $@ $^

%.o : %.cpp
	- checksource $<
	${GPP} -c $<

ci : ${SOURCES}
	- checksource $^
	cid -is $^

clean :
	- rm --force ${OBJECTS} test.log test.out test.err

lis : ${SOURCES} Makefile.deps
	mkpspdf Listing.ps $^

spotless : clean
	- rm --force ${EXECBIN} Listing.{ps,pdf} Makefile.deps

tar : ${SOURCES}
	${MAKE} --no-print-directory spotless
	( DIRNAME=$$(basename $$(pwd)) \
	; cd .. \
	; ${MKTAR} --exclude=RCS --file=countwords.tar.gz $$DIRNAME \
	)

test : ${EXECBIN}
	${GRIND} --log-file=test.log \
	         ${EXECBIN} ${SOURCES} 1>test.out 2>test.err

Makefile.deps :
	${GPP} -MM ${C_FILES} >Makefile.deps
	
submit : $(EXECBIN) $(C_FILES) $(H_FILES)
	submit cse111-wm.w21 lab0 $(EXECBIN) $(C_FILES) $(H_FILES) Makefile README

ifeq (${NODEPS}, )
include Makefile.deps
endif

