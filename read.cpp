#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <sstream>
#include <cerrno>
#include <list>

#define BUFFER_SIZE         100000
#define STR_SIZE             150

using namespace std;

// for jpg use mclement.us/jpg.c
//compare filesize of jpg to 51793


bool compare_nocase (const std::string& first, const std::string& second)
{
  unsigned int i=0;
  while ( (i<first.length()) && (i<second.length()) )
  {
    if (tolower(first[i])<tolower(second[i])) return true;
    else if (tolower(first[i])>tolower(second[i])) return false;
    ++i;
  }
  return ( first.length() < second.length() );
}

void return_file(int hSocket, char * fullpath, struct stat& filestat, char * pBuffer)
{
	char *dot = strrchr(fullpath, '.');
	if (dot && !strcmp(dot, ".html")){
		FILE *fp = fopen(fullpath, "r");
		char *buffer = (char *)malloc(filestat.st_size + 1);
		fread(buffer, filestat.st_size, 1, fp);
		sprintf(pBuffer, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%ld\r\n\r\n", filestat.st_size);
		write(hSocket,pBuffer, strlen(pBuffer));
		write(hSocket,buffer, strlen(buffer));
		free(buffer);
		fclose(fp);
	}
	else if (dot && !strcmp(dot, ".txt")){
		FILE *fp = fopen(fullpath, "r");
		char *buffer = (char *)malloc(filestat.st_size + 1);
		fread(buffer, filestat.st_size, 1, fp);
		sprintf(pBuffer, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length:%ld\r\n\r\n", filestat.st_size);
		write(hSocket,pBuffer, strlen(pBuffer));
		write(hSocket,buffer, strlen(buffer));
		free(buffer);
		fclose(fp);	
	}
	else if (dot && !strcmp(dot, ".jpg")){
		sprintf(pBuffer,"HTTP/1.1 200 OK\r\nContent-Type: image/jpg\r\nContent-Length: 51793\r\n\r\n");
		write(hSocket,pBuffer, strlen(pBuffer));
		FILE *fp = fopen(fullpath,"r");
		char *buffer = (char *)malloc(filestat.st_size + 1);
		fread(buffer, filestat.st_size, 1,fp);
		write(hSocket,buffer,filestat.st_size);
		free(buffer);
		fclose(fp);
	}
	else if (dot && !strcmp(dot, ".gif")){
		sprintf(pBuffer,"HTTP/1.1 200 OK\r\nContent-Type: image/jpg\r\nContent-Length: 51793\r\n\r\n");
		write(hSocket,pBuffer, strlen(pBuffer));
		FILE *fp = fopen(fullpath,"r");
		char *buffer = (char *)malloc(filestat.st_size + 1);
		fread(buffer, filestat.st_size, 1,fp);
		write(hSocket,buffer,filestat.st_size);
		free(buffer);
		fclose(fp);
	}
	else{
		FILE *fp = fopen(fullpath, "r");
		char *buffer = (char *)malloc(filestat.st_size + 1);
		fread(buffer, filestat.st_size, 1, fp);
		sprintf(pBuffer, "HTTP/1.1 200 OK\r\nContent-Length:%ld\r\n\r\n", filestat.st_size);
		write(hSocket,pBuffer, strlen(pBuffer));
		write(hSocket,buffer, strlen(buffer));
		free(buffer);
		fclose(fp);
	}
}

void respond(int hSocket, char * fullpath, char * fname)
{
    DIR *dirp;
    struct dirent *dp;
	struct stat filestat;
	char pBuffer[BUFFER_SIZE];
	memset(pBuffer,0,sizeof(pBuffer));


	try {
		if(stat(fullpath, &filestat)) {
			sprintf(pBuffer, "HTTP/1.1 404 Not Found\r\nContent-Type:text/html\r\n\r\n<html><p>404 not found</p></html>");
			write(hSocket,pBuffer,strlen(pBuffer));
		}
		else if(S_ISREG(filestat.st_mode)) {
			return_file(hSocket, fullpath, filestat, pBuffer);
		}
		else if(S_ISDIR(filestat.st_mode)) {
			cout << fullpath << " is a directory \n";
			dirp = opendir(fullpath);
			list<char*> dir;
			bool has_slash = false;
			if (fullpath[strlen(fullpath)-1] == '/')
				has_slash = true;
			bool has_index = false;
			while ((dp = readdir(dirp)) != NULL){
				dir.push_back(dp->d_name);
				if (strcmp(dp->d_name, "index.html") == 0)
					has_index = true;
			}
			if (has_index){
				cout << fullpath << " has an index\n";
				char index_dir[STR_SIZE];
				if (has_slash)
					sprintf(index_dir, "%s%s", fullpath, "index.html");
				else
					sprintf(index_dir, "%s/%s", fullpath, "index.html");
				stat(index_dir, &filestat);
				return_file(hSocket, index_dir, filestat, pBuffer);
			}
			else {
				dir.sort(compare_nocase);
				stringstream trace;
				trace << "<ul>";
				list<char*>::iterator it;
				for (it=dir.begin(); it!=dir.end(); ++it)
					if (has_slash)
						trace << "<li><a href=\"" << fname << *it << "\">" << *it << "</a></li>";
					else
						trace << "<li><a href=\"" << fname << "/" << *it << "\">" << *it << "</a></li>";
				trace << "</ul>";
				(void)closedir(dirp);
				sprintf(pBuffer, "HTTP/1.1 200 OK\r\nContent-Type:text/html\r\n\r\n<html>%s</html>", trace.str().c_str());
				write(hSocket,pBuffer,strlen(pBuffer));
			}
		} 
	} catch (exception& e){
		memset(pBuffer,0,sizeof(pBuffer));
		sprintf(pBuffer, "HTTP/1.1 500 Internal Server Error\r\nContent-Type:text/html\r\n\r\n<html><p>500 Internal Server Error</p></html>");
		write(hSocket,pBuffer,strlen(pBuffer));
	}
}

