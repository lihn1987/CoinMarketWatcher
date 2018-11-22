#include "QuantitativeTransaction.h"


void QuantitativeTransactionItem::Init(QuantitativeComputeType type, std::vector<std::string> &param_list){
  compute_type_ = type;
  param_list_ = param_list;
}

std::set<std::string> QuantitativeTransactionItem::GetCoinsymbolSet(){
  return coin_symbol_;
}

void QuantitativeTransactionItem::Removesymbol(const std::string &symbol){
  coin_symbol_.erase(symbol);
}

bool QuantitativeTransactionItem::Compute(const std::string &coin_symbol, CoinInfo &info){
  switch(compute_type_){
    case QuantitativeComputeType::QC_TransactionValueSum:
      return ComputeValueSum(coin_symbol, info);
      break;
    case QuantitativeComputeType::QC_TransactionTimeSum:
      return ComputeTimeSum(coin_symbol, info);
      break;
    case QuantitativeComputeType::QC_TransactionBuySellValueScale:
      return ComputeBuySellValueScale(coin_symbol, info);
      break;
    case QuantitativeComputeType::QC_TransactionBuySellTimeScale:
      return ComputeBuySellTimeScale(coin_symbol, info);
      break;
    default:
      return false;
  }
}

bool QuantitativeTransactionItem::ComputeValueSum(const std::string &coin_symbol, CoinInfo &info){
  if(!boost::algorithm::ends_with(coin_symbol, "btc"))return false;
  if((atoi(param_list_[2].c_str()) == 1 && param_list_.size() < 4)||
     (atoi(param_list_[2].c_str()) == 2 && param_list_.size() < 5))
    return false;
  double value_count = 0;
  double pravious_count = 0;
  for(auto iter = info.trade_list_[coin_symbol].rbegin(); iter != info.trade_list_[coin_symbol].rend(); iter++){
    QDateTime time = QDateTime::currentDateTime();
    if(time.toMSecsSinceEpoch() - atoll(iter->ts_.data()) < 1000*atol(param_list_[0].c_str())){
      value_count+=atof(iter->amount_.c_str())*atof(iter->price_.c_str());
    }else{
      break;
    }
  }
  for(auto iter = info.trade_list_[coin_symbol].rbegin(); iter != info.trade_list_[coin_symbol].rend(); iter++){
    QDateTime time = QDateTime::currentDateTime();
    if(time.toMSecsSinceEpoch() - atoll(iter->ts_.data()) < 1000*atol(param_list_[3].c_str())){
      pravious_count+=atof(iter->amount_.c_str())*atof(iter->price_.c_str());
    }else{
      break;
    }
  }
  if(atoi(param_list_[2].c_str()) == 1){
    if(atoi(param_list_[1].c_str()) == 1 && value_count > atof(param_list_[3].c_str())){
      coin_symbol_.insert(coin_symbol);
      return true;
    }else if(atoi(param_list_[1].c_str()) == 2 && value_count < atof(param_list_[3].c_str())){
      coin_symbol_.insert(coin_symbol);
      return true;
    }
  }else if(atoi(param_list_[2].c_str()) == 2){
    if(atoi(param_list_[1].c_str()) == 1 && (value_count/atoi(param_list_[0].c_str()))/(pravious_count/atoi(param_list_[3].c_str()))*100 > atof(param_list_[4].c_str())){
      coin_symbol_.insert(coin_symbol);
      return true;
    }else if(atoi(param_list_[1].c_str()) == 2 && (value_count/atoi(param_list_[0].c_str()))/(pravious_count/atoi(param_list_[3].c_str()))*100 < atof(param_list_[4].c_str())){
      coin_symbol_.insert(coin_symbol);
      return true;
    }
  }
  coin_symbol_.erase(coin_symbol);
  return false;
}

