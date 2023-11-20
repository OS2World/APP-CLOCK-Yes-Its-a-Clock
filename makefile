# nmake makefile
#
# Tools used:
#  Compile::Watcom Resource Compiler
#  Compile::GNU C
#  Make: GNU make
all : clock.exe

clock.exe : clock.obj clock.def clock.res
	gcc -Zomf clock.obj clock.def -o clock.exe
	wrc clock.res

clock.obj : clock.c clock.h
	gcc -Wall -Zomf -c -O2 clock.c -o clock.obj
	
clock.res : clock.rc clock.ico
	wrc -r clock.rc

clean :
	rm -rf *exe *res *obj
