#ifndef QUANTITATIVETRANSACTION_H
#define QUANTITATIVETRANSACTION_H
#include <list>
#include <string>
#include <set>
#include "MarketInterface.h"
enum QuantitativeComputeType{
  QC_INVALIDATE = -1,
  QC_TransactionValueSum = 0,
  QC_TransactionTimeSum = 1
};

class QuantitativeTransactionItem{
private:
  QuantitativeComputeType compute_type_=QC_INVALIDATE;
  /*
  0:time_second 1:>(1) <(2)  2:now(1)      3 value
                             2:pravious(2) 3 second  4 persent
  */
  std::vector<std::string> param_list_;
  std::set<std::string> coin_symble_;
public:
  void Init(QuantitativeComputeType type, std::vector<std::string>& param_list){
    compute_type_ = type;
    param_list_ = param_list;
  }
  bool Compute(const std::string &coin_symble, CoinInfo& info){
    switch(compute_type_){
      case QuantitativeComputeType::QC_TransactionValueSum:
        return ComputeValueSum(coin_symble, info);
        break;
      case QuantitativeComputeType::QC_TransactionTimeSum:
        return ComputeTimeSum(coin_symble, info);
        break;
      default:
        return false;
    }
  }
  bool ComputeValueSum(const std::string &coin_symble, CoinInfo& info){
    if((atoi(param_list_[2].c_str()) == 1 && param_list_.size() < 4)||
       (atoi(param_list_[2].c_str()) == 2 && param_list_.size() < 5))
      return false;
    double value_count = 0;
    double pravious_count = 0;
    for(auto iter = info.trade_list_[coin_symble].rbegin(); iter != info.trade_list_[coin_symble].rend(); iter++){
      QDateTime time = QDateTime::currentDateTime();
      if(atoll(iter->ts_.data()) - time.toMSecsSinceEpoch() < 1000*atol(param_list_[0].c_str())){
        value_count+=atof(iter->amount_.c_str());
      }else{
        break;
      }
    }
    for(auto iter = info.trade_list_[coin_symble].rbegin(); iter != info.trade_list_[coin_symble].rend(); iter++){
      QDateTime time = QDateTime::currentDateTime();
      if(atoll(iter->ts_.data()) - time.toMSecsSinceEpoch() < 1000*atol(param_list_[3].c_str())){
        pravious_count+=atof(iter->amount_.c_str());
      }else{
        break;
      }
    }
    if(atoi(param_list_[2].c_str()) == 1){
      if(atoi(param_list_[1].c_str()) == 1 && value_count > atof(param_list_[3].c_str())){
        return true;
      }else if(atoi(param_list_[1].c_str()) == 2 && value_count < atof(param_list_[3].c_str())){
        return true;
      }
    }else if(atoi(param_list_[2].c_str()) == 2){
      if(atoi(param_list_[1].c_str()) == 1 && (value_count/atoi(param_list_[0].c_str()))/(pravious_count/atoi(param_list_[3].c_str()))/100 < atof(param_list_[4].c_str())){
        return true;
      }else if(atoi(param_list_[1].c_str()) == 2 && (value_count/atoi(param_list_[0].c_str()))/(pravious_count/atoi(param_list_[3].c_str()))/100 > atof(param_list_[4].c_str())){
        return true;
      }
    }
    return false;
  }
  bool ComputeTimeSum(const std::string &coin_symble, CoinInfo& info){
    if((atoi(param_list_[2].c_str()) == 1 && param_list_.size() < 4)||
       (atoi(param_list_[2].c_str()) == 2 && param_list_.size() < 5))
      return false;
    double value_count = 0;
    double pravious_count = 0;
    for(auto iter = info.trade_list_[coin_symble].rbegin(); iter != info.trade_list_[coin_symble].rend(); iter++){
      QDateTime time = QDateTime::currentDateTime();
      if(atoll(iter->ts_.data()) - time.toMSecsSinceEpoch() < 1000*atol(param_list_[0].c_str())){
        value_count+=1;
      }else{
        break;
      }
    }
    for(auto iter = info.trade_list_[coin_symble].rbegin(); iter != info.trade_list_[coin_symble].rend(); iter++){
      QDateTime time = QDateTime::currentDateTime();
      if(atoll(iter->ts_.data()) - time.toMSecsSinceEpoch() < 1000*atol(param_list_[3].c_str())){
        pravious_count+=1;
      }else{
        break;
      }
    }
    if(atoi(param_list_[2].c_str()) == 1){
      if(atoi(param_list_[1].c_str()) == 1 && value_count > atof(param_list_[3].c_str())){
        return true;
      }else if(atoi(param_list_[1].c_str()) == 2 && value_count < atof(param_list_[3].c_str())){
        return true;
      }
    }else if(atoi(param_list_[2].c_str()) == 2){
      if(atoi(param_list_[1].c_str()) == 1 && (value_count/atoi(param_list_[0].c_str()))/(pravious_count/atoi(param_list_[3].c_str()))/100 < atof(param_list_[4].c_str())){
        return true;
      }else if(atoi(param_list_[1].c_str()) == 2 && (value_count/atoi(param_list_[0].c_str()))/(pravious_count/atoi(param_list_[3].c_str()))/100 > atof(param_list_[4].c_str())){
        return true;
      }
    }
    return false;
  }
};


#endif // QUANTITATIVETRANSACTION_H
