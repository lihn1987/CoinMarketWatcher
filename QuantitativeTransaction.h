#ifndef QUANTITATIVETRANSACTION_H
#define QUANTITATIVETRANSACTION_H
#include <list>
#include <string>
#include <set>
#include "MarketInterface.h"
enum QuantitativeComputeType{
  QC_INVALIDATE = -1,
  /*
  参数描述
  0:time_second 1:>(1) <(2)  2:now(1)      3 value
                             2:pravious(2) 3 second  4 persent
  */
  QC_TransactionValueSum = 0,//交易总量
  QC_TransactionTimeSum = 1,//交易总次数
  QC_TransactionBuySellValueScale = 2,//买卖交易量比例
  QC_TransactionBuySellTimeScale = 3,//买卖交易数比例

  //参数描述  0:卖盘最低价/买盘最高价 1:>(1) <(2) 2:value
  QC_DepthBuySellMargin = 4,//买卖价格差
  //参数描述 0:相对于卖盘买盘的差价百分比   买完需要的比特币个数 1:>(1) <(2)  2:value
  QC_DepthBuyCount = 5,//
  //参数描述  1:相对于买盘相对于卖盘的差价百分比能卖出的比特币个数 1:>(1) <(2)  2:value
  QC_DepthSellCount = 6,
};

class QuantitativeTransactionItem{
private:
  QuantitativeComputeType compute_type_=QC_INVALIDATE;

  std::vector<std::string> param_list_;
  std::set<std::string> coin_symbol_;
public:
  void Init(QuantitativeComputeType type, std::vector<std::string>& param_list);
  std::set<std::string> GetCoinsymbolSet();
  QuantitativeComputeType GetComputeType();
  void Removesymbol(const std::string& symbol);
  bool Compute(const std::string &coin_symbol, CoinInfo& info);
  bool ComputeValueSum(const std::string &coin_symbol, CoinInfo& info);
  bool ComputeTimeSum(const std::string &coin_symbol, CoinInfo& info);
  bool ComputeBuySellValueScale(const std::string &coin_symbol, CoinInfo& info);
  bool ComputeBuySellTimeScale(const std::string &coin_symbol, CoinInfo& info);
  bool ComputeBuySellMargin(const std::string &coin_symbol, CoinInfo& info);
  bool ComputeBuyCount(const std::string &coin_symbol, CoinInfo& info);
  bool ComputeSellCount(const std::string &coin_symbol, CoinInfo& info);

};
#endif // QUANTITATIVETRANSACTION_H
