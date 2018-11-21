#include "MainWidget.h"
#include "ui_MainWidget.h"

MainWidget::MainWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::MainWidget){
  ui->setupUi(this);
  connect(&timer_, SIGNAL(timeout()), this, SLOT(OnTimer()));
  timer_.start(500);
}

MainWidget::~MainWidget()
{
  delete ui;
}

void MainWidget::OnTimer(){
  //刷新火币列表
  if(ui->tabHuobi->rowCount() == 0){
    std::list<std::pair<std::string, std::string> > huobi_pair_list = huobi_market_.GetMarketPair();
    for(const auto& item:huobi_pair_list){
      ui->tabHuobi->insertRow(ui->tabHuobi->rowCount());
      ui->tabHuobi->setItem(ui->tabHuobi->rowCount()-1, 0, new QTableWidgetItem((item.first+item.second).c_str()));
    }
  }else{
    for(int i = 0; i < ui->tabHuobi->rowCount(); i++){
      QString symble = ui->tabHuobi->item(i, 0)->text();
      if(huobi_market_.GetCoinInfo().trade_list_[symble.toStdString()].size()){
        ui->tabHuobi->setItem(i, 1, new QTableWidgetItem(huobi_market_.GetCoinInfo().trade_list_[symble.toStdString()].back().price_.c_str()));
        ui->tabHuobi->setItem(i, 2, new QTableWidgetItem(
                                QString::number(huobi_market_.GetCoinInfo().depth_info_[symble.toStdString()].delay_state_.Delay())));
      }
    }
  }
  //刷新火币延时
  uint32_t huobi_delay = huobi_market_.GetDelayState().Delay();
  ui->process_huobi->setValue(huobi_delay > ui->process_huobi->maximum()?ui->process_huobi->maximum():huobi_delay);

  //刷新可购买量化列表
  std::vector<QTableWidget*> buy_quan_list = {ui->tab_buyQuantitative_1, ui->tab_buyQuantitative_2};
  for(size_t i = 0; i < buy_quan_list.size(); i++){
    while(buy_quan_list[i]->rowCount()){
      buy_quan_list[i]->removeRow(0);
    }
  }
  for(size_t i = 0; i < huobi_market_.buy_quan_list_.size(); i++){
    for(const std::string& item:huobi_market_.buy_quan_list_[i].GetCoinSymbleSet()){
      buy_quan_list[i]->insertRow(0);
      buy_quan_list[i]->setItem(0, 0, new QTableWidgetItem(item.c_str()));
    }
  }

}

void MainWidget::on_btnHuobiTransactionStart_clicked(){
  huobi_market_.StartWatch();
}

void MainWidget::InitMarketPair()
{
}

void MainWidget::on_tabHuobi_clicked(const QModelIndex &index)
{
  QString symble = ui->tabHuobi->item(index.row(),0)->text();
  //刷新交易历史
  while(ui->tabHuobiTradeDetail->rowCount()){
    ui->tabHuobiTradeDetail->removeRow(0);
  }
  if(huobi_market_.GetCoinInfo().trade_list_[symble.toStdString()].size()){
    for(const TradeItem& item:huobi_market_.GetCoinInfo().trade_list_[symble.toStdString()]){
      ui->tabHuobiTradeDetail->insertRow(ui->tabHuobiTradeDetail->rowCount());
      ui->tabHuobiTradeDetail->setItem(ui->tabHuobiTradeDetail->rowCount()-1, 0
                                       ,new QTableWidgetItem(item.id_.c_str()));
      QDateTime time;
      time.setMSecsSinceEpoch(atol(item.ts_.data()));
      ui->tabHuobiTradeDetail->setItem(ui->tabHuobiTradeDetail->rowCount()-1, 1
                                       ,new QTableWidgetItem(time.toString("yyyy-MM-dd hh:mm:ss:zzz")));
      ui->tabHuobiTradeDetail->setItem(ui->tabHuobiTradeDetail->rowCount()-1, 2
                                       ,new QTableWidgetItem(item.price_.c_str()));
      ui->tabHuobiTradeDetail->setItem(ui->tabHuobiTradeDetail->rowCount()-1, 3
                                       ,new QTableWidgetItem(item.direction_=="sell"?"卖":"买"));
      ui->tabHuobiTradeDetail->setItem(ui->tabHuobiTradeDetail->rowCount()-1, 4
                                       ,new QTableWidgetItem(item.amount_.c_str()));
    }
  }
  //刷新挂盘
  while(ui->tabHuobiDepthAsks->rowCount()){
    ui->tabHuobiDepthAsks->removeRow(0);
  }
  for(std::pair<std::string, std::string> item: huobi_market_.GetCoinInfo().depth_info_[symble.toStdString()].asks_){
    ui->tabHuobiDepthAsks->insertRow(0);
    ui->tabHuobiDepthAsks->setItem(0, 0, new QTableWidgetItem(item.first.c_str()));
    ui->tabHuobiDepthAsks->setItem(0, 1, new QTableWidgetItem(item.second.c_str()));
  }

  while(ui->tabHuobiDepthBids->rowCount()){
    ui->tabHuobiDepthBids->removeRow(0);
  }
  for(std::pair<std::string, std::string> item: huobi_market_.GetCoinInfo().depth_info_[symble.toStdString()].bids_){
    ui->tabHuobiDepthBids->insertRow(ui->tabHuobiDepthBids->rowCount());
    ui->tabHuobiDepthBids->setItem(ui->tabHuobiDepthBids->rowCount()-1, 0, new QTableWidgetItem(item.first.c_str()));
    ui->tabHuobiDepthBids->setItem(ui->tabHuobiDepthBids->rowCount()-1, 1, new QTableWidgetItem(item.second.c_str()));
  }
}

