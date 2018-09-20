#include "market_huobi.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/iostreams/filter/gzip.hpp>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filtering_stream.hpp>

#include "WebSocketMessage.h"
std::map<std::string, std::vector<std::string> > MarketHuobi::GetMarketPairMap(){
  return market_pair_map_;
}

void MarketHuobi::StartWatch(){
  //深度监控
  /*for(const std::pair<std::string, std::vector<std::string>>& item1:market_pair_map_){
    for(const std::string& item2:item1.second){
      std::string coin_base = item1.first;
      std::string coin_for = item2;
      std::string url = (boost::format("https://api.huobi.pro/market/depth?symbol=%s%s&type=step0")%boost::algorithm::to_lower_copy(coin_for)%boost::algorithm::to_lower_copy(coin_base=="USD"?"USDT":coin_base)).str();
      std::shared_ptr<AsioHttpsSocket> socket = asio_https_.CreateAsioHttpSocket();
      depth_socket_map_[coin_for+"_"+coin_base] = socket;
      ProxyConfig config;
      config.url_="127.0.0.1";
      config.port_ = 1080;
      socket->Process(url, config, boost::bind(&MarketHuobi::OnGetDepth, this, _1, _2, coin_for, coin_base));
    }
  }*//*
  //交易历史监控
  for(const std::pair<std::string, std::vector<std::string>>& item1:market_pair_map_){
    for(const std::string& item2:item1.second){
      std::string coin_base = item1.first;
      std::string coin_for = item2;
      std::string url = (boost::format("https://api.huobi.pro/market/history/trade?symbol=%s%s&size=2000")%boost::algorithm::to_lower_copy(coin_for)%boost::algorithm::to_lower_copy(coin_base=="USD"?"USDT":coin_base)).str();
      ProxyConfig config;
      config.url_="127.0.0.1";
      config.port_ = 1080;
      socket_map_[coin_for+"_"+coin_base]->Process(url, config, boost::bind(&MarketHuobi::OnGetTradeHistory, this, _1, _2, coin_for, coin_base));
    }
  }*/
  std::shared_ptr<AsioHttpsRequest> request = std::make_shared<AsioHttpsRequest>();
  request->config_.ssl_ = true;
  request->config_.use_proxy_ = true;
  request->config_.websocket_ = true;
  request->config_.proxy_.url_ = "127.0.0.1";
  request->config_.proxy_.port_ = 1080;
  request->head_.DeleteAllAttribute();
  request->head_.SetAttribute("host", "api.huobi.pro");
  request->head_.SetAttribute("Origin", "wss://api.huobi.pro");
  request->head_.SetAttribute("Upgrade", "websocket");
  request->head_.SetAttribute("Connection", "Upgrade");
  request->head_.SetAttribute("Sec-WebSocket-Key", "x3JJHMbDL1EzLkh9GBhXDw==");
  request->head_.SetAttribute("Sec-WebSocket-Version", "13");
  request->head_.url_ = "/ws";
  websocket_transaction_->Process(request, boost::bind(&MarketHuobi::OnGetTradeHistoryWebsocket, this, _1,_2, websocket_transaction_));
}



