all: server
server: server.cpp
	g++ -o server server.cpp -pthread
git_local:
	git add .
	git commit -m "dev"
	git push origin master
git_remote: server.cpp
	git stash save
	git pull
	git stash drop