void MainWidget::on_btn_buyQuantitative_1_clicked(){
  huobi_market_.buy_quan_list_.clear();
  std::vector<QComboBox*> quan_type_list = {ui->cb_quan_type_1, ui->cb_quan_type_2};
  std::vector<QCheckBox*> use_quan_list = {ui->cb_buyQuantitative_1, ui->cb_buyQuantitative_2};
  std::vector<QLineEdit*> param1_list = {ui->edt_param_1_1, ui->edt_param_1_2};
  std::vector<QComboBox*> param2_list = {ui->cb_param_2_1, ui->cb_param_2_2};
  std::vector<QRadioButton*> param3_list = {ui->radio_param_31_1, ui->radio_param_31_2};
  std::vector<QLineEdit*> param41_list = {ui->edt_Param_41_1, ui->edt_Param_41_2};
  std::vector<QLineEdit*> param42_list = {ui->edt_Param_42_1, ui->edt_Param_42_2};
  std::vector<QLineEdit*> param5_list = {ui->edt_Param_5_1, ui->edt_Param_5_2};
  for(size_t i = 0; i < use_quan_list.size(); i++){
    if(use_quan_list[i]->checkState() == Qt::Checked){
      std::vector<std::string> param_list;
      param_list.push_back(param1_list[i]->text().toStdString());
      param_list.push_back((param2_list[i]->currentIndex() == 0)?"1":"2");
      if(param3_list[i]->isChecked() == true){
        param_list.push_back("1");
        param_list.push_back(param41_list[i]->text().toStdString());
      }else{
        param_list.push_back("2");
        param_list.push_back(param42_list[i]->text().toStdString());
        param_list.push_back(param5_list[i]->text().toStdString());
      }
      QuantitativeTransactionItem quan_item;
      quan_item.Init((QuantitativeComputeType)(quan_type_list[i]->currentIndex()), param_list);
      huobi_market_.buy_quan_list_.push_back(quan_item);
    }
  }



/*


  if(ui->cb_buyQuantitative_1->checkState() == Qt::Checked){
    std::vector<std::string> param_list;
    param_list.push_back(ui->edt_param_1_1->text().toStdString());
    param_list.push_back((ui->cb_param_2_1->currentIndex() == 0)?"1":"2");
    if(ui->radio_param_31_1->isChecked() == true){
      param_list.push_back("1");
      param_list.push_back(ui->edt_Param_41_1->text().toStdString());
    }else{
      param_list.push_back("2");
      param_list.push_back(ui->edt_Param_42_1->text().toStdString());
      param_list.push_back(ui->edt_Param_5_1->text().toStdString());
    }
    QuantitativeTransactionItem quan_item;
    quan_item.Init(QuantitativeComputeType::QC_TransactionValueSum, param_list);
    huobi_market_.buy_quan_list_.push_back(quan_item);
  }
  if(ui->cb_buyQuantitative_2->checkState() == Qt::Checked){
    std::vector<std::string> param_list;
    param_list.push_back(ui->edt_param_1_2->text().toStdString());
    param_list.push_back((ui->cb_param_2_2->currentIndex() == 0)?"1":"2");
    if(ui->radio_param_31_1->isChecked() == true){
      param_list.push_back("1");
      param_list.push_back(ui->edt_Param_41_2->text().toStdString());
    }else{
      param_list.push_back("2");
      param_list.push_back(ui->edt_Param_42_2->text().toStdString());
      param_list.push_back(ui->edt_Param_5_2->text().toStdString());
    }
    QuantitativeTransactionItem quan_item;
    quan_item.Init(QuantitativeComputeType::QC_TransactionValueSum, param_list);
    huobi_market_.buy_quan_list_.push_back(quan_item);
  }
  */
}
