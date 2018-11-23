#ifndef MARKETINTERFACE_H
#define MARKETINTERFACE_H
#include <boost/date_time.hpp>
#include <boost/format.hpp>
#include <QObject>
#include <QDateTime>
#include <QDebug>
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

//封装对网络消息延时的监控
class DelayState{
public:
  //刷新最近接收网络信息的时间，并返回消息延时
  uint32_t Flush(uint64_t time_receive){
    latest_time_ = time_receive;
    return Delay();
  }
  //获取消息延时
  uint32_t Delay(){
    QDateTime time = QDateTime::currentDateTime();
    //qDebug()<<"消息延时为:"<<(time.toMSecsSinceEpoch() - latest_time_)/1000.0f<<"秒";
    return time.toMSecsSinceEpoch() - latest_time_;
  }
private:
  uint64_t latest_time_;
};
struct DepthInfo{
  std::list<std::pair<std::string/*价格*/, std::string/*数量*/>> bids_;//买价，数量begin->end 高->低
  std::list<std::pair<std::string/*价格*/, std::string/*数量*/>> asks_;//卖价，数量begin->end 低->高
  DelayState delay_state_;
};

//对每笔交易的封装
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

class CoinInfo{
public:
  std::map<std::string/*symbol*/, DepthInfo> depth_info_;
  std::map<std::string/*symbol*/, std::list<TradeItem>> trade_list_;
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
  virtual CoinInfo& GetCoinInfo() = 0;
  virtual DelayState GetDelayState() = 0;
};

#endif // MARKETINTERFACE_H
