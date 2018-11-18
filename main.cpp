#include "MainWidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  QNetworkProxy proxy;
  proxy.setType(QNetworkProxy::Socks5Proxy);
  proxy.setHostName("127.0.0.1");
  proxy.setPort(1080);
  QNetworkProxy::setApplicationProxy(proxy);
  MainWidget w;
  w.show();

  return a.exec();
}
