#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "session.cpp"

using boost::asio::ip::tcp;
using namespace boost;

class server
{
    
    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;
    int& fd_baseband;
    
public:
  server(boost::asio::io_service& io_service, short port, int &fd)
    : io_service_(io_service),
      acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
      fd_baseband(fd)
    {
        //printf("Server Starting with File Descriptor %i", fd_baseband);
        start_accept();
    }
   
    
private:
    void start_accept()
    {
        //printf("Accepting with File Descriptor %i\n",fd_baseband);
        session* new_session = new session(io_service_, fd_baseband);
        acceptor_.async_accept(new_session->socket(), boost::bind(&server::handle_accept, this, new_session, boost::asio::placeholders::error));
    }

    
    void handle_accept(session* new_session, const boost::system::error_code& error)
    {
        if (!error)
        {
          new_session->start();
          printf("Server Session with File Descriptor %i", fd_baseband);
        }
        else
        {
          delete new_session;
        }

        start_accept();
    }

    
};