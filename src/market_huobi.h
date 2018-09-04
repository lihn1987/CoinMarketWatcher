#ifndef MARKET_HUOBI_H
#define MARKET_HUOBI_H
#include <memory.h>
#include <boost/unordered_map.hpp>
#include <boost/signals2.hpp>
#include "market_interface.h"
#include "AsioHttps.h"
class MarketHuobi : public MarketInterface{
public:
  virtual std::map<std::string, std::vector<std::string>> GetMarketPairMap() override;
  virtual void StartWatch();

private:
  void OnGetDepth(std::shared_ptr<AsioHttpsRequest> request, std::shared_ptr<HttpResponseMsgStruct> responce, std::string coin_for, std::string coin_base);
  void OnGetTradeHistory(std::shared_ptr<AsioHttpsRequest> request, std::shared_ptr<HttpResponseMsgStruct> responce, std::string coin_for, std::string coin_base);
public:
  //获取某个交易对的深度信息，例如coin_for: eos, coin_base: usd
  DepthInfo GetCoinDepthInfo(const std::string& coin_for, const std::string& coin_base);
  //获取某个交易对的最新交易历史，例如coin_for: eos, coin_base: usd
  TradeHistory GetCoinTradeHistory(const std::string& coin_for, const std::string& coin_base);
  boost::signals2::connection AddDepthChangeWatcher(boost::function<void(std::string , std::string, DepthInfo)> callback);
  boost::signals2::connection AddTradeHistroyChangeWatcher(boost::function<void(std::string , std::string, TradeHistory)> callback);
private:
    AsioHttps asio_https_;

    std::map<std::string, std::vector<std::string>> market_pair_map_;
    std::map<std::string,CoinInfo> coin_info_;
    std::map<std::string,std::shared_ptr<AsioHttpsSocket> > socket_map_;
    std::mutex coin_info_mutex_;
    boost::signals2::signal<void(std::string , std::string, DepthInfo)> DoDepthChangeSig;
    boost::signals2::signal<void(std::string , std::string, TradeHistory)> DoTradeHistroyChangeSig;

private:
    void InitMarketPair();

public:
  MarketHuobi();
};

#endif // MARKET_HUOBI_H
