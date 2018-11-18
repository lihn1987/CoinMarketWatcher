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
    if(value != "-1"){
      qDebug()<<"处理ping信息";
      std::string str_out = (boost::format("{\"pong\":%s}")%value).str();
      delay_state_.Flush(atol(value.c_str()));
      web_socket_.sendTextMessage(QString::fromStdString(str_out));
    }
    else{
      value = pt.get<std::string>("rep", "-1");

      if(value.find("depth") != -1){
        qDebug()<<"处理depth信息";
        //刷新延时
        delay_state_.Flush(atol(pt.get<std::string>("ts").c_str()));
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
        SubScribeMarketDepth();
        //std::string str_ch = pt.get<std::string>("ch");
      }else{
        value = pt.get<std::string>("ch", "-1");
        if(value != "-1"){
          qDebug()<<"处理tradedetail信息";
          try{
            //刷新延时
            delay_state_.Flush(atol(pt.get<std::string>("ts").c_str()));
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
