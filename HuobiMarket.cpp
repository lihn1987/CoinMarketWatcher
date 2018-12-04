#include "HuobiMarket.h"
#include <QDebug>
#include <boost/iostreams/filter/gzip.hpp>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

HuobiMarket::HuobiMarket(){
  InitMarket();
  connect(&web_socket_,SIGNAL(connected()),this,SLOT(OnConnected()),Qt::AutoConnection);
  connect(&web_socket_,SIGNAL(disconnected()),this,SLOT(OnDisConnected()),Qt::AutoConnection);
  connect(&web_socket_,SIGNAL(binaryMessageReceived(QByteArray)),this,SLOT(OnSubScribeMsgReceived(const QByteArray &)),Qt::AutoConnection);
}

void HuobiMarket::InitMarket(){

}

void HuobiMarket::LoadTradePair(){
  QNetworkRequest request(QUrl("https://api.huobi.pro/v1/common/symbols"));
  QNetworkReply * reply = network_manager_.get(request);
  connect(reply, SIGNAL(finished()), this, SLOT(OnLoadTradePair()));
}

void HuobiMarket::SubScribeTradeDetail()
{
  std::list<std::pair<std::string/*base*/, std::string/*quote*/>> trade_list = market_pair_list_;
  for(const auto &item: trade_list){
    QString str = "{\
                  \"sub\": \"market.%1%2.trade.detail\",\
                  \"id\": \"%3\"\
                }";
    str = str.arg(item.first.c_str()).arg(item.second.c_str()).arg(QString::number(qrand()));
    web_socket_.sendTextMessage(str);
  }
}

void HuobiMarket::SubScribeMarketDepth(){
  std::vector<std::pair<std::string, std::string>> market_pair_vector(market_pair_list_.begin(), market_pair_list_.end());
  std::pair<std::string, std::string> item = market_pair_vector[depth_idx_];
  QString str = "{\
    \"req\": \"market.%1%2.depth.step0\",\
    \"id\": \"%3\"\
  }";
  str = str.arg(item.first.c_str()).arg(item.second.c_str()).arg(QString::number(qrand()));
  web_socket_.sendTextMessage(str);
  depth_idx_++;
  if(depth_idx_ == market_pair_list_.size()){
    depth_idx_ = 0;
  }
}

bool HuobiMarket::ComputeBuy(const std::string &coin_symbol){
  bool pass = buy_quan_list_.size()?true:false;
  for(size_t i = 0; i < buy_quan_list_.size(); i++){
    if(pass == false){
      buy_quan_list_[i].Removesymbol(coin_symbol);
    }else if(!buy_quan_list_[i].Compute(coin_symbol, info_)){
      pass = false;
    }
  }
  return pass;
}

bool HuobiMarket::ComputeSell(const std::string &coin_symbol){
  std::set<std::string> symbol_set;
  for(std::pair<std::string/*symbol*/, double/*amount*/> item: balance_){
    if(item.first == "btc")continue;
    if(item.second*atof(info_.trade_list_[item.first+"btc"].back().price_.c_str())>0.0001){
      symbol_set.insert(item.first+"btc");
    }
  }
  //防止没有卖出时执行卖出
  bool pass = sell_quan_list_.size()?true:false;
  for(size_t i = 0; i < sell_quan_list_.size(); i++){
    if(pass == false){
      sell_quan_list_[i].Removesymbol(coin_symbol);
    }else if(symbol_set.find(coin_symbol) == symbol_set.end()||!sell_quan_list_[i].Compute(coin_symbol, info_)){
      pass = false;
    }else {
      int fordebug = 1;
      fordebug=0;
    }
  }
  return pass;
}

void HuobiMarket::Disconnect(){
 //TODO:头几条消息延时会非常长，不能从一开始就记录超时
 // web_socket_.close();
}

void HuobiMarket::OnConnected(){
  qDebug()<<"connected!";
  SubScribeTradeDetail();
  SubScribeMarketDepth();
  SubScribeMarketDepth();
  SubScribeMarketDepth();
  SubScribeMarketDepth();
  SubScribeMarketDepth();
  //depth_timer_.start(100);
}