void MarketHuobi::OnGetDepth(std::shared_ptr<AsioHttpsRequest> request, std::shared_ptr<HttpResponseMsgStruct> responce,std::string coin_for, std::string coin_base){
  static std::mutex mutex;
  std::string str_data;
  mutex.lock();
  std::string url = (boost::format("https://api.huobi.pro/market/depth?symbol=%s%s&type=step0")%boost::algorithm::to_lower_copy(coin_for)%boost::algorithm::to_lower_copy(coin_base=="USD"?"USDT":coin_base)).str();
  if(responce->error_ !=""){
    std::cout<<"error"<<std::endl;
  }else{
    //str_data = responce->body_;
  }
  mutex.unlock();

  if(str_data != ""){
    //对深度信息进行处理
    DepthInfo depth_info;
    try{//进行json解析
      boost::property_tree::ptree pt;
      std::istringstream stream(str_data);
      boost::property_tree::read_json(stream, pt);
      //处理bids
      boost::property_tree::ptree child_bids = pt.get_child("tick.bids");
      for(boost::property_tree::ptree::value_type value:child_bids){
        std::vector<std::string> vec_str;
        for(boost::property_tree::ptree::value_type value_item: value.second){
          vec_str.push_back(value_item.second.get<std::string>(""));
        }
        if(vec_str.size() >= 2){
          depth_info.bids_.push_back(std::make_pair(vec_str[0], vec_str[1]));
        }
      }
      //处理asks
      boost::property_tree::ptree child_asks = pt.get_child("tick.asks");
      for(boost::property_tree::ptree::value_type value:child_asks){
        std::vector<std::string> vec_str;
        for(boost::property_tree::ptree::value_type value_item: value.second){
          vec_str.push_back(value_item.second.get<std::string>(""));
        }
        if(vec_str.size() >= 2){
          depth_info.asks_.push_back(std::make_pair(vec_str[0], vec_str[1]));
        }
      }
    }catch(...){//json解析失败
      std::cout<<"Huobi Depth info analyze error"<<std::endl;
    }
    {//修改深度信息
      boost::lock_guard<std::mutex> lk(coin_info_mutex_);
      depth_info.active_info_.interval_ = ((boost::posix_time::microsec_clock::local_time()-coin_info_[coin_for+"_"+coin_base].depth_info_.active_info_.last_time_)).total_milliseconds();
      depth_info.active_info_.last_time_ = boost::posix_time::microsec_clock::local_time();
      coin_info_[coin_for+"_"+coin_base].depth_info_ = depth_info;
      DoDepthChangeSig(coin_for, coin_base, depth_info);
    }
  }
  ProxyConfig config;
  config.url_="127.0.0.1";
  config.port_ = 1080;
  depth_socket_map_[coin_for+"_"+coin_base]->Process(url, config, boost::bind(&MarketHuobi::OnGetDepth, this, _1, _2, coin_for, coin_base));
}

void MarketHuobi::OnGetTradeHistory(std::shared_ptr<AsioHttpsRequest> request, std::shared_ptr<HttpResponseMsgStruct> responce, std::string coin_for, std::string coin_base){
  static std::mutex mutex;
  std::string str_data;
  mutex.lock();
  std::string url = (boost::format("https://api.huobi.pro/market/history/trade?symbol=%s%s&size=2000")%boost::algorithm::to_lower_copy(coin_for)%boost::algorithm::to_lower_copy(coin_base=="USD"?"USDT":coin_base)).str();
  if(responce->error_ !=""){
    std::cout<<"error"<<std::endl;
  }else{
    str_data = responce->body_;
  }
  mutex.unlock();

  if(str_data != ""){
    TradeHistory trade_history;
    try{
      boost::property_tree::ptree pt;
      std::istringstream stream(str_data);
      boost::property_tree::read_json(stream, pt);

      boost::property_tree::ptree child_pt = pt.get_child("data");
      for(boost::property_tree::ptree::value_type value:child_pt){
        for(boost::property_tree::ptree::value_type value_item: value.second.get_child("data")){
          TradeItem trade_item;
          trade_item.id_ = value_item.second.get<std::string>("id");
          trade_item.price_ = value_item.second.get<std::string>("price");
          trade_item.amount_ = value_item.second.get<std::string>("amount");
          trade_item.direction_ = value_item.second.get<std::string>("direction");
          trade_item.ts = value_item.second.get<std::string>("ts");
          trade_history.trade_list_.push_back(trade_item);
        }
      }
    }catch(...){//json 解析失败
      std::cout<<"Huobi trade history info analyze error:"<<str_data<<std::endl;
    }
    {
      std::lock_guard<std::mutex> lk(coin_info_mutex_);
      trade_history.active_info_.interval_ = ((boost::posix_time::microsec_clock::local_time()-coin_info_[coin_for+"_"+coin_base].last_trade_history_.active_info_.last_time_)).total_milliseconds();
      trade_history.active_info_.last_time_ = boost::posix_time::microsec_clock::local_time();
      coin_info_[coin_for+"_"+coin_base].last_trade_history_ = trade_history;
      DoTradeHistroyChangeSig(coin_for, coin_base, trade_history);
    }
  }
  ProxyConfig config;
  config.url_="127.0.0.1";
  config.port_ = 1080;
  transaction_socket_map_[coin_for+"_"+coin_base]->Process(url, config, boost::bind(&MarketHuobi::OnGetTradeHistory, this, _1, _2, coin_for, coin_base));
}
#include <boost/iostreams/copy.hpp>
#include <sstream>
void MarketHuobi::OnGetTradeHistoryWebsocket(std::shared_ptr<AsioHttpsRequest> request, std::shared_ptr<HttpResponseMsgStruct> responce, std::shared_ptr<AsioHttpsSocket> socket)
{
  if(responce->error_ == ""){
    std::cout<<"get response success!"<<request->head_.GetAttribute("host")<<std::endl;
    std::cout<<responce->str_ori_<<std::endl;
    trade_history_websocket_receive_ += responce->str_ori_;
    WebSocketMessage msg;
    if(msg.Decode(trade_history_websocket_receive_)){
      std::string str_unzip;
      {
        boost::iostreams::filtering_ostream fos;
        fos.push(boost::iostreams::gzip_decompressor());
        fos.push(boost::iostreams::back_inserter(str_unzip));
        fos << msg.body_;
        fos << std::flush;
        std::cout<<str_unzip<<std::endl;

        /*boost::algorithm::replace_all(str_unzip, "ping", "pong");
        std::string send_msg = WebSocketMessage::Encode(str_unzip);
        send_msg.insert(send_msg.end(), str_unzip.begin(), str_unzip.end());
        socket->SocketSend(send_msg);
        return;*/
      }
      std::string out = HandleWebSocketMessage(str_unzip);
      if(out.size()){


        std::string str_zip;
        /*{
          boost::iostreams::filtering_ostream fos;
          fos.push(boost::iostreams::gzip_compressor());
          fos.push(boost::iostreams::back_inserter(str_zip));
          fos << out;
          fos << std::flush;
          std::cout<<str_zip<<std::endl;
        }*/


        boost::algorithm::replace_all(out, "ping", "pong");
        std::cout<<"write:"<<out<<std::endl;
        str_zip = out;
        std::string send_msg = WebSocketMessage::Encode(str_zip);

        //send_msg.insert(send_msg.end(), str_zip.begin(), str_zip.end());
        socket->SocketSend(send_msg);
      }
    }
  }else{
    trade_history_websocket_receive_.clear();
    std::cout<<"get response faild:"<<request->head_.GetAttribute("host")<<responce->error_<<std::endl;
  }
}

