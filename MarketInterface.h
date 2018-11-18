#ifndef MARKETINTERFACE_H
#define MARKETINTERFACE_H
#include <boost/date_time.hpp>
#include <boost/format.hpp>
#include <QObject>
#include <QDateTime>

/*
 * 用于封装每个交易所所有交易对的类
 */
class TradePair{
public:
  void AddTradePair(std::string base_coin, std::string quote_coin){
    trade_pair_list_.push_back(std::make_pair(base_coin, quote_coin));
    trade_base_coin_map_[base_coin].push_back(std::make_pair(base_coin, quote_coin));
    trade_quote_coin_map_[quote_coin].push_back(std::make_pair(base_coin, quote_coin));
  }
public:
  std::list<std::pair<std::string/*base*/, std::string/*quote*/>> GetTradePairList(){return trade_pair_list_;}
  std::list<std::pair<std::string/*base*/, std::string/*quote*/>> GetTradePairListByBaseCoin(std::string base_coin){
    if(trade_base_coin_map_.find(base_coin) != trade_base_coin_map_.end()){
      return trade_base_coin_map_[base_coin];
    }else{
      return std::list<std::pair<std::string/*base*/, std::string/*quote*/>>();
    }
  }
  std::list<std::pair<std::string/*base*/, std::string/*quote*/>> GetTradePairListByQuoteCoin(std::string quote_coin){
    if(trade_quote_coin_map_.find(quote_coin) != trade_quote_coin_map_.end()){
      return trade_quote_coin_map_[quote_coin];
    }else{
      return std::list<std::pair<std::string/*base*/, std::string/*quote*/>>();
    }
  }
public:
  QString toString(){
    std::string rtn;
    for(auto item: trade_pair_list_){
      rtn+= item.first+item.second+"\r\n";
    }
    return rtn.data();
  }
private:
  std::list<std::pair<std::string/*base*/, std::string/*quote*/>> trade_pair_list_;
  std::map<std::string, std::list<std::pair<std::string/*base*/, std::string/*quote*/>>> trade_base_coin_map_;
  std::map<std::string, std::list<std::pair<std::string/*base*/, std::string/*quote*/>>> trade_quote_coin_map_;
};
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
  std::string ts_;//时间
  QString ToQString(){
    QString rtn = "以%1的价格在%2的时候%3了%4币，当前id为%5";
    QDateTime time;
    time.setMSecsSinceEpoch(atol(ts_.data()));
    rtn = rtn.arg(price_.data()).arg(time.toString("yyyy-MM-dd hh:mm:ss:zzz")).arg(direction_ == "sell"?"卖":"买").arg(amount_.data()).arg(id_.data());
    return rtn;
  }
  std::string ToStdString(){
    QDateTime time;
    time.setMSecsSinceEpoch(atol(ts_.data()));
    return (boost::format("以%1的价格在%2的时候%3了%4币，当前id为%5")
         %price_
         %time.toString("yyyy-MM-dd hh:mm:ss:zzz").toStdString()
         %(direction_ == "sell"?"卖":"买")
         %amount_.data()
         %id_.data())
        .str();
  }
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
