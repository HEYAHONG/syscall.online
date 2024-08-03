FROM debian:latest
MAINTAINER HEYAHONG 2229388563@qq.com
WORKDIR /var/www/html
ENV DEBIAN_FRONTEND noninteractive
RUN find /etc/apt -type f  | xargs sed -i "s/deb.debian.org/mirrors.ustc.edu.cn/g"  && apt-get update && apt-get full-upgrade -yyy && apt-get install -yyy build-essential cmake gcc g++ dbus libdbus-1-dev libcgicc-dev systemd nginx fcgiwrap make libjsoncpp-dev libboost-all-dev emscripten
RUN mkdir -p /var/www/html/static && mkdir /var/www/html/api && systemctl enable fcgiwrap
COPY src/ /usr/src/
RUN if [ -f /usr/src/Makefile ]; then make -C /usr/src; else true;fi
COPY nginx_site.conf  /etc/nginx/sites-enabled/default
VOLUME ["/var/www/html/static/","/usr/src"]
# 暴露端口
EXPOSE 80
# 按照正常系统进行启动
CMD [ "/usr/bin/systemd" ]
