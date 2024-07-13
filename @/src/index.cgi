#!/bin/sh

# 发送301重定向
cat << EOF
Status: 301
Location: static/


EOF
