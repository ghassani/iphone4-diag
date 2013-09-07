#include <iostream>
#include "server.cpp"
#include "util.h"
#include <cstdlib>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "boost/thread.hpp"
#include "util.h"
#include "pthread.h"
#include "main.h"
#include <mach-o/dyld.h>

using namespace std;
using boost::asio::ip::tcp;
using namespace boost;

#define DYLD_INTERPOSE(_replacment,_replacee) \
__attribute__((used)) static struct{ const void* replacment; const void* replacee; } _interpose_##_replacee \
__attribute__ ((section ("__DATA,__interpose"))) = { (const void*)(unsigned long)&_replacment, (const void*)(unsigned long)&_replacee }

DYLD_INTERPOSE(_open, open);
DYLD_INTERPOSE(_read, read);
DYLD_INTERPOSE(_write, write);
DYLD_INTERPOSE(_close, close);
//DYLD_INTERPOSE(_fread, fread);

FILE* logs[100];
char fds[100][256];

std::string log_targets[] = {
    "/dev/mux.spi-baseband",
    "/dev/dlci.spi-baseband.extra_c",
    "/dev/tty.debug",
    "/dev/dlci.spi-baseband.qmux1_c",
    "/dev/dlci.spi-baseband.qmux1_d",
    "/dev/dlci.spi-baseband.qmux2_c",
    "/dev/dlci.spi-baseband.qmux2_d",
    //"/dev/uart.debug",
};

const char* log_path = "/var/log/commcenter";
const char* target_device = "/dev/dlci.spi-baseband.qmux2_c";
int target_device_fd = 0;
bool target_opened = false;
bool server_started = false;
pthread_mutex_t mutex_lock;
FILE* open_close_log;
const char* program_name;

/**
 * inject_init(void)
 */
void init(void)
{
    
    printf("Successfully Injected\n");
    
    /*const char* imageName = _dyld_get_image_name(0);
    if (NULL == strcasestr(imageName, "CommCenter")) {
        printf("Injection library loaded in %s, idle", imageName);
        return;
    }*/
    
    std::string _program_full_name = _dyld_get_image_name(0);
    std::string _program_name;
    program_name = _program_full_name.substr(_program_full_name.find_last_of("/")+1,_program_full_name.length()).c_str();
    char _log_path[256];
    sprintf(_log_path,"%s/%s-open_close.log",log_path,program_name);
    open_close_log = fopen(_log_path,"a+");
    printf("Open Close Log %s\n",_log_path);
    
    pthread_mutex_init(&mutex_lock, NULL);
}


/**
 * start_server()
 */
void start_server()
{
    printf("Starting Server on port TCP:8885 with awaiting device %s\n", target_device);
    boost::asio::io_service io_service;
    try{
        server tcp_server(io_service, 8885, target_device_fd);
        io_service.run();
    } catch(boost::system::system_error& e) {
        printf("Exception: %s\n", e.what());
    }
}

/**
 * _read - Custom Read Function
 */
int _read(int  fd, void  *buffer, int nbyte)
{
    
    unsigned int tid = (unsigned int)pthread_self();
    int ret = 0;
    if(fd == target_device_fd && target_device_fd != 0){
        pthread_mutex_lock(&mutex_lock);
        printf("Read Thread Locked To Read Up To %i Bytes By Thread %i\n", nbyte, tid);
    }

    ret = read(fd,buffer,nbyte);
    
    if(strlen(fds[fd]) != 0){
        if(logs[fd] != 0){
            fprintf(logs[fd], "\n\nRead from %s BYTES: %i RET: %i THREAD: %i\n",fds[fd], nbyte, ret, tid);
            hexdump_log(logs[fd], (unsigned char*)buffer, ret);
        }
    }
    
    if(fd == target_device_fd && target_device_fd != 0){
        pthread_mutex_unlock(&mutex_lock);
        printf("Read Thread Unlocked By %i\n", tid);
    }
 
	return ret;
}

/**
 *
 */
int _write(int fd, void *buffer, int nbyte)
{
    
    unsigned int tid = (unsigned int)pthread_self();
    
    if(fd == target_device_fd && target_device_fd != 0){
        pthread_mutex_lock(&mutex_lock);
        printf("Write Thread Locked To Write %i Bytes By %i\n", nbyte, tid);
    }
    
    int ret = write(fd,buffer,nbyte);
    
    if(strlen(fds[fd]) != 0){
        if(logs[fd] != 0){
            //hexdump((unsigned char*)buffer,ret);
            fprintf(logs[fd], "\n\nWrite to %s BYTES: %i RET: %i THREAD: %i:\n",fds[fd], nbyte, ret, tid);
            hexdump_log(logs[fd], (unsigned char*)buffer, nbyte);
        }
    }
    
    if(fd == target_device_fd && target_device_fd != 0){
        pthread_mutex_unlock(&mutex_lock);
        printf("Write Thread Unlocked By %i\n", tid);
    }
    
	return ret;
}


/**
 * _close - Custom Close Function
 */
int _close(int fd)
{
	int ret = close(fd);
    
	if (logs[fd] != 0){
		fclose(logs[fd]);
        logs[fd] = 0;
	}
	
    if(strlen(fds[fd]) != 0){
        printf("Closed %s\n",fds[fd]);
        memset(fds[fd], 0x00, sizeof(fds[fd]));
        
    }

	return ret;
}


/**
 * _open - Custom Open Function
 */
int _open(const char *path, int flags, mode_t mode)
{
    
    int fd = open(path, flags, mode);
    
    if(strlen(fds[fd]) == 0){
        strcpy(fds[fd],path);
    }
    
    for(int i = 0; i < sizeof(log_targets)/sizeof(std::string); i++){
        if(strcmp(log_targets[i].c_str(),path) == 0){
            printf("Opened %s - FD: %i FLAGS: %x MODE: %x\n", path, fd, flags, mode);
            if(logs[fd] == 0){
                std::string _path = path;
                const char* log_file_name = _path.substr(_path.find_last_of("/")+1).c_str();
                char _log_file_path[256];
                sprintf(_log_file_path,"%s/CommCenter-%s.log",log_path, log_file_name);
                printf("Logging to %s\n",_log_file_path);
                logs[fd] = fopen(_log_file_path,"a+");
            }           
        }
    }
    
    
    if(strcmp(path,target_device) == 0 && !server_started){
        target_opened = true;
        server_started = true;
        target_device_fd = fd;
        boost::thread server_thread(&start_server);
        if(server_thread.joinable()) {
            printf("Detaching Server Thread into Background\n");
            server_thread.detach();
        }
    } else {
        printf("Untargeted File Opened %s - FD: %i FLAGS: %x MODE: %x\n", path, fd, flags, mode);
    }
    return fd;
}