#ifndef HUOBIMARKET_H
#define HUOBIMARKET_H

#include "MarketInterface.h"
#include <QWebSocket>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <QSqlDatabase>
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
  QNetworkAccessManager network_info_;
  std::list<std::string> coin_list_;
  DelayState delay_state_;
  uint32_t depth_idx_ = 0;//depth请求索引
  QSqlDatabase db_;
private:
  void InitMarket();
  void LoadTradePair();
  void SubScribeTradeDetail();
  void SubScribeMarketDepth();
  bool ComputeBuy(const std::string& coin_symbol);
  bool ComputeSell(const std::string& coin_symbol);
  void Disconnect();
public slots:
  void OnConnected();
  void OnDisConnected();
  void OnSubScribeMsgReceived(const QByteArray &message);
  void OnLoadTradePair();
  void OnLoadCoinInfo();
public:
  std::vector<QuantitativeTransactionItem> buy_quan_list_;
  std::vector<QuantitativeTransactionItem> sell_quan_list_;
public:
  void StartWatch();
  void StopWatch();
  std::list<std::pair<std::string, std::string>> GetMarketPair();
  CoinInfo& GetCoinInfo();
  DelayState GetDelayState();
  std::list<std::pair<std::string/*base*/, std::string/*quote*/>> GetTradePairList();
  std::list<TradeHistoryItem> GetTradeHistory();
  void ClearTradeHistory();
public://账户相关
  void SetSimulate(bool is_simulate);
  double GetBalanceBtcAll();
  std::map<std::string/*symbol*/, double/*amount*/> GetBalanceAll();
  double GetBalance(const std::string& symbol);
  void SetBalance(const std::string& symbol, double amount);
  void ClearBalance();
  void Buy(const std::string& symbol, double count);
  void Sell(const std::string& symble, double count);
  void Log();
private:
  std::map<std::string/*symbol*/, double/*amount*/> balance_;
  bool is_watching_;
  bool is_simulate_;

};

#endif // HUOBIMARKET_H
