#ifndef HUOBIMARKET_H
#define HUOBIMARKET_H

#include "MarketInterface.h"
#include <QWebSocket>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include "QuantitativeTransaction.h"

class HuobiMarket : public MarketInterface
{
  Q_OBJECT
public:
  HuobiMarket();
  virtual ~HuobiMarket(){}
private:
  std::list<std::pair<std::string, std::string>> market_pair_list_;
  CoinInfo info_;
  QWebSocket web_socket_;
  TradePair trade_pair_;
  QNetworkAccessManager network_manager_;
  DelayState delay_state_;
  uint32_t depth_idx_ = 0;//depth请求索引
private:
  void InitMarket();
  void LoadTradePair();
  void SubScribeTradeDetail();
  void SubScribeMarketDepth();
  void Compute(const std::string& coin_symbol);
  void Disconnect();
public slots:
  void OnConnected();
  void OnDisConnected();
  void OnSubScribeMsgReceived(const QByteArray &message);
  void OnLoadTradePair();
public:
  std::vector<QuantitativeTransactionItem> buy_quan_list_;
public:
  void StartWatch();
  std::list<std::pair<std::string, std::string>> GetMarketPair();
  CoinInfo& GetCoinInfo();
  DelayState GetDelayState();
  std::list<std::pair<std::string/*base*/, std::string/*quote*/>> GetTradePairList();

public://账户相关
  void SetSimulate(bool is_simulate);
  double GetBalanceBtcAll();
  std::map<std::string/*symbol*/, double/*amount*/> GetBalanceAll();
  double GetBalance(const std::string& symbol);
  void SetBalance(const std::string& symbol, double amount);
  void Buy(const std::string& symbol, double count);
  void Sell(const std::string& symble, double count);
private:
  std::map<std::string/*symbol*/, double/*amount*/> balance_;
  bool is_simulate_;

};

#endif // HUOBIMARKET_H
