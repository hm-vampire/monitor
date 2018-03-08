debug:
	mkdir obj -p
	gcc -c -o ./obj/monitor.o monitor.c -g
	cd httpd; make
	mv httpd/*.o obj/
	gcc -o monitor obj/*.o -lpthread
release:
clean:
	rm -rf ./obj monitor
