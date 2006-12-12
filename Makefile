all:
	gcc -lncurses pacman.c     -o pacman
	gcc -lncurses pacmanedit.c -o pacmanedit

install:	all
	cp pacman /usr/local/bin
	cp pacmanedit /usr/local/bin
	mkdir -p /usr/local/share/pacman
	cp -fR Levels/ /usr/local/share/pacman/
	chown root:games /usr/local/bin/pacman
	chown root:games /usr/local/share/pacman -R
	chmod 750 /usr/local/bin/pacman
	chmod 750 /usr/local/bin/pacmanedit
	chmod 750 /usr/local/share/pacman/ -R

uninstall:
	rm -f /usr/local/bin/pacman
	rm -f /usr/local/bin/pacmanedit
	rm -f /usr/local/share/pacman/Levels/level0[1-9].dat
	rm -f /usr/local/share/pacman/Levels/README
	rm -f /usr/local/share/pacman/Levels/template.dat
	if [ -e /usr/local/share/pacman/Levels/ ] ; then rmdir /usr/local/share/pacman/Levels/ ; fi
	if [ -e /usr/local/share/pacman/ ] ; then rmdir /usr/local/share/pacman/ ; fi

clean:	uninstall
	rm -f pacman
	rm -f pacmanedit
