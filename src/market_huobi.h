#ifndef MARKET_HUOBI_H
#define MARKET_HUOBI_H

#include "market_interface.h"
class MarketHuobi : public MarketInterface{
public:
  virtual std::map<std::string, std::vector<std::string>> GetMarketPairMap() override;
private:
  void InitMarketPair();
private:
    std::map<std::string, std::vector<std::string>> market_pair_map_;
    std::map<std::pair<std::string, std::string>,CoinInfo> coin_map_;
public:
  MarketHuobi();
};

#endif // MARKET_HUOBI_H