DepthInfo MarketHuobi::GetCoinDepthInfo(const std::string &coin_for, const std::string &coin_base){
  std::lock_guard<std::mutex> lk(coin_info_mutex_);
  return coin_info_[coin_for+"_"+coin_base].depth_info_;
}

TradeHistory MarketHuobi::GetCoinTradeHistory(const std::string &coin_for, const std::string &coin_base){
  std::lock_guard<std::mutex> lk(coin_info_mutex_);
  return coin_info_[coin_for+"_"+coin_base].last_trade_history_;
}

boost::signals2::connection MarketHuobi::AddDepthChangeWatcher(boost::function<void (std::string, std::string, DepthInfo)> callback){
  return DoDepthChangeSig.connect(callback);
}

boost::signals2::connection MarketHuobi::AddTradeHistroyChangeWatcher(boost::function<void (std::string, std::string, TradeHistory)> callback){
  return DoTradeHistroyChangeSig.connect(callback);
}

std::string MarketHuobi::HandleWebSocketMessage(const std::string &str_in){
  boost::property_tree::ptree pt;
  std::istringstream istm(str_in);
  try{
    boost::property_tree::read_json(istm, pt);
    std::string rtn;
    if(HandleWebSocketPing(pt, rtn)){
      return rtn;
    }

  }catch(...){
    std::cout<<"json error"<<std::endl;
    return "";
  }
  return "";
}

bool MarketHuobi::HandleWebSocketPing(boost::property_tree::ptree &pt, std::string &str_out){
  try{
    std::string value = pt.get<std::string>("ping");
    std::cout<<"type of message is ping."<<std::endl;
    str_out = (boost::format("{\"pong\":%s}")%value).str();
    return true;
  }catch(...){
    return false;
  }
}

