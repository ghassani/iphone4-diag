#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "util.h"
#include "pthread.h"
#include "main.h"

using boost::asio::ip::tcp;
using namespace boost;

class session
{
public:
    tcp::socket socket_;
    int& fd_baseband;
    enum { max_length = 2048 };
    char data_[max_length];
    unsigned char serial_data_[max_length];
    const char* read_none = "NO";    
    
    session(boost::asio::io_service& io_service, int &fd)
    : socket_(io_service), fd_baseband(fd)
    {

    }

    ~session()
    {
      
    }
  
    tcp::socket& socket()
    {
        return socket_;
    }

    
    void start()
    {
        socket_.async_read_some(
            boost::asio::buffer(data_, max_length),
            boost::bind(
                &session::handle_read,
                this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
            )
        );
  }

private:
    /**
     * handle_read
     * 
     */
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred)
    {
        //int fd = open("/dev/tty.debug", O_RDWR | O_NOCTTY);
        
        if (!error){
            printf("Read From Socket:\n");
            hexdump((unsigned char*)data_, bytes_transferred);
            
            int wrote = ::_write(fd_baseband, data_, bytes_transferred);
            printf("Wrote %i Bytes Expected %li\n",wrote, bytes_transferred);
            
            fd_set readfds;
            struct timeval timeout;
            FD_ZERO(&readfds);
            FD_SET(fd_baseband, &readfds);
            int bytes_read = 0;
            // Wait a second
            timeout.tv_sec = 1;
            timeout.tv_usec = 500000;
            memset(serial_data_,0x00,max_length);
            
            if(select(fd_baseband+1, &readfds, NULL, NULL, &timeout) > 0){
                bytes_read = ::_read(fd_baseband, serial_data_, max_length);
                FD_ZERO(&readfds);
                FD_SET(fd_baseband, &readfds);
                timeout.tv_sec = 0;
                timeout.tv_usec = 500000;
            }
            
            if(bytes_read > 0){
                hexdump((unsigned char*)serial_data_, bytes_read);
                boost::asio::async_write(
                    socket_,
                    boost::asio::buffer(serial_data_, bytes_read),
                    boost::bind(&session::handle_write, this, bytes_transferred, boost::asio::placeholders::error)
                );
            } else {
                hexdump((unsigned char*)read_none, sizeof(read_none));
                hexdump((unsigned char*)serial_data_, 128);
                boost::asio::async_write(
                    socket_,
                    boost::asio::buffer(read_none, sizeof(read_none)),
                    boost::bind(&session::handle_write, this, bytes_transferred, boost::asio::placeholders::error)
                );
            }            

        }else{
            printf("Error During handle_read\n");
            delete this;
        }
    }
    
    /**
     * handle_write
     *
     */
    void handle_write(size_t bytes_received, const boost::system::error_code& error)
    {
        if (!error){            
            //if(data_[0] != 0xFFFFFFFF){ // i dunno wtf this is i guess when a serial port connects it sends some garble. garble deez nuts
            socket_.async_read_some(
                boost::asio::buffer(data_, max_length),
                boost::bind(&session::handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
            );
        } else {
            delete this;
        }
    }

};