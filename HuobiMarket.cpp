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
  market_pair_list_.push_back(std::make_pair("USD","BTC"));
  return;
  //return;
  market_pair_list_.push_back(std::make_pair("USD","BCH"));
  market_pair_list_.push_back(std::make_pair("USD","ETH"));
  market_pair_list_.push_back(std::make_pair("USD","ETC"));

  market_pair_list_.push_back(std::make_pair("USD","LTC"));
  market_pair_list_.push_back(std::make_pair("USD","EOS"));
  market_pair_list_.push_back(std::make_pair("USD","XRP"));
  market_pair_list_.push_back(std::make_pair("USD","ADA"));
  market_pair_list_.push_back(std::make_pair("USD","OMG"));
  market_pair_list_.push_back(std::make_pair("USD","IOTA"));
  market_pair_list_.push_back(std::make_pair("USD","DASH"));
  market_pair_list_.push_back(std::make_pair("USD","ZEC"));
  market_pair_list_.push_back(std::make_pair("USD","STEEM"));
  market_pair_list_.push_back(std::make_pair("USD","HT"));

return;
  market_pair_list_.push_back(std::make_pair("BTC","BCH"));
  market_pair_list_.push_back(std::make_pair("BTC","ETH"));
  market_pair_list_.push_back(std::make_pair("BTC","LTC"));
  market_pair_list_.push_back(std::make_pair("BTC","ETC"));
  market_pair_list_.push_back(std::make_pair("BTC","EOS"));
  market_pair_list_.push_back(std::make_pair("BTC","XRP"));
  market_pair_list_.push_back(std::make_pair("BTC","ADA"));
  market_pair_list_.push_back(std::make_pair("BTC","OMG"));
  market_pair_list_.push_back(std::make_pair("BTC","IOTA"));
  market_pair_list_.push_back(std::make_pair("BTC","DASH"));
  market_pair_list_.push_back(std::make_pair("BTC","ZEC"));
  market_pair_list_.push_back(std::make_pair("BTC","STEEM"));
  market_pair_list_.push_back(std::make_pair("BTC","XMR"));

  market_pair_list_.push_back(std::make_pair("BTC","HIT"));
  market_pair_list_.push_back(std::make_pair("BTC","PAI"));
  market_pair_list_.push_back(std::make_pair("BTC","HT"));
  market_pair_list_.push_back(std::make_pair("BTC","ONT"));
  market_pair_list_.push_back(std::make_pair("BTC","XZC"));
  market_pair_list_.push_back(std::make_pair("BTC","ELA"));
  market_pair_list_.push_back(std::make_pair("BTC","THETA"));
  market_pair_list_.push_back(std::make_pair("BTC","BFT"));
  market_pair_list_.push_back(std::make_pair("BTC","CTXC"));
  market_pair_list_.push_back(std::make_pair("BTC","SOC"));
  market_pair_list_.push_back(std::make_pair("BTC","TRX"));
  market_pair_list_.push_back(std::make_pair("BTC","CMT"));
  market_pair_list_.push_back(std::make_pair("BTC","IOST"));
  market_pair_list_.push_back(std::make_pair("BTC","ZIL"));
  market_pair_list_.push_back(std::make_pair("BTC","VET"));
  market_pair_list_.push_back(std::make_pair("BTC","NAS"));
  market_pair_list_.push_back(std::make_pair("BTC","BTM"));
  market_pair_list_.push_back(std::make_pair("BTC","EDU"));
  market_pair_list_.push_back(std::make_pair("BTC","TNB"));
  market_pair_list_.push_back(std::make_pair("BTC","NEO"));
  market_pair_list_.push_back(std::make_pair("BTC","STK"));
  market_pair_list_.push_back(std::make_pair("BTC","DTA"));
  market_pair_list_.push_back(std::make_pair("BTC","ACT"));
  market_pair_list_.push_back(std::make_pair("BTC","RUFF"));
  market_pair_list_.push_back(std::make_pair("BTC","OCN"));
  market_pair_list_.push_back(std::make_pair("BTC","ICX"));
  market_pair_list_.push_back(std::make_pair("BTC","PROPY"));
  market_pair_list_.push_back(std::make_pair("BTC","GAS"));
  market_pair_list_.push_back(std::make_pair("BTC","QASH"));
  market_pair_list_.push_back(std::make_pair("BTC","BOX"));
  market_pair_list_.push_back(std::make_pair("BTC","ELF"));
  market_pair_list_.push_back(std::make_pair("BTC","YEE"));
  market_pair_list_.push_back(std::make_pair("BTC","BLZ"));
  market_pair_list_.push_back(std::make_pair("BTC","SWFTC"));
  market_pair_list_.push_back(std::make_pair("BTC","LSK"));
  market_pair_list_.push_back(std::make_pair("BTC","EKO"));
  market_pair_list_.push_back(std::make_pair("BTC","QTUM"));
  market_pair_list_.push_back(std::make_pair("BTC","GXS"));
  market_pair_list_.push_back(std::make_pair("BTC","ABT"));
  market_pair_list_.push_back(std::make_pair("BTC","WAN"));
  market_pair_list_.push_back(std::make_pair("BTC","XLM"));
  market_pair_list_.push_back(std::make_pair("BTC","ITC"));
  market_pair_list_.push_back(std::make_pair("BTC","WTC"));
  market_pair_list_.push_back(std::make_pair("BTC","WICC"));
  market_pair_list_.push_back(std::make_pair("BTC","LINK"));
  market_pair_list_.push_back(std::make_pair("BTC","PAY"));
  market_pair_list_.push_back(std::make_pair("BTC","HSR"));
  market_pair_list_.push_back(std::make_pair("BTC","DGB"));
  market_pair_list_.push_back(std::make_pair("BTC","DCR"));
  market_pair_list_.push_back(std::make_pair("BTC","AIDOC"));
  market_pair_list_.push_back(std::make_pair("BTC","ZRX"));
  market_pair_list_.push_back(std::make_pair("BTC","MTN"));
  market_pair_list_.push_back(std::make_pair("BTC","WAX"));
  market_pair_list_.push_back(std::make_pair("BTC","KNC"));
  market_pair_list_.push_back(std::make_pair("BTC","SRN"));
  market_pair_list_.push_back(std::make_pair("BTC","CHAT"));
  market_pair_list_.push_back(std::make_pair("BTC","LBA"));
  market_pair_list_.push_back(std::make_pair("BTC","MEET"));
  market_pair_list_.push_back(std::make_pair("BTC","LET"));
  market_pair_list_.push_back(std::make_pair("BTC","POLY"));
  market_pair_list_.push_back(std::make_pair("BTC","MANA"));
  market_pair_list_.push_back(std::make_pair("BTC","MTX"));
  market_pair_list_.push_back(std::make_pair("BTC","ZLA"));
  market_pair_list_.push_back(std::make_pair("BTC","GNX"));
  market_pair_list_.push_back(std::make_pair("BTC","DBC"));
  market_pair_list_.push_back(std::make_pair("BTC","XEM"));
  market_pair_list_.push_back(std::make_pair("BTC","KAN"));
  market_pair_list_.push_back(std::make_pair("BTC","QSP"));
  market_pair_list_.push_back(std::make_pair("BTC","BTS"));
  market_pair_list_.push_back(std::make_pair("BTC","OST"));
  market_pair_list_.push_back(std::make_pair("BTC","QUN"));
  market_pair_list_.push_back(std::make_pair("BTC","GNT"));
  market_pair_list_.push_back(std::make_pair("BTC","RCN"));
  market_pair_list_.push_back(std::make_pair("BTC","RDN"));
  market_pair_list_.push_back(std::make_pair("BTC","XVG"));
  market_pair_list_.push_back(std::make_pair("BTC","BAT"));
  market_pair_list_.push_back(std::make_pair("BTC","MDS"));
  market_pair_list_.push_back(std::make_pair("BTC","DAT"));
  market_pair_list_.push_back(std::make_pair("BTC","EVX"));
  market_pair_list_.push_back(std::make_pair("BTC","MTL"));
  market_pair_list_.push_back(std::make_pair("BTC","REQ"));
  market_pair_list_.push_back(std::make_pair("BTC","BIX"));
  market_pair_list_.push_back(std::make_pair("BTC","POWR"));
  market_pair_list_.push_back(std::make_pair("BTC","AST"));
  market_pair_list_.push_back(std::make_pair("BTC","MCO"));
  market_pair_list_.push_back(std::make_pair("BTC","SNT"));
  market_pair_list_.push_back(std::make_pair("BTC","DGD"));
  market_pair_list_.push_back(std::make_pair("BTC","WPR"));
  market_pair_list_.push_back(std::make_pair("BTC","SMT"));
  market_pair_list_.push_back(std::make_pair("BTC","STORJ"));
  market_pair_list_.push_back(std::make_pair("BTC","CVC"));
  market_pair_list_.push_back(std::make_pair("BTC","LUN"));
  market_pair_list_.push_back(std::make_pair("BTC","RPX"));
  market_pair_list_.push_back(std::make_pair("BTC","TNT"));
  market_pair_list_.push_back(std::make_pair("BTC","TOPC"));
  market_pair_list_.push_back(std::make_pair("BTC","ENG"));
  market_pair_list_.push_back(std::make_pair("BTC","UTK"));
  market_pair_list_.push_back(std::make_pair("BTC","SNC"));
  market_pair_list_.push_back(std::make_pair("BTC","APPC"));
  market_pair_list_.push_back(std::make_pair("BTC","SALT"));
  market_pair_list_.push_back(std::make_pair("BTC","WAVES"));
  market_pair_list_.push_back(std::make_pair("BTC","ADX"));
  market_pair_list_.push_back(std::make_pair("BTC","SBTC"));
  market_pair_list_.push_back(std::make_pair("BTC","BCD"));
  market_pair_list_.push_back(std::make_pair("BTC","BCX"));
  market_pair_list_.push_back(std::make_pair("BTC","BIFI"));
  market_pair_list_.push_back(std::make_pair("BTC","BTG"));
