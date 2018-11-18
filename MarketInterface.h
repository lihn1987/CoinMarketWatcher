#ifndef MARKETINTERFACE_H
#define MARKETINTERFACE_H
#include <boost/date_time.hpp>
#include <QObject>
struct ActiveInfo{
  boost::posix_time::ptime last_time_;
  uint32_t interval_ = 0;
};
struct DepthInfo{
  std::list<std::pair<std::string, std::string>> bids_;//卖价，数量
  std::list<std::pair<std::string, std::string>> asks_;//买价，数量
  ActiveInfo active_info_;
};
struct TradeItem{
  std::string id_;//id
  std::string price_;//价格
  std::string amount_;//数量
  std::string direction_;//方向 sell,buy
  std::string ts;//时间
};
struct TradeHistory{
  std::list<TradeItem> trade_list_;
  ActiveInfo active_info_;
};
class CoinInfo{
public:
  DepthInfo depth_info_;
  TradeHistory last_trade_history_;
};

class MarketInterface : public QObject
{
  Q_OBJECT
public:
  MarketInterface(QObject *parent = nullptr);
  virtual ~MarketInterface(){}
public:
  virtual void StartWatch() = 0;
  virtual std::list<std::pair<std::string, std::string>> GetMarketPair() = 0;
  virtual CoinInfo GetCoinInfo(std::pair<std::string, std::string> pair) = 0;
};

#endif // MARKETINTERFACE_H