bool QuantitativeTransactionItem::ComputeTimeSum(const std::string &coin_symbol, CoinInfo &info){
  if(!boost::algorithm::ends_with(coin_symbol, "btc"))return false;
  if((atoi(param_list_[2].c_str()) == 1 && param_list_.size() < 4)||
     (atoi(param_list_[2].c_str()) == 2 && param_list_.size() < 5))
    return false;
  double value_count = 0;
  double pravious_count = 0;
  for(auto iter = info.trade_list_[coin_symbol].rbegin(); iter != info.trade_list_[coin_symbol].rend(); iter++){
    QDateTime time = QDateTime::currentDateTime();
    if(time.toMSecsSinceEpoch() - atoll(iter->ts_.data()) < 1000*atol(param_list_[0].c_str())){
      value_count+=1;
    }else{
      break;
    }
  }
  for(auto iter = info.trade_list_[coin_symbol].rbegin(); iter != info.trade_list_[coin_symbol].rend(); iter++){
    QDateTime time = QDateTime::currentDateTime();
    if(time.toMSecsSinceEpoch() - atoll(iter->ts_.data()) < 1000*atol(param_list_[3].c_str())){
      pravious_count+=1;
    }else{
      break;
    }
  }
  if(atoi(param_list_[2].c_str()) == 1){
    if(atoi(param_list_[1].c_str()) == 1 && value_count > atof(param_list_[3].c_str())){
      coin_symbol_.insert(coin_symbol);
      return true;
    }else if(atoi(param_list_[1].c_str()) == 2 && value_count < atof(param_list_[3].c_str())){
      coin_symbol_.insert(coin_symbol);
      return true;
    }
  }else if(atoi(param_list_[2].c_str()) == 2){
    if(atoi(param_list_[1].c_str()) == 1 && (value_count/atoi(param_list_[0].c_str()))/(pravious_count/atoi(param_list_[3].c_str()))*100 > atof(param_list_[4].c_str())){
      coin_symbol_.insert(coin_symbol);
      return true;
    }else if(atoi(param_list_[1].c_str()) == 2 && (value_count/atoi(param_list_[0].c_str()))/(pravious_count/atoi(param_list_[3].c_str()))*100 < atof(param_list_[4].c_str())){
      coin_symbol_.insert(coin_symbol);
      return true;
    }
  }
  coin_symbol_.erase(coin_symbol);
  return false;
}

bool QuantitativeTransactionItem::ComputeBuySellValueScale(const std::string &coin_symbol, CoinInfo &info){
  if(!boost::algorithm::ends_with(coin_symbol, "btc"))return false;
  if((atoi(param_list_[2].c_str()) == 1 && param_list_.size() < 4)||
     (atoi(param_list_[2].c_str()) == 2 && param_list_.size() < 5))
    return false;
  double buy_count = 0;
  double sell_count = 0;
  double pravious_buy_count = 0;
  double pravious_sell_count = 0;
  for(auto iter = info.trade_list_[coin_symbol].rbegin(); iter != info.trade_list_[coin_symbol].rend(); iter++){
    QDateTime time = QDateTime::currentDateTime();
    if(time.toMSecsSinceEpoch() - atoll(iter->ts_.data()) < 1000*atol(param_list_[0].c_str())){
      if(iter->direction_ == "sell"){
        sell_count += atof(iter->amount_.c_str());
      }else{
        buy_count += atof(iter->amount_.c_str());
      }
    }else{
      break;
    }
  }
  for(auto iter = info.trade_list_[coin_symbol].rbegin(); iter != info.trade_list_[coin_symbol].rend(); iter++){
    QDateTime time = QDateTime::currentDateTime();
    if(time.toMSecsSinceEpoch() - atoll(iter->ts_.data()) < 1000*atol(param_list_[3].c_str())){
      if(iter->direction_ == "sell"){
        pravious_sell_count += atof(iter->amount_.c_str());
      }else{
        pravious_buy_count += atof(iter->amount_.c_str());
      }
    }else{
      break;
    }
  }
  double buy_sell_scale = buy_count/(sell_count == 0? 0.00000001:sell_count);
  double praviouse_buy_sell_scale = pravious_buy_count/(pravious_sell_count == 0? 0.00000001:pravious_sell_count);
  if(atoi(param_list_[2].c_str()) == 1){
    if(atoi(param_list_[1].c_str()) == 1 && buy_sell_scale > atof(param_list_[3].c_str())){
      coin_symbol_.insert(coin_symbol);
      return true;
    }else if(atoi(param_list_[1].c_str()) == 2 && buy_sell_scale < atof(param_list_[3].c_str())){
      coin_symbol_.insert(coin_symbol);
      return true;
    }
  }else if(atoi(param_list_[2].c_str()) == 2){
    if(atoi(param_list_[1].c_str()) == 1 && buy_sell_scale/(praviouse_buy_sell_scale==0?0.000000001:praviouse_buy_sell_scale)*100 > atof(param_list_[4].c_str())){
      coin_symbol_.insert(coin_symbol);
      return true;
    }else if(atoi(param_list_[1].c_str()) == 2 && buy_sell_scale/(praviouse_buy_sell_scale==0?0.000000001:praviouse_buy_sell_scale)*100 < atof(param_list_[4].c_str())){
      coin_symbol_.insert(coin_symbol);
      return true;
    }
  }
  coin_symbol_.erase(coin_symbol);
  return false;
}

