all: aos

aos: aos.c atomik_core_sim.c
	gcc aos.c atomik_core_sim.c -o aos

clean:
	rm -f aos aos.exe
