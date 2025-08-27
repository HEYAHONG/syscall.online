FROM debian:latest
MAINTAINER HEYAHONG 2229388563@qq.com
WORKDIR /var/www/html
ENV DEBIAN_FRONTEND noninteractive
ENV TZ=Asia/Shanghai
RUN find /etc/apt -type f  | xargs sed -i "s/deb.debian.org/mirrors.ustc.edu.cn/g"  && apt-get update && apt-get full-upgrade -yyy && apt-get install -yyy build-essential cmake gcc g++  make s6
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
COPY src/ /usr/src/
RUN if [ -f /usr/src/Makefile ]; then make -C /usr/src; else true;fi
VOLUME ["/usr/src"]
# 暴露端口
EXPOSE 58080
CMD [ "/usr/bin/s6-svscan","/etc/s6-service" ]
