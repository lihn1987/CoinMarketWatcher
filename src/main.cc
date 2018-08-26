#include <iostream>
#include "AsioHttps.h"
#include <boost/bind.hpp>
#include <QApplication>
#include "main_widget.h"
void callback(std::shared_ptr<HttpRequestMsgStruct> request, std::shared_ptr<HttpResponseMsgStruct> response, int addtion){
  if(response->error_ == ""){
    //std::cout<<response->str_ori_<<std::endl;
    std::cout<<"get response success!"<<request->head_.GetAttribute("host")<<",addtion:"<<addtion<<std::endl;
  }else{
    std::cout<<"get response faild:"<<request->head_.GetAttribute("host")<<response->error_<<std::endl;
  }
}

int main(int argc, char** argv){
  std::cout<<"hello world!"<<std::endl;
  AsioHttps https(4);
  std::shared_ptr<AsioHttpsSocket> socket = https.CreateAsioHttpSocket();

  {
    ProxyConfig config;
    config.url_ = "127.0.0.1";
    config.port_ = 1080;

    socket->Process("https://www.google.com/"
                     , config
                     , boost::bind(callback,_1,_2, 1));
  }

  {
    ProxyConfig config;
    config.url_ = "127.0.0.1";
    config.port_ = 1080;

    socket->Process("http://www.sohu.com/"
                     , config
                     , boost::bind(callback,_1,_2, 2));
  }
  {
    ProxyConfig config;
    config.url_ = "127.0.0.1";
    config.port_ = 1080;

    socket->Process("http://www.sohu.com/"
                     , config
                     , boost::bind(callback,_1,_2, 3));
  }

  {
    socket->Process("http://www.sohu.com/"
                     , boost::bind(callback,_1,_2, 4));
  }

  {
    socket->Process("https://www.baidu.com/"
                     , boost::bind(callback,_1,_2, 5));
  }

  std::shared_ptr<AsioHttpsSocket> socket_muti[100];
  for(int i = 0; i < 100; i++){
    socket_muti[i] = https.CreateAsioHttpSocket();
    socket_muti[i]->Process("http://www.sohu.com/"
                            , boost::bind(callback,_1,_2, 6+i));
  }

  QApplication app(argc, argv);
  MainWidget widget;
  widget.show();
  app.exec();
  return 0;
}
