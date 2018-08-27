#include "market_interface.h"
#include "market_huobi.h"
#include "market_binance.h"
#include "market_bitfinex.h"
#include "market_ok.h"

float CoinInfo::price_now(){
  return price_now_;
}

void CoinInfo::set_price_now(float price){
  price_now_ = price;
}


std::vector<std::pair<std::string, MarketInterface::MarketType>> MarketInterface::market_list ={

};


std::shared_ptr<MarketInterface> MarketInterface::CreateMarket(MarketInterface::MarketType market_type){
  std::shared_ptr<MarketInterface> rtn;
  switch(market_type){
  case MT_HUOBI:{
      rtn = std::make_shared<MarketHuobi>();
      break;
    }
  case MT_OK:{
      rtn = std::make_shared<MarketOk>();
      break;
    }
  case MT_BINANCE:{
      rtn = std::make_shared<MarketBinance>();
      break;
    }
  case MT_BITFINEX:{
      rtn = std::make_shared<MarketBitfinex>();
      break;
    }
  }
  return rtn;
}

const std::vector<std::pair<std::string, MarketInterface::MarketType> > MarketInterface::GetMarketList(){
  return market_list;
}

MarketInterface::MarketInterface(){
  /*market_type.push_back();
  market_type.push_back();
  market_type.push_back(std::make_pair("binance", MT_BINANCE));
  market_type.push_back(std::make_pair("bitfinex", MT_BITFINEX));*/
}

