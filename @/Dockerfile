FROM debian:latest
MAINTAINER HEYAHONG 2229388563@qq.com
WORKDIR /var/www/html
ENV DEBIAN_FRONTEND noninteractive
ENV TZ=Asia/Shanghai
RUN find /etc/apt -type f  | xargs sed -i "s/deb.debian.org/mirrors.ustc.edu.cn/g"  && apt-get update && apt-get full-upgrade -yyy && apt-get install -yyy build-essential cmake gcc g++ dbus libdbus-1-dev libcgicc-dev systemd nginx fcgiwrap make libjsoncpp-dev libboost-all-dev emscripten libwebsockets-dev
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
RUN mkdir -p /var/www/html/static && mkdir /var/www/html/api && systemctl enable fcgiwrap && systemctl mask getty@tty1 getty@tty2 getty@tty3 getty@tty4  getty@tty5 getty@tty6 getty@tty7 getty@tty8
COPY src/ /usr/src/
RUN if [ -f /usr/src/Makefile ]; then make -C /usr/src; else true;fi
COPY nginx_site.conf  /etc/nginx/sites-enabled/default
VOLUME ["/var/www/html/static/","/usr/src"]
# 暴露端口
EXPOSE 80
# 按照正常系统进行启动
CMD [ "/usr/lib/systemd/systemd" ]
