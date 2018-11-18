#ifndef HUOBIMARKET_H
#define HUOBIMARKET_H

#include "MarketInterface.h"
#include <QWebSocket>

class HuobiMarket : public MarketInterface
{
  Q_OBJECT
public:
  HuobiMarket();
  virtual ~HuobiMarket(){}
private:
  std::list<std::pair<std::string, std::string>> market_pair_list_;
  QWebSocket web_socket_depth_;
private:
  void InitMarket();
public slots:
  void OnDepthConnected();
  void OnDepthDisConnected();
  void OnDepthMsgReceived(const QByteArray &message);
public:
  void StartWatch();
  std::list<std::pair<std::string, std::string>> GetMarketPair();
  CoinInfo GetCoinInfo(std::pair<std::string, std::string> pair);
};

#endif // HUOBIMARKET_H
