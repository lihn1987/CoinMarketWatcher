#include "HuobiMarket.h"
#include <QDebug>
#include <boost/iostreams/filter/gzip.hpp>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

HuobiMarket::HuobiMarket(){
  InitMarket();
  connect(&web_socket_,SIGNAL(connected()),this,SLOT(OnDepthConnected()),Qt::AutoConnection);
  connect(&web_socket_,SIGNAL(disconnected()),this,SLOT(OnDepthDisConnected()),Qt::AutoConnection);
  connect(&web_socket_,SIGNAL(binaryMessageReceived(QByteArray)),this,SLOT(OnDepthMsgReceived(const QByteArray &)),Qt::AutoConnection);
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
  std::list<std::pair<std::string/*base*/, std::string/*quote*/>> trade_list = trade_pair_.GetTradePairListByQuoteCoin("btc");
  for(const auto &item: trade_list){
    QString str = "{\
                  \"sub\": \"market.%1%2.trade.detail\",\
                  \"id\": \"asdfew2323aedfry\"\
                }";
    str = str.arg(item.first.c_str()).arg(item.second.c_str());
    web_socket_.sendTextMessage(str);
  }
}

void HuobiMarket::SubScribeMarketDepth(){
  QString str = "{\
    \"req\": \"market.btcusdt.depth.step0\",\
    \"id\": \"asdfew2323aedfry\"\
  }";
  web_socket_.sendTextMessage(str);
}

void HuobiMarket::OnDepthConnected(){
  qDebug()<<"Depth connected!";
  SubScribeTradeDetail();
  //SubScribeMarketDepth();
}

void HuobiMarket::OnDepthDisConnected(){
  qDebug()<<"Depth disconnected!";
}

void HuobiMarket::OnDepthMsgReceived(const QByteArray &message){
  std::string str_unzip;
  {
    boost::iostreams::filtering_ostream fos;
    fos.push(boost::iostreams::gzip_decompressor());
    fos.push(boost::iostreams::back_inserter(str_unzip));
    fos << message.toStdString();
    fos << std::flush;
  }
  qDebug()<<"Depth getmessage:"<<str_unzip.data();
  boost::property_tree::ptree pt;
  std::istringstream istm(str_unzip);
  try{
    boost::property_tree::read_json(istm, pt);
    std::string value = pt.get<std::string>("ping", "-1");
    if(value != "-1"){
      qDebug()<<"处理ping信息";
      std::string str_out = (boost::format("{\"pong\":%s}")%value).str();
      web_socket_.sendTextMessage(QString::fromStdString(str_out));
    }
    else{
      value = pt.get<std::string>("rep", "-1");
      if(value != "-1"){
        qDebug()<<"处理depth信息";
      }else{
        value = pt.get<std::string>("ch", "-1");
        if(value != "-1"){
          qDebug()<<"处理tradedetail信息";
          try{
            //boost::property_tree::ptree child_pt = pt.get_child("tick");
            //for(boost::property_tree::ptree::value_type value:child_pt){
              for(boost::property_tree::ptree::value_type value_item: pt.get_child("tick.data")){
                TradeItem trade_item;
                trade_item.id_ = value_item.second.get<std::string>("id");
                trade_item.price_ = value_item.second.get<std::string>("price");
                trade_item.amount_ = value_item.second.get<std::string>("amount");
                trade_item.direction_ = value_item.second.get<std::string>("direction");
                trade_item.ts_ = value_item.second.get<std::string>("ts");
                //trade_history.trade_list_.push_back(trade_item);
                qDebug()<<"处理tradedetail信息成功";
                qDebug()<<trade_item.ToQString();
              }
            //}
          }catch(...){//json 解析失败
            qDebug()<<"处理tradedetail信息失败";
          }
        }
        else{
          qDebug()<<"无法识别的信息";
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
    qDebug()<<trade_pair_.toString();
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

CoinInfo HuobiMarket::GetCoinInfo(std::pair<std::string, std::string> pair)
{
  return CoinInfo();
}