void HuobiMarket::OnDisConnected(){
  qDebug()<<"Depth disconnected!";
  StartWatch();
}

void HuobiMarket::OnSubScribeMsgReceived(const QByteArray &message){
  std::string str_unzip;
  {
    boost::iostreams::filtering_ostream fos;
    fos.push(boost::iostreams::gzip_decompressor());
    fos.push(boost::iostreams::back_inserter(str_unzip));
    fos << message.toStdString();
    fos << std::flush;
  }
  qDebug()<<"getmessage:"<<str_unzip.data();
  boost::property_tree::ptree pt;
  std::istringstream istm(str_unzip);
  try{
    boost::property_tree::read_json(istm, pt);
    std::string value = pt.get<std::string>("ping", "-1");
    if(value != "-1"){//处理pingpong
      qDebug()<<"处理ping信息";
      std::string str_out = (boost::format("{\"pong\":%s}")%value).str();
      delay_state_.Flush(atol(value.c_str()));
      web_socket_.sendTextMessage(QString::fromStdString(str_out));
    }
    else{//处理深度信息
      value = pt.get<std::string>("rep", "-1");

      if(value.find("depth") != std::string::npos){
        qDebug()<<"处理depth信息";
        //刷新延时
        if(delay_state_.Flush(atol(pt.get<std::string>("ts").c_str()))>10000){
          Disconnect();
        }
        //分析币种
        std::string str_ch = pt.get<std::string>("rep");
        std::vector<std::string> ch_tmp;
        boost::split(ch_tmp, str_ch, boost::is_any_of("."));
        str_ch = ch_tmp[1];
        info_.depth_info_[str_ch].bids_.clear();
        info_.depth_info_[str_ch].asks_.clear();
        for(boost::property_tree::ptree::value_type value_item: pt.get_child("data.bids")){
          std::vector<std::string> vec_str;
          for(boost::property_tree::ptree::value_type value_item2: value_item.second){
            vec_str.push_back(value_item2.second.get<std::string>(""));
          }
          if(vec_str.size() >= 2){
            info_.depth_info_[str_ch].bids_.push_back(std::make_pair(vec_str[0], vec_str[1]));
          }
        }
        for(boost::property_tree::ptree::value_type value_item: pt.get_child("data.asks")){
          std::vector<std::string> vec_str;
          for(boost::property_tree::ptree::value_type value_item2: value_item.second){
            vec_str.push_back(value_item2.second.get<std::string>(""));
          }
          if(vec_str.size() >= 2){
            info_.depth_info_[str_ch].asks_.push_back(std::make_pair(vec_str[0], vec_str[1]));
          }
        }
        info_.depth_info_[str_ch].delay_state_.Flush(atol(pt.get<std::string>("ts").c_str()));
        if(ComputeBuy(str_ch)){
          if(balance_["btc"] >= 0.001){
            Buy(str_ch, 0.001);
          }
        }
        if(ComputeSell(str_ch)){
          int idx = str_ch.find_last_of("btc");
          if(idx != -1){
            std::string arm_coin_symbol = std::string(str_ch.c_str(), idx-3+1);
            Sell(str_ch, balance_[arm_coin_symbol]);
          }
        }
        SubScribeMarketDepth();
        //std::string str_ch = pt.get<std::string>("ch");
      }else{//处理交易信息
        value = pt.get<std::string>("ch", "-1");
        if(value != "-1"){
          qDebug()<<"处理tradedetail信息";
          try{
            //刷新延时
            if(delay_state_.Flush(atol(pt.get<std::string>("ts").c_str())) > 10000){
              Disconnect();
            }
            //分析币种
            std::string str_ch = pt.get<std::string>("ch");
            std::vector<std::string> ch_tmp;
            boost::split(ch_tmp, str_ch, boost::is_any_of("."));
            str_ch = ch_tmp[1];
            for(boost::property_tree::ptree::value_type value_item: pt.get_child("tick.data")){
              TradeItem trade_item;
              trade_item.id_ = value_item.second.get<std::string>("id");
              trade_item.price_ = value_item.second.get<std::string>("price");
              trade_item.amount_ = value_item.second.get<std::string>("amount");
              trade_item.direction_ = value_item.second.get<std::string>("direction");
              trade_item.ts_ = value_item.second.get<std::string>("ts");
              info_.trade_list_[str_ch].push_back(trade_item);
              if(info_.trade_list_[str_ch].size() > 2000){
                info_.trade_list_[str_ch].pop_front();
              }
              qDebug()<<"处理tradedetail信息成功";
              qDebug()<<trade_item.ToQString();
            }
          }catch(...){//json 解析失败
            qDebug()<<"处理tradedetail信息失败";
          }
        }
        else{
          //{"status":"error","err-code":"bad-request","err-msg":"req with invalid topic market.venbtc.depth.step0","ts":1542542578031,"id":"1973510880"}
          if(pt.get<std::string>("status") == "error" &&
             pt.get<std::string>("err-code") == "bad-request"){
            std::string err_msg = pt.get<std::string>("err-msg");
            std::vector<std::string> str_list;
            boost::split(str_list, err_msg, boost::is_any_of("."));
            if((str_list.size() > 2 && str_list[2] == "depth")||err_msg == "symbol trade not open now"){
              //depth请求错误
              SubScribeMarketDepth();
            }
          }else{
            qDebug()<<"无法识别的信息";
          }
        }
      }
    }
  }
  catch(...){
    //TODO
  }

}

