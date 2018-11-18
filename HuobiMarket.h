#ifndef HUOBIMARKET_H
#define HUOBIMARKET_H

#include "MarketInterface.h"
#include <QWebSocket>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class HuobiMarket : public MarketInterface
{
  Q_OBJECT
public:
  HuobiMarket();
  virtual ~HuobiMarket(){}
private:
  std::list<std::pair<std::string, std::string>> market_pair_list_;
  QWebSocket web_socket_;
  TradePair trade_pair_;
  QNetworkAccessManager network_manager_;
  DelayState delay_state_;
private:
  void InitMarket();
  void LoadTradePair();
  void SubScribeTradeDetail();
  void SubScribeMarketDepth();
public slots:
  void OnDepthConnected();
  void OnDepthDisConnected();
  void OnSubScribeMsgReceived(const QByteArray &message);
  void OnLoadTradePair();
public:
  void StartWatch();
  std::list<std::pair<std::string, std::string>> GetMarketPair();
  CoinInfo GetCoinInfo(std::pair<std::string, std::string> pair);
  DelayState GetDelayState();
  std::list<std::pair<std::string/*base*/, std::string/*quote*/>> GetTradePairList();
};

#endif // HUOBIMARKET_H
