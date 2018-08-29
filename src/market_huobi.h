#ifndef MARKET_HUOBI_H
#define MARKET_HUOBI_H
#include <memory.h>
#include <boost/unordered_map.hpp>
#include "market_interface.h"
#include "AsioHttps.h"
class MarketHuobi : public MarketInterface{
public:
  virtual std::map<std::string, std::vector<std::string>> GetMarketPairMap() override;
  virtual void StartWatch();

private:
  void OnGetDepth(std::shared_ptr<AsioHttpsRequest> request, std::shared_ptr<HttpResponseMsgStruct> responce, std::string coin_for, std::string coin_base);
private:
  void InitMarketPair();
private:
    AsioHttps asio_https_;

    std::map<std::string, std::vector<std::string>> market_pair_map_;
    std::map<std::string,CoinInfo> coin_map_;
    std::map<std::string,std::shared_ptr<AsioHttpsSocket> > socket_map_;
public:
  MarketHuobi();
};

#endif // MARKET_HUOBI_H
