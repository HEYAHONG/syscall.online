
.PHONY:all index.cgi

all:install

index.cgi:
	g++ --std=c++17 index.cpp -I/usr/include/jsoncpp -ljsoncpp -lcgicc -o index.cgi

install:index.cgi
	#安装index.cgi
	install -d /var/www/html/api/ip/
	install index.cgi /var/www/html/api/ip/
