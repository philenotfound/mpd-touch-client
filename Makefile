all:
	gcc `pkg-config --cflags gtk+-3.0` mpd_client.c `pkg-config --libs gtk+-3.0` -lmpdclient