void HuobiMarket::OnLoadTradePair(){
  qDebug()<<"收到了交易对回复";
  QNetworkReply * reply = dynamic_cast<QNetworkReply*>(sender());
  if(reply->error())qDebug()<<reply->errorString();
  QByteArray array = reply->readAll();
  std::istringstream istm(std::string(array.data(), array.size()));
  try{
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(istm, pt);
    for(boost::property_tree::ptree::value_type value_item: pt.get_child("data")){
      trade_pair_.AddTradePair(
        value_item.second.get<std::string>("base-currency"),
        value_item.second.get<std::string>("quote-currency")
      );
    }
    qDebug()<<"交易对解析成功";
    market_pair_list_ = trade_pair_.GetTradePairListByQuoteCoin("btc");
    market_pair_list_.push_back(std::make_pair("btc", "usdt"));
    //market_pair_list_.push_back(std::make_pair("ltc", "usdt"));
    web_socket_.open(QUrl("wss://api.huobi.pro/ws"));
  }catch(...){

  }
  reply->deleteLater();
}

void HuobiMarket::StartWatch(){
  LoadTradePair();
}

std::list<std::pair<std::string, std::string> > HuobiMarket::GetMarketPair(){
  return market_pair_list_;
}

CoinInfo& HuobiMarket::GetCoinInfo(){
  return info_;
}

DelayState HuobiMarket::GetDelayState(){
  return delay_state_;
}

std::list<TradeHistoryItem> HuobiMarket::GetTradeHistory(){
  return info_.trade_history_;
}

void HuobiMarket::ClearTradeHistory(){
  info_.trade_history_.clear();
}

void HuobiMarket::SetSimulate(bool is_simulate){
  is_simulate_ = is_simulate;
}

double HuobiMarket::GetBalanceBtcAll(){
  double rtn = 0;
  for(std::pair<std::string/*symbol*/, double/*amount*/>item: balance_){
    if(item.first == "btc"){
      rtn += item.second;
    }else if(item.second != 0){
      std::string symbol = (item.first+"btc");
      if(info_.trade_list_.find(symbol) != info_.trade_list_.end()){
        rtn += QString::fromStdString(info_.trade_list_[symbol].back().price_).toDouble()*item.second;
      }
    }
  }
  return rtn;
}

std::map<std::string, double> HuobiMarket::GetBalanceAll(){
  return balance_;
}

double HuobiMarket::GetBalance(const std::string &symbol){
  if(balance_.find(symbol) != balance_.end()){
    return balance_[symbol];
  }else{
    return 0;
  }
}

void HuobiMarket::SetBalance(const std::string &symbol, double amount){
  balance_[symbol] = amount;
}

void HuobiMarket::ClearBalance(){
  balance_.clear();
}