bool QuantitativeTransactionItem::ComputeBuySellTimeScale(const std::string &coin_symbol, CoinInfo &info){
  if(!boost::algorithm::ends_with(coin_symbol, "btc"))return false;
  if((atoi(param_list_[2].c_str()) == 1 && param_list_.size() < 4)||
     (atoi(param_list_[2].c_str()) == 2 && param_list_.size() < 5))
    return false;
  double buy_count = 0;
  double sell_count = 0;
  double pravious_buy_count = 0;
  double pravious_sell_count = 0;
  for(auto iter = info.trade_list_[coin_symbol].rbegin(); iter != info.trade_list_[coin_symbol].rend(); iter++){
    QDateTime time = QDateTime::currentDateTime();
    if(time.toMSecsSinceEpoch() - atoll(iter->ts_.data()) < 1000*atol(param_list_[0].c_str())){
      if(iter->direction_ == "sell"){
        sell_count += 1;
      }else{
        buy_count += 1;
      }
    }else{
      break;
    }
  }
  for(auto iter = info.trade_list_[coin_symbol].rbegin(); iter != info.trade_list_[coin_symbol].rend(); iter++){
    QDateTime time = QDateTime::currentDateTime();
    if(time.toMSecsSinceEpoch() - atoll(iter->ts_.data()) < 1000*atol(param_list_[3].c_str())){
      if(iter->direction_ == "sell"){
        pravious_sell_count += 1;
      }else{
        pravious_buy_count += 1;
      }
    }else{
      break;
    }
  }
  double buy_sell_scale = buy_count/(sell_count == 0? 0.00000001:sell_count);
  double praviouse_buy_sell_scale = pravious_buy_count/(pravious_sell_count == 0? 0.00000001:pravious_sell_count);
  if(atoi(param_list_[2].c_str()) == 1){
    if(atoi(param_list_[1].c_str()) == 1 && buy_sell_scale > atof(param_list_[3].c_str())){
      coin_symbol_.insert(coin_symbol);
      return true;
    }else if(atoi(param_list_[1].c_str()) == 2 && buy_sell_scale < atof(param_list_[3].c_str())){
      coin_symbol_.insert(coin_symbol);
      return true;
    }
  }else if(atoi(param_list_[2].c_str()) == 2){
    if(atoi(param_list_[1].c_str()) == 1 && buy_sell_scale/(praviouse_buy_sell_scale==0?0.000000001:praviouse_buy_sell_scale)*100 > atof(param_list_[4].c_str())){
      coin_symbol_.insert(coin_symbol);
      return true;
    }else if(atoi(param_list_[1].c_str()) == 2 && buy_sell_scale/(praviouse_buy_sell_scale==0?0.000000001:praviouse_buy_sell_scale)*100 < atof(param_list_[4].c_str())){
      coin_symbol_.insert(coin_symbol);
      return true;
    }
  }
  coin_symbol_.erase(coin_symbol);
  return false;
}
