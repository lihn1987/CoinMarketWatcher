#include "QuantitativeTransaction.h"


void QuantitativeTransactionItem::Init(QuantitativeComputeType type, std::vector<std::string> &param_list){
  compute_type_ = type;
  param_list_ = param_list;
}

std::set<std::string> QuantitativeTransactionItem::GetCoinsymbolSet(){
  return coin_symbol_;
}

void QuantitativeTransactionItem::SetCoinSymbolSet(const std::set<std::string> &symbol_set){
  coin_symbol_ = symbol_set;
}

QuantitativeComputeType QuantitativeTransactionItem::GetComputeType(){
  return compute_type_;
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
    case QuantitativeComputeType::QC_DepthBuySellMargin:
      return ComputeBuySellMargin(coin_symbol, info);
      break;
    case QuantitativeComputeType::QC_DepthBuyCount:
      return ComputeBuyCount(coin_symbol, info);
      break;
    case QuantitativeComputeType::QC_DepthSellCount:
      return ComputeSellCount(coin_symbol, info);
      break;
    case QuantitativeComputeType::QC_PROFIT_AND_LOSS:
      return ComputeProfitAndLoss(coin_symbol, info);
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

bool QuantitativeTransactionItem::ComputeBuySellMargin(const std::string &coin_symbol, CoinInfo &info)
{
//参数描述  卖盘最低价/买盘最高价 0:>(1) <(2) 1:value
  if(!info.depth_info_[coin_symbol].asks_.size() ||
     !info.depth_info_[coin_symbol].bids_.size()||
     param_list_.size() < 2){
    coin_symbol_.erase(coin_symbol);
    return false;
  }
  double value_from = atof(info.depth_info_[coin_symbol].asks_.front().first.c_str())/
      atof(info.depth_info_[coin_symbol].bids_.front().first.c_str()) -1.0f;
  double value_to = atof(param_list_[1].c_str())/100.0f;
  if((param_list_[0] == "1" && value_from > value_to)||
     (param_list_[0] == "2" && value_from < value_to)){
    coin_symbol_.insert(coin_symbol);
    return true;
  }
  coin_symbol_.erase(coin_symbol);
  return false;
}

bool QuantitativeTransactionItem::ComputeBuyCount(const std::string &coin_symbol, CoinInfo &info){
  //参数描述 0:相对于卖盘买盘的差价百分比   买完需要的比特币个数 1:>(1) <(2)  2:value
  if(param_list_.size() < 3){
    coin_symbol_.erase(coin_symbol);
    return false;
  }
  double max_price = QString::fromStdString(info.depth_info_[coin_symbol].bids_.front().first).toDouble()*(1+QString::fromStdString(param_list_[0].c_str()).toDouble()/100.0f);
  double btc_arm_count = atof(param_list_[2].c_str());
  double btc_real_count = 0;
  for(std::pair<std::string/*价格*/, std::string/*数量*/> item:info.depth_info_[coin_symbol].asks_){
    if(QString::fromStdString(item.first).toDouble() <= max_price){
      btc_real_count+=QString::fromStdString(item.second).toLower().toDouble()*QString::fromStdString(item.first).toDouble();
      qDebug()<<QString::fromStdString(item.second).toLower().toDouble();
    }
  }
  if(param_list_[1] == "1" && btc_real_count > btc_arm_count){
    coin_symbol_.insert(coin_symbol);
    return true;
  }else if(param_list_[1] == "2" && btc_arm_count < btc_real_count){
    coin_symbol_.insert(coin_symbol);
    return true;
  }
  coin_symbol_.erase(coin_symbol);
  return false;
}

bool QuantitativeTransactionItem::ComputeSellCount(const std::string &coin_symbol, CoinInfo &info)
{
  //参数描述 0:相对于卖盘买盘的差价百分比   买完需要的比特币个数 1:>(1) <(2)  2:value
  if(param_list_.size() < 3){
    coin_symbol_.erase(coin_symbol);
    return false;
  }
  double min_price = QString::fromStdString(info.depth_info_[coin_symbol].asks_.front().first).toDouble()*(1-QString::fromStdString(param_list_[0].c_str()).toDouble()/100.0f);
  double btc_arm_count = atof(param_list_[2].c_str());
  double btc_real_count = 0;
  for(std::pair<std::string/*价格*/, std::string/*数量*/> item:info.depth_info_[coin_symbol].bids_){
    if(QString::fromStdString(item.first).toDouble() >= min_price){
      btc_real_count+=QString::fromStdString(item.second).toLower().toDouble()*QString::fromStdString(item.first).toDouble();
      //qDebug()<<QString::fromStdString(item.second).toLower().toDouble();
    }
  }
  if(param_list_[1] == "1" && btc_real_count > btc_arm_count){
    coin_symbol_.insert(coin_symbol);
    return true;
  }else if(param_list_[1] == "2" && btc_arm_count < btc_real_count){
    coin_symbol_.insert(coin_symbol);
    return true;
  }
  coin_symbol_.erase(coin_symbol);
  return false;
}

bool QuantitativeTransactionItem::ComputeProfitAndLoss(const std::string &coin_symbol, CoinInfo &info){
  //参数描述 0:利润大于 1亏损小于
  if(param_list_.size() < 2)return false;
  double base_price = 0;
  double now_price = atof(info.trade_list_[coin_symbol].back().price_.c_str());
  for(auto iter = info.trade_history_.rbegin(); iter != info.trade_history_.rend(); iter++){
    if(iter->coin_==coin_symbol && iter->buy_ == true){
      base_price = iter->price_;
      break;
    }
  }
  if(base_price == 0)return false;

  if(now_price > base_price && (now_price - base_price)/base_price*100 > atof(param_list_[0].c_str())){
    return true;
  }
  else if(now_price < base_price && (base_price - now_price)/base_price*100 > atof(param_list_[1].c_str())){
    return true;
  }
  else{
    return false;
  }
}
