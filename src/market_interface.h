#ifndef MARKET_INTERFACE_H
#define MARKET_INTERFACE_H
#include <memory>
#include <vector>
#include <map>
#include <atomic>
#include <boost/date_time.hpp>
class MarketHuobi;
class MarketOK;
class MarketBinance;
class MarketBitFinex;

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
  float price_now();
  void set_price_now(float price);
  DepthInfo depth_info_;
  TradeHistory last_trade_history_;

private:
  std::atomic<float> price_now_;
};

class MarketInterface
{
public:
  enum MarketType{
    MT_HUOBI=1,
    MT_OK,
    MT_BINANCE,
    MT_BITFINEX
  };
public:
  virtual std::map<std::string, std::vector<std::string>> GetMarketPairMap(){return std::map<std::string, std::vector<std::string>>();}
public:
  static std::shared_ptr<MarketInterface> CreateMarket(MarketType market_type);
  static const std::vector<std::pair<std::string, enum MarketType>> GetMarketList();
private:
  static std::vector<std::pair<std::string, enum MarketType>> market_list;
protected:
  MarketInterface();
};

#endif // MARKET_INTERFACE_H
