

.PHONY:all docker_image docker_image_clean  docker_container docker_container_clean  deploy

all:deploy

docker_image:
	git submodule update --init --recursive
	if [ -f Dockerfile ]; then docker build -t syscall.online .; else true; fi

docker_image_clean:docker_container_clean
	if [ -f Dockerfile ]; then docker image rm syscall.online; else true; fi

docker_container: docker_image
	if [ -f docker-compose.yml ]; then COMPOSE_PROJECT_NAME=syscall_online  docker-compose down && COMPOSE_PROJECT_NAME=syscall_online docker-compose up -d; else true; fi;

docker_container_clean:
	if [ -f docker-compose.yml ]; then COMPOSE_PROJECT_NAME=syscall_online  docker-compose down; else true; fi;

deploy:docker_container

clean:docker_image_clean