void MarketHuobi::InitMarketPair(){
  market_pair_map_["USD"].push_back("BTC");
  return;
  market_pair_map_["USD"].push_back("BCH");
  market_pair_map_["USD"].push_back("ETH");
  market_pair_map_["USD"].push_back("ETC");

  market_pair_map_["USD"].push_back("LTC");
  market_pair_map_["USD"].push_back("EOS");
  market_pair_map_["USD"].push_back("XRP");
  market_pair_map_["USD"].push_back("ADA");
  market_pair_map_["USD"].push_back("OMG");
  market_pair_map_["USD"].push_back("IOTA");
  market_pair_map_["USD"].push_back("DASH");
  market_pair_map_["USD"].push_back("ZEC");
  market_pair_map_["USD"].push_back("STEEM");
  market_pair_map_["USD"].push_back("HT");


  market_pair_map_["BTC"].push_back("BCH");
  market_pair_map_["BTC"].push_back("ETH");
  market_pair_map_["BTC"].push_back("LTC");
  market_pair_map_["BTC"].push_back("ETC");
  market_pair_map_["BTC"].push_back("EOS");
  market_pair_map_["BTC"].push_back("XRP");
  market_pair_map_["BTC"].push_back("ADA");
  market_pair_map_["BTC"].push_back("OMG");
  market_pair_map_["BTC"].push_back("IOTA");
  market_pair_map_["BTC"].push_back("DASH");
  market_pair_map_["BTC"].push_back("ZEC");
  market_pair_map_["BTC"].push_back("STEEM");
  market_pair_map_["BTC"].push_back("XMR");

  market_pair_map_["BTC"].push_back("HIT");
  market_pair_map_["BTC"].push_back("PAI");
  market_pair_map_["BTC"].push_back("HT");
  market_pair_map_["BTC"].push_back("ONT");
  market_pair_map_["BTC"].push_back("XZC");
  market_pair_map_["BTC"].push_back("ELA");
  market_pair_map_["BTC"].push_back("THETA");
  market_pair_map_["BTC"].push_back("BFT");
  market_pair_map_["BTC"].push_back("CTXC");
  market_pair_map_["BTC"].push_back("SOC");
  market_pair_map_["BTC"].push_back("TRX");
  market_pair_map_["BTC"].push_back("CMT");
  market_pair_map_["BTC"].push_back("IOST");
  market_pair_map_["BTC"].push_back("ZIL");
  market_pair_map_["BTC"].push_back("VET");
  market_pair_map_["BTC"].push_back("NAS");
  market_pair_map_["BTC"].push_back("BTM");
  market_pair_map_["BTC"].push_back("EDU");
  market_pair_map_["BTC"].push_back("TNB");
  market_pair_map_["BTC"].push_back("NEO");
  market_pair_map_["BTC"].push_back("STK");
  market_pair_map_["BTC"].push_back("DTA");
  market_pair_map_["BTC"].push_back("ACT");
  market_pair_map_["BTC"].push_back("RUFF");
  market_pair_map_["BTC"].push_back("OCN");
  market_pair_map_["BTC"].push_back("ICX");
  market_pair_map_["BTC"].push_back("PROPY");
  market_pair_map_["BTC"].push_back("GAS");
  market_pair_map_["BTC"].push_back("QASH");
  market_pair_map_["BTC"].push_back("BOX");
  market_pair_map_["BTC"].push_back("ELF");
  market_pair_map_["BTC"].push_back("YEE");
  market_pair_map_["BTC"].push_back("BLZ");
  market_pair_map_["BTC"].push_back("SWFTC");
  market_pair_map_["BTC"].push_back("LSK");
  market_pair_map_["BTC"].push_back("EKO");
  market_pair_map_["BTC"].push_back("QTUM");
  market_pair_map_["BTC"].push_back("GXS");
  market_pair_map_["BTC"].push_back("ABT");
  market_pair_map_["BTC"].push_back("WAN");
  market_pair_map_["BTC"].push_back("XLM");
  market_pair_map_["BTC"].push_back("ITC");
  market_pair_map_["BTC"].push_back("WTC");
  market_pair_map_["BTC"].push_back("WICC");
  market_pair_map_["BTC"].push_back("LINK");
  market_pair_map_["BTC"].push_back("PAY");
  market_pair_map_["BTC"].push_back("HSR");
  market_pair_map_["BTC"].push_back("DGB");
  market_pair_map_["BTC"].push_back("DCR");
  market_pair_map_["BTC"].push_back("AIDOC");
  market_pair_map_["BTC"].push_back("ZRX");
  market_pair_map_["BTC"].push_back("MTN");
  market_pair_map_["BTC"].push_back("WAX");
  market_pair_map_["BTC"].push_back("KNC");
  market_pair_map_["BTC"].push_back("SRN");
  market_pair_map_["BTC"].push_back("CHAT");
  market_pair_map_["BTC"].push_back("LBA");
  market_pair_map_["BTC"].push_back("MEET");
  market_pair_map_["BTC"].push_back("LET");
  market_pair_map_["BTC"].push_back("POLY");
  market_pair_map_["BTC"].push_back("MANA");
  market_pair_map_["BTC"].push_back("MTX");
  market_pair_map_["BTC"].push_back("ZLA");
  market_pair_map_["BTC"].push_back("GNX");
  market_pair_map_["BTC"].push_back("DBC");
  market_pair_map_["BTC"].push_back("XEM");
  market_pair_map_["BTC"].push_back("KAN");
  market_pair_map_["BTC"].push_back("QSP");
  market_pair_map_["BTC"].push_back("BTS");
  market_pair_map_["BTC"].push_back("OST");
  market_pair_map_["BTC"].push_back("QUN");
  market_pair_map_["BTC"].push_back("GNT");
  market_pair_map_["BTC"].push_back("RCN");
  market_pair_map_["BTC"].push_back("RDN");
  market_pair_map_["BTC"].push_back("XVG");
  market_pair_map_["BTC"].push_back("BAT");
  market_pair_map_["BTC"].push_back("MDS");
  market_pair_map_["BTC"].push_back("DAT");
  market_pair_map_["BTC"].push_back("EVX");
  market_pair_map_["BTC"].push_back("MTL");
  market_pair_map_["BTC"].push_back("REQ");
  market_pair_map_["BTC"].push_back("BIX");
  market_pair_map_["BTC"].push_back("POWR");
  market_pair_map_["BTC"].push_back("AST");
  market_pair_map_["BTC"].push_back("MCO");
  market_pair_map_["BTC"].push_back("SNT");
  market_pair_map_["BTC"].push_back("DGD");
  market_pair_map_["BTC"].push_back("WPR");
  market_pair_map_["BTC"].push_back("SMT");
  market_pair_map_["BTC"].push_back("STORJ");
  market_pair_map_["BTC"].push_back("CVC");
  market_pair_map_["BTC"].push_back("LUN");
  market_pair_map_["BTC"].push_back("RPX");
  market_pair_map_["BTC"].push_back("TNT");
  market_pair_map_["BTC"].push_back("TOPC");
  market_pair_map_["BTC"].push_back("ENG");
  market_pair_map_["BTC"].push_back("UTK");
  market_pair_map_["BTC"].push_back("SNC");
  market_pair_map_["BTC"].push_back("APPC");
  market_pair_map_["BTC"].push_back("SALT");
  market_pair_map_["BTC"].push_back("WAVES");
  market_pair_map_["BTC"].push_back("ADX");
  market_pair_map_["BTC"].push_back("SBTC");
  market_pair_map_["BTC"].push_back("BCD");
  market_pair_map_["BTC"].push_back("BCX");
  market_pair_map_["BTC"].push_back("BIFI");
  market_pair_map_["BTC"].push_back("BTG");


  market_pair_map_["ETH"].push_back("EOS");
  market_pair_map_["ETH"].push_back("ADA");
  market_pair_map_["ETH"].push_back("OMG");
  market_pair_map_["ETH"].push_back("IOTA");
  market_pair_map_["ETH"].push_back("XMR");
  market_pair_map_["ETH"].push_back("STEEM");
  market_pair_map_["ETH"].push_back("PAI");
  market_pair_map_["ETH"].push_back("HIT");
  market_pair_map_["ETH"].push_back("HT");
  market_pair_map_["ETH"].push_back("ONT");
  market_pair_map_["ETH"].push_back("XZC");
  market_pair_map_["ETH"].push_back("ELA");
  market_pair_map_["ETH"].push_back("IOST");
  market_pair_map_["ETH"].push_back("CMT");
  market_pair_map_["ETH"].push_back("CTXC");
  market_pair_map_["ETH"].push_back("TRX");
  market_pair_map_["ETH"].push_back("ZIL");
  market_pair_map_["ETH"].push_back("BTM");
  market_pair_map_["ETH"].push_back("SOC");
  market_pair_map_["ETH"].push_back("NAS");
  market_pair_map_["ETH"].push_back("THETA");
  market_pair_map_["ETH"].push_back("VET");
  market_pair_map_["ETH"].push_back("TNB");
  market_pair_map_["ETH"].push_back("EDU");
  market_pair_map_["ETH"].push_back("DTA");
  market_pair_map_["ETH"].push_back("BFT");
  market_pair_map_["ETH"].push_back("BOX");
  market_pair_map_["ETH"].push_back("RUFF");
  market_pair_map_["ETH"].push_back("ACT");
  market_pair_map_["ETH"].push_back("YEE");
  market_pair_map_["ETH"].push_back("ELF");
  market_pair_map_["ETH"].push_back("GAS");
  market_pair_map_["ETH"].push_back("ABT");
  market_pair_map_["ETH"].push_back("GXS");
  market_pair_map_["ETH"].push_back("BLZ");
  market_pair_map_["ETH"].push_back("QASH");
  market_pair_map_["ETH"].push_back("OCN");
  market_pair_map_["ETH"].push_back("LBA");
  market_pair_map_["ETH"].push_back("ICX");
  market_pair_map_["ETH"].push_back("LSK");
  market_pair_map_["ETH"].push_back("ITC");
  market_pair_map_["ETH"].push_back("STK");
  market_pair_map_["ETH"].push_back("SWFTC");
  market_pair_map_["ETH"].push_back("EKO");
  market_pair_map_["ETH"].push_back("WAN");
  market_pair_map_["ETH"].push_back("WICC");
  market_pair_map_["ETH"].push_back("PROPY");
  market_pair_map_["ETH"].push_back("QTUM");
  market_pair_map_["ETH"].push_back("MTN");
  market_pair_map_["ETH"].push_back("GNX");
  market_pair_map_["ETH"].push_back("PAY");
  market_pair_map_["ETH"].push_back("XLM");
  market_pair_map_["ETH"].push_back("KNC");
  market_pair_map_["ETH"].push_back("AIDOC");
  market_pair_map_["ETH"].push_back("WTC");
  market_pair_map_["ETH"].push_back("LET");
  market_pair_map_["ETH"].push_back("HSR");
  market_pair_map_["ETH"].push_back("POLY");
  market_pair_map_["ETH"].push_back("DBC");
  market_pair_map_["ETH"].push_back("MTX");
  market_pair_map_["ETH"].push_back("MANA");
  market_pair_map_["ETH"].push_back("BTS");
  market_pair_map_["ETH"].push_back("LINK");
  market_pair_map_["ETH"].push_back("DGB");
  market_pair_map_["ETH"].push_back("QSP");
  market_pair_map_["ETH"].push_back("ZRX");
  market_pair_map_["ETH"].push_back("WAX");
  market_pair_map_["ETH"].push_back("MEET");
  market_pair_map_["ETH"].push_back("MDS");
  market_pair_map_["ETH"].push_back("BIX");
  market_pair_map_["ETH"].push_back("DCR");
  market_pair_map_["ETH"].push_back("ZLA");
  market_pair_map_["ETH"].push_back("RDN");
  market_pair_map_["ETH"].push_back("CHAT");
  market_pair_map_["ETH"].push_back("EVX");
  market_pair_map_["ETH"].push_back("SMT");
  market_pair_map_["ETH"].push_back("DAT");
  market_pair_map_["ETH"].push_back("KAN");
  market_pair_map_["ETH"].push_back("SRN");
  market_pair_map_["ETH"].push_back("QUN");
  market_pair_map_["ETH"].push_back("GNT");
  market_pair_map_["ETH"].push_back("RCN");
  market_pair_map_["ETH"].push_back("TOPC");
  market_pair_map_["ETH"].push_back("AST");
  market_pair_map_["ETH"].push_back("POWR");
  market_pair_map_["ETH"].push_back("XVG");
  market_pair_map_["ETH"].push_back("REQ");
  market_pair_map_["ETH"].push_back("BAT");
  market_pair_map_["ETH"].push_back("CVC");
  market_pair_map_["ETH"].push_back("APPC");
  market_pair_map_["ETH"].push_back("OST");
  market_pair_map_["ETH"].push_back("DGD");
  market_pair_map_["ETH"].push_back("UTK");
  market_pair_map_["ETH"].push_back("WPR");
  market_pair_map_["ETH"].push_back("TNT");
  market_pair_map_["ETH"].push_back("LUN");
  market_pair_map_["ETH"].push_back("SALT");
  market_pair_map_["ETH"].push_back("ENG");
  market_pair_map_["ETH"].push_back("SNC");
  market_pair_map_["ETH"].push_back("MCO");
  market_pair_map_["ETH"].push_back("ADX");
  market_pair_map_["ETH"].push_back("WAVES");

  market_pair_map_["HT"].push_back("EOS");
  market_pair_map_["HT"].push_back("XRP");
  market_pair_map_["HT"].push_back("BCH");
  market_pair_map_["HT"].push_back("ETC");
  market_pair_map_["HT"].push_back("LTC");
  market_pair_map_["HT"].push_back("DASH");
  market_pair_map_["HT"].push_back("IOST");

}

MarketHuobi::MarketHuobi():asio_https_(4){
  InitMarketPair();
  //depth_socket_ = asio_https_.CreateAsioHttpSocket();
  websocket_transaction_ = asio_https_.CreateAsioHttpSocket();
}
