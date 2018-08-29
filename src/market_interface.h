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
  std::list<std::pair<std::string, std::string>> bids_;
  std::list<std::pair<std::string, std::string>> asks_;
};
class CoinInfo{
public:
  float price_now();
  void set_price_now(float price);
  DepthInfo depth_info_;
  ActiveInfo active_info_;
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
