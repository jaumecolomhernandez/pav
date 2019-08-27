CFLAGS=-I. -g

vad : main_vad.o vad.o pav_analysis.o
	gcc main_vad.o vad.o pav_analysis.o -lm -lsndfile -o vad

main_vad.o : main_vad.c vad.h
vad.o : vad.c vad.h pav_analysis.h
pav_analysis.o : pav_analysis.c pav_analysis.h