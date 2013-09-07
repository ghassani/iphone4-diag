// AppleTest.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <string>
#include "boost/asio.hpp"
#include "boost/array.hpp"
#include "util.h"
#include "pthread.h"
/*
 protected enum EnumCommands
 {
 Exit = 0xff,
 Hello = 0xfe,
 ItemRead = 0x26,
 ItemWrite = 0x27,
 Pref = 0xfd,
 SendSpc = 0x41
 }
 // byte[] buffer = this.DoPac(new byte[] { 2, 0, 0, 0, 0xfd, 0xfe });
 
 */
char cmd_DiagSendSpcZeros[] = { 0x30, 0x30, 0x30, 0x30, 0x30, 0x30 };
unsigned char cmd_DiagSendSpcZerosWithCrc[] = { 0x41, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xDF, 0x8A, 0x7E };
unsigned char packetSPC[] = { 2, 0, 0, 0, 0xfd, 0x41, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x8A, 0x7E };
unsigned char serialNumberPacket[] = { 2, 0, 0, 0, 0xfd, 0xfe };
unsigned char at[] = "AT\r\n";
unsigned char at2[] = "AT$QCDMG\r\n";
unsigned char spc[] = { 0x41, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xDF, 0x8A, 0x7E };
unsigned char spc1[] = { 0x7D, 0x41, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xDF, 0x8A, 0x7E };
unsigned char spc2[] = { 0x7E, 0x41, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xDF, 0x8A, 0x7E };
//unsigned char serialNumberPacket[] = { 2, 0, 0, 0, 0xfd, 0xfe };

using namespace std;

int main(int argc, char* argv[])
{
    const char* host = "127.0.0.1";
    const char* port = "8885";
    boost::asio::ip::address address;
    address = address.from_string(host);

    
    try {
        boost::asio::io_service io_service;
        boost::asio::ip::tcp::resolver::query query(host, port);
        boost::asio::ip::tcp::resolver resolver(io_service);
        boost::asio::ip::tcp::resolver::iterator destination = resolver.resolve(query);
        boost::asio::ip::tcp::resolver::iterator end ;
        boost::asio::ip::tcp::endpoint endpoint;
        
        while ( destination != end ) {
            endpoint = *destination++;
            std::cout<<endpoint<<std::endl;
        }
        
        boost::asio::ip::tcp::socket socket(io_service);
        socket.connect(endpoint);
        std::cout << "Connected" << std::endl;
        
        size_t wrote = boost::asio::write(socket, boost::asio::buffer(spc1), boost::asio::transfer_all());
        boost::array< char, 128 > buf;
        boost::system::error_code error;
        std::size_t read = boost::asio::read(socket, boost::asio::buffer(buf, 512), boost::asio::transfer_all(), error);
        printf("Read %li\n",read);
        int i;
        for(i = 0; i <= read; i++){
            printf("%02X ",buf[i]);
        }
        std::cout << "DONE WITH READ" << std::endl;
        
        //unsigned char packet[] = { 55 };
        
        /*
        size_t wrote = boost::asio::write(socket, boost::asio::buffer(serialNumberPacket), boost::asio::transfer_all());
        std::cout << "Wrote " << wrote << " Bytes" << std::endl;
        boost::array< char, 128 > buf;
        boost::system::error_code error;
        std::size_t read = boost::asio::read(socket, boost::asio::buffer(buf, 512), boost::asio::transfer_all(), error);
        std::cout << "Read " << read << " Bytes" << std::endl;
        int i;
        for(i = 0; i <= read; i++){
            printf("%02X ",buf[i]);
        }
        std::cout << "DONE WITH READ" << std::endl;
        
        size_t wrote2 = boost::asio::write(socket, boost::asio::buffer(packetSPC), boost::asio::transfer_all());
        std::cout << "Wrote " << wrote2 << " Bytes" << std::endl;
        std::size_t read2 = boost::asio::read(socket, boost::asio::buffer(buf, 512), boost::asio::transfer_all(), error);
        std::cout << "Read " << read2 << " Bytes" << std::endl;
        
        for(i = 0; i <= read2; i++){
            printf("%02X ",buf[i]);
        }
        std::cout << "DONE WITH READ2" << std::endl;
        */
        
        /*
         char buffer[1024];
         size_t read = boost::asio::read(socket, boost::asio::buffer(buffer));
         
         std::cout << "Read " << read << " Bytes" << std::endl;
         std::cout << std::endl;
         std::cout << buffer;*/
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    
    cin.get();
    
    return 0;
}

