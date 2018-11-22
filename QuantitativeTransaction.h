#ifndef QUANTITATIVETRANSACTION_H
#define QUANTITATIVETRANSACTION_H
#include <list>
#include <string>
#include <set>
#include "MarketInterface.h"
enum QuantitativeComputeType{
  QC_INVALIDATE = -1,
  QC_TransactionValueSum = 0,//交易总量
  QC_TransactionTimeSum = 1,//交易总次数
  QC_TransactionBuySellValueScale = 2,//买卖交易量比例
  QC_TransactionBuySellTimeScale = 3,//买卖交易数比例
};

class QuantitativeTransactionItem{
private:
  QuantitativeComputeType compute_type_=QC_INVALIDATE;
  /*
  0:time_second 1:>(1) <(2)  2:now(1)      3 value
                             2:pravious(2) 3 second  4 persent
  */
  std::vector<std::string> param_list_;
  std::set<std::string> coin_symbol_;
public:
  void Init(QuantitativeComputeType type, std::vector<std::string>& param_list);
  std::set<std::string> GetCoinsymbolSet();
  void Removesymbol(const std::string& symbol);
  bool Compute(const std::string &coin_symbol, CoinInfo& info);
  bool ComputeValueSum(const std::string &coin_symbol, CoinInfo& info);
  bool ComputeTimeSum(const std::string &coin_symbol, CoinInfo& info);
  bool ComputeBuySellValueScale(const std::string &coin_symbol, CoinInfo& info);
  bool ComputeBuySellTimeScale(const std::string &coin_symbol, CoinInfo& info);

};
#endif // QUANTITATIVETRANSACTION_H
