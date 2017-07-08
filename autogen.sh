if type "gtkdocize" > /dev/null; then
	gtkdocize
fi
autoreconf -i
./configure