return;

  market_pair_list_.push_back(std::make_pair("ETH","EOS"));
  market_pair_list_.push_back(std::make_pair("ETH","ADA"));
  market_pair_list_.push_back(std::make_pair("ETH","OMG"));
  market_pair_list_.push_back(std::make_pair("ETH","IOTA"));
  market_pair_list_.push_back(std::make_pair("ETH","XMR"));
  market_pair_list_.push_back(std::make_pair("ETH","STEEM"));
  market_pair_list_.push_back(std::make_pair("ETH","PAI"));
  market_pair_list_.push_back(std::make_pair("ETH","HIT"));
  market_pair_list_.push_back(std::make_pair("ETH","HT"));
  market_pair_list_.push_back(std::make_pair("ETH","ONT"));
  market_pair_list_.push_back(std::make_pair("ETH","XZC"));
  market_pair_list_.push_back(std::make_pair("ETH","ELA"));
  market_pair_list_.push_back(std::make_pair("ETH","IOST"));
  market_pair_list_.push_back(std::make_pair("ETH","CMT"));
  market_pair_list_.push_back(std::make_pair("ETH","CTXC"));
  market_pair_list_.push_back(std::make_pair("ETH","TRX"));
  market_pair_list_.push_back(std::make_pair("ETH","ZIL"));
  market_pair_list_.push_back(std::make_pair("ETH","BTM"));
  market_pair_list_.push_back(std::make_pair("ETH","SOC"));
  market_pair_list_.push_back(std::make_pair("ETH","NAS"));
  market_pair_list_.push_back(std::make_pair("ETH","THETA"));
  market_pair_list_.push_back(std::make_pair("ETH","VET"));
  market_pair_list_.push_back(std::make_pair("ETH","TNB"));
  market_pair_list_.push_back(std::make_pair("ETH","EDU"));
  market_pair_list_.push_back(std::make_pair("ETH","DTA"));
  market_pair_list_.push_back(std::make_pair("ETH","BFT"));
  market_pair_list_.push_back(std::make_pair("ETH","BOX"));
  market_pair_list_.push_back(std::make_pair("ETH","RUFF"));
  market_pair_list_.push_back(std::make_pair("ETH","ACT"));
  market_pair_list_.push_back(std::make_pair("ETH","YEE"));
  market_pair_list_.push_back(std::make_pair("ETH","ELF"));
  market_pair_list_.push_back(std::make_pair("ETH","GAS"));
  market_pair_list_.push_back(std::make_pair("ETH","ABT"));
  market_pair_list_.push_back(std::make_pair("ETH","GXS"));
  market_pair_list_.push_back(std::make_pair("ETH","BLZ"));
  market_pair_list_.push_back(std::make_pair("ETH","QASH"));
  market_pair_list_.push_back(std::make_pair("ETH","OCN"));
  market_pair_list_.push_back(std::make_pair("ETH","LBA"));
  market_pair_list_.push_back(std::make_pair("ETH","ICX"));
  market_pair_list_.push_back(std::make_pair("ETH","LSK"));
  market_pair_list_.push_back(std::make_pair("ETH","ITC"));
  market_pair_list_.push_back(std::make_pair("ETH","STK"));
  market_pair_list_.push_back(std::make_pair("ETH","SWFTC"));
  market_pair_list_.push_back(std::make_pair("ETH","EKO"));
  market_pair_list_.push_back(std::make_pair("ETH","WAN"));
  market_pair_list_.push_back(std::make_pair("ETH","WICC"));
  market_pair_list_.push_back(std::make_pair("ETH","PROPY"));
  market_pair_list_.push_back(std::make_pair("ETH","QTUM"));
  market_pair_list_.push_back(std::make_pair("ETH","MTN"));
  market_pair_list_.push_back(std::make_pair("ETH","GNX"));
  market_pair_list_.push_back(std::make_pair("ETH","PAY"));
  market_pair_list_.push_back(std::make_pair("ETH","XLM"));
  market_pair_list_.push_back(std::make_pair("ETH","KNC"));
  market_pair_list_.push_back(std::make_pair("ETH","AIDOC"));
  market_pair_list_.push_back(std::make_pair("ETH","WTC"));
  market_pair_list_.push_back(std::make_pair("ETH","LET"));
  market_pair_list_.push_back(std::make_pair("ETH","HSR"));
  market_pair_list_.push_back(std::make_pair("ETH","POLY"));
  market_pair_list_.push_back(std::make_pair("ETH","DBC"));
  market_pair_list_.push_back(std::make_pair("ETH","MTX"));
  market_pair_list_.push_back(std::make_pair("ETH","MANA"));
  market_pair_list_.push_back(std::make_pair("ETH","BTS"));
  market_pair_list_.push_back(std::make_pair("ETH","LINK"));
  market_pair_list_.push_back(std::make_pair("ETH","DGB"));
  market_pair_list_.push_back(std::make_pair("ETH","QSP"));
  market_pair_list_.push_back(std::make_pair("ETH","ZRX"));
  market_pair_list_.push_back(std::make_pair("ETH","WAX"));
  market_pair_list_.push_back(std::make_pair("ETH","MEET"));
  market_pair_list_.push_back(std::make_pair("ETH","MDS"));
  market_pair_list_.push_back(std::make_pair("ETH","BIX"));
  market_pair_list_.push_back(std::make_pair("ETH","DCR"));
  market_pair_list_.push_back(std::make_pair("ETH","ZLA"));
  market_pair_list_.push_back(std::make_pair("ETH","RDN"));
  market_pair_list_.push_back(std::make_pair("ETH","CHAT"));
  market_pair_list_.push_back(std::make_pair("ETH","EVX"));
  market_pair_list_.push_back(std::make_pair("ETH","SMT"));
  market_pair_list_.push_back(std::make_pair("ETH","DAT"));
  market_pair_list_.push_back(std::make_pair("ETH","KAN"));
  market_pair_list_.push_back(std::make_pair("ETH","SRN"));
  market_pair_list_.push_back(std::make_pair("ETH","QUN"));
  market_pair_list_.push_back(std::make_pair("ETH","GNT"));
  market_pair_list_.push_back(std::make_pair("ETH","RCN"));
  market_pair_list_.push_back(std::make_pair("ETH","TOPC"));
  market_pair_list_.push_back(std::make_pair("ETH","AST"));
  market_pair_list_.push_back(std::make_pair("ETH","POWR"));
  market_pair_list_.push_back(std::make_pair("ETH","XVG"));
  market_pair_list_.push_back(std::make_pair("ETH","REQ"));
  market_pair_list_.push_back(std::make_pair("ETH","BAT"));
  market_pair_list_.push_back(std::make_pair("ETH","CVC"));
  market_pair_list_.push_back(std::make_pair("ETH","APPC"));
  market_pair_list_.push_back(std::make_pair("ETH","OST"));
  market_pair_list_.push_back(std::make_pair("ETH","DGD"));
  market_pair_list_.push_back(std::make_pair("ETH","UTK"));
  market_pair_list_.push_back(std::make_pair("ETH","WPR"));
  market_pair_list_.push_back(std::make_pair("ETH","TNT"));
  market_pair_list_.push_back(std::make_pair("ETH","LUN"));
  market_pair_list_.push_back(std::make_pair("ETH","SALT"));
  market_pair_list_.push_back(std::make_pair("ETH","ENG"));
  market_pair_list_.push_back(std::make_pair("ETH","SNC"));
  market_pair_list_.push_back(std::make_pair("ETH","MCO"));
  market_pair_list_.push_back(std::make_pair("ETH","ADX"));
  market_pair_list_.push_back(std::make_pair("ETH","WAVES"));

  market_pair_list_.push_back(std::make_pair("HT","EOS"));
  market_pair_list_.push_back(std::make_pair("HT","XRP"));
  market_pair_list_.push_back(std::make_pair("HT","BCH"));
  market_pair_list_.push_back(std::make_pair("HT","ETC"));
  market_pair_list_.push_back(std::make_pair("HT","LTC"));
  market_pair_list_.push_back(std::make_pair("HT","DASH"));
  market_pair_list_.push_back(std::make_pair("HT","IOST"));
}

void HuobiMarket::SubScribeTradeDetail()
{
  QString str = "{\
                \"sub\": \"market.btcusdt.trade.detail\",\
                \"id\": \"asdfew2323aedfry\"\
              }";
  web_socket_.sendTextMessage(str);
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
  SubScribeMarketDepth();
}

void HuobiMarket::OnDepthDisConnected()
{
qDebug()<<"Depth disconnected!";
}

void HuobiMarket::OnDepthMsgReceived(const QByteArray &message)
{
  ;
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

void HuobiMarket::StartWatch(){
  web_socket_.open(QUrl("wss://api.huobi.pro/ws"));
}

std::list<std::pair<std::string, std::string> > HuobiMarket::GetMarketPair(){
  return market_pair_list_;
}

CoinInfo HuobiMarket::GetCoinInfo(std::pair<std::string, std::string> pair)
{
  return CoinInfo();
}