void HuobiMarket::Buy(const std::string &symbol, double count){
  if(is_simulate_){//模拟买入
    int idx = symbol.find_last_of("btc");
    if(idx != -1){
      std::string arm_coin_symbol = std::string(symbol.c_str(), idx-3+1);
      if(balance_[arm_coin_symbol]*atof(info_.trade_list_[symbol].back().price_.c_str()) > 0.0005){
        //已经有了，不再买了
        return;
      }
      if(balance_.find(arm_coin_symbol) == balance_.end()){
        balance_[arm_coin_symbol] = 0;
      }
      double receive_count = count;
      for(std::pair<std::string/*价格*/, std::string/*数量*/> item:info_.depth_info_[symbol].asks_){
        if(atof(item.first.c_str())*atof(item.second.c_str()) <= receive_count){
          receive_count -= atof(item.first.c_str())*atof(item.second.c_str());
          balance_[arm_coin_symbol]+=atof(item.second.c_str());
        }else{
          balance_[arm_coin_symbol]+=receive_count/atof(item.first.c_str());
          break;
        }
      }
      //扣税。。。。。。
      balance_[arm_coin_symbol]*= 0.998;
      TradeHistoryItem item;
      item.coin_ = symbol;
      item.count_ =balance_[arm_coin_symbol];
      item.price_ = count/balance_[arm_coin_symbol];
      item.buy_ = true;
      info_.trade_history_.push_back(item);
      balance_["btc"] -= count;
    }
  }
  Log();
}

void HuobiMarket::Sell(const std::string &symbol, double count){
  if(is_simulate_){//模拟买入
    int idx = symbol.find_last_of("btc");
    if(idx != -1){
      std::string arm_coin_symbol = std::string(symbol.c_str(), idx-3+1);
      if(balance_[arm_coin_symbol]*atof(info_.trade_list_[symbol].back().price_.c_str()) < 0.0005){
        //没有，不能卖
        return;
      }
      if(balance_.find(arm_coin_symbol) == balance_.end()){
        balance_[arm_coin_symbol] = 0;
      }
      //交税
      double receive_count = count*= 0.998;
      double btc_all = 0;
      for(std::pair<std::string/*价格*/, std::string/*数量*/> item:info_.depth_info_[symbol].bids_){
        if(atof(item.second.c_str()) <= receive_count){
          receive_count -= atof(item.second.c_str());
          balance_["btc"]+=atof(item.second.c_str())*atof(item.first.c_str());
          btc_all+=atof(item.second.c_str())*atof(item.first.c_str());
        }else{
          balance_["btc"]+=receive_count*atof(item.first.c_str());
          btc_all+=receive_count*atof(item.first.c_str());
          break;
        }
      }
      balance_[arm_coin_symbol] = 0;
      balance_.erase(arm_coin_symbol);
      TradeHistoryItem item;
      item.coin_ = symbol;
      item.count_ =count;
      item.price_ = btc_all/count;
      item.buy_ = false;
      item.time_ = QDateTime::currentDateTime().toString("YYYY:MM:dd-hh-mm-ss").toStdString();
      info_.trade_history_.push_back(item);
      //balance_["btc"] -= count;
    }
  }
  Log();
}
#include <QDateTime>
#include <QFile>
void HuobiMarket::Log(){
  QFile file;
  static int idx=0;
  idx++;
  if(idx%2){
    file.setFileName("./a.txt");
  }else{
    file.setFileName("./b.txt");
  }
  file.open(QFile::ReadWrite);

  QString log;
  QDateTime time = QDateTime::currentDateTime();
  log += time.toString("YYYY:MM:dd-hh-mm-ss");
  log += "\r\n";
  log += "当前资产\r\n";
  for(std::pair<std::string, double>item:balance_){
    log+=item.first.c_str();
    log+=":";
    log+=QString::number(item.second);
    log+="\r\n";
  }
  log += "交易明细\r\n";
  for(TradeHistoryItem item:info_.trade_history_){
    log+= item.time_.c_str();
    log+= "~~~~";
    log+= item.buy_?"买":"卖";
    log+= "~~~~";
    log+= item.coin_.c_str();
    log+= "~~~~";
    log+= QString::number(item.price_);
    log+= "~~~~";
    log+= QString::number(item.count_);
    log+= "\r\n\"";
  }

  file.write(log.toLocal8Bit());
  file.close();
}
