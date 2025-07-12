#!/bin/sh

cd docker
docker-compose down

cd ..
sudo rm -rf drive
mkdir -p drive/logs/stdout
mkdir -p drive/logs/stderr

cd docker
# docker-compose build
# docker compose up
docker network prune
docker-compose up --scale server=3 --scale client=6
