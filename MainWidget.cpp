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
      QString symbol = ui->tabHuobi->item(i, 0)->text();
      if(huobi_market_.GetCoinInfo().trade_list_[symbol.toStdString()].size()){
        ui->tabHuobi->setItem(i, 1, new QTableWidgetItem(huobi_market_.GetCoinInfo().trade_list_[symbol.toStdString()].back().price_.c_str()));
        ui->tabHuobi->setItem(i, 2, new QTableWidgetItem(
                                QString::number(huobi_market_.GetCoinInfo().depth_info_[symbol.toStdString()].delay_state_.Delay())));
      }
    }
  }
  //刷新火币延时
  int huobi_delay = (int)huobi_market_.GetDelayState().Delay();
  ui->process_huobi->setValue(huobi_delay > ui->process_huobi->maximum()?ui->process_huobi->maximum():huobi_delay);

  //刷新可购买量化列表
  std::vector<QTableWidget*> buy_quan_list = {ui->tab_buyQuantitative_1, ui->tab_buyQuantitative_2, ui->tab_buyQuantitative_3, ui->tab_buyQuantitative_4, ui->tab_buyQuantitative_5, ui->tab_buyQuantitative_6, ui->tab_buyQuantitative_7, ui->tab_buyQuantitative_8, ui->tab_buyQuantitative_9, ui->tab_buyQuantitative_10};
  for(size_t i = 0; i < buy_quan_list.size(); i++){
    while(buy_quan_list[i]->rowCount()){
      buy_quan_list[i]->removeRow(0);
    }
  }
  for(size_t i = 0; i < huobi_market_.buy_quan_list_.size(); i++){
    if(huobi_market_.buy_quan_list_[i].GetComputeType() != QuantitativeComputeType::QC_DepthBuySellMargin &&
    huobi_market_.buy_quan_list_[i].GetComputeType() != QuantitativeComputeType::QC_DepthBuyCount &&
       huobi_market_.buy_quan_list_[i].GetComputeType() != QuantitativeComputeType::QC_DepthSellCount){
      for(const std::string& item:huobi_market_.buy_quan_list_[i].GetCoinsymbolSet()){
        buy_quan_list[i]->insertRow(0);
        buy_quan_list[i]->setItem(0, 0, new QTableWidgetItem(item.c_str()));
      }
    }
  }

  if(huobi_market_.buy_quan_list_.size()){
    QuantitativeTransactionItem & latest_item = huobi_market_.buy_quan_list_[huobi_market_.buy_quan_list_.size()-1];
    if(latest_item.GetComputeType() == QuantitativeComputeType::QC_DepthBuySellMargin||
       latest_item.GetComputeType() == QuantitativeComputeType::QC_DepthBuyCount||
       latest_item.GetComputeType() == QuantitativeComputeType::QC_DepthSellCount){
      while(ui->tab_buyQuantitative_depth->rowCount()){
        ui->tab_buyQuantitative_depth->removeRow(0);
      }
      for(const std::string& item:latest_item.GetCoinsymbolSet()){
        ui->tab_buyQuantitative_depth->insertRow(0);
        ui->tab_buyQuantitative_depth->setItem(0, 0, new QTableWidgetItem(item.c_str()));
      }
    }
  }

  ui->edt_huobi_balance->setText(QString().sprintf("%f", huobi_market_.GetBalanceBtcAll()));
  while(ui->tabHuobiBalance->rowCount()){
    ui->tabHuobiBalance->removeRow(0);
  }
  for(auto item:huobi_market_.GetBalanceAll()){
    ui->tabHuobiBalance->insertRow(0);
    ui->tabHuobiBalance->setItem(0,0, new QTableWidgetItem(item.first.c_str()));
    ui->tabHuobiBalance->setItem(0,1, new QTableWidgetItem(QString::number(item.second)));
    //ui->tabHuobiBalance->setItem(0,2, new QTableWidgetItem(QString::number(QString::fromStdString(item.first).toDouble()*item.second)));
  }

  ui->edt_huobi_balance->setText(QString().sprintf("%f", huobi_market_.GetBalanceBtcAll()));
  while(ui->tabHuobHistory->rowCount()){
    ui->tabHuobHistory->removeRow(0);
  }
  for(auto item:huobi_market_.GetTradeHistory()){
    ui->tabHuobHistory->insertRow(0);
    ui->tabHuobHistory->setItem(0,0, new QTableWidgetItem(item.coin_.c_str()));
    ui->tabHuobHistory->setItem(0,1, new QTableWidgetItem(QString::number(item.count_)));
    ui->tabHuobHistory->setItem(0,2, new QTableWidgetItem(QString::number(item.price_)));
    ui->tabHuobHistory->setItem(0,3, new QTableWidgetItem(huobi_market_.GetCoinInfo().trade_list_[item.coin_].back().price_.c_str()));
    //ui->tabHuobiBalance->setItem(0,2, new QTableWidgetItem(QString::number(QString::fromStdString(item.first).toDouble()*item.second)));
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
  QString symbol = ui->tabHuobi->item(index.row(),0)->text();
  //刷新交易历史
  while(ui->tabHuobiTradeDetail->rowCount()){
    ui->tabHuobiTradeDetail->removeRow(0);
  }
  if(huobi_market_.GetCoinInfo().trade_list_[symbol.toStdString()].size()){
    for(const TradeItem& item:huobi_market_.GetCoinInfo().trade_list_[symbol.toStdString()]){
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
  for(std::pair<std::string, std::string> item: huobi_market_.GetCoinInfo().depth_info_[symbol.toStdString()].asks_){
    ui->tabHuobiDepthAsks->insertRow(0);
    ui->tabHuobiDepthAsks->setItem(0, 0, new QTableWidgetItem(item.first.c_str()));
    ui->tabHuobiDepthAsks->setItem(0, 1, new QTableWidgetItem(item.second.c_str()));
  }

  while(ui->tabHuobiDepthBids->rowCount()){
    ui->tabHuobiDepthBids->removeRow(0);
  }
  for(std::pair<std::string, std::string> item: huobi_market_.GetCoinInfo().depth_info_[symbol.toStdString()].bids_){
    ui->tabHuobiDepthBids->insertRow(ui->tabHuobiDepthBids->rowCount());
    ui->tabHuobiDepthBids->setItem(ui->tabHuobiDepthBids->rowCount()-1, 0, new QTableWidgetItem(item.first.c_str()));
    ui->tabHuobiDepthBids->setItem(ui->tabHuobiDepthBids->rowCount()-1, 1, new QTableWidgetItem(item.second.c_str()));
  }
}

void MainWidget::on_btn_buyQuantitative_1_clicked(){
  huobi_market_.buy_quan_list_.clear();
  std::vector<QComboBox*> quan_type_list;
  std::vector<QCheckBox*> use_quan_list;
  std::vector<QLineEdit*> param1_list;
  std::vector<QComboBox*> param2_list;
  std::vector<QRadioButton*> param3_list;
  std::vector<QLineEdit*> param41_list;
  std::vector<QLineEdit*> param42_list;
  std::vector<QLineEdit*> param5_list;
  for(int i = 0; i < 10; i++){
    quan_type_list.push_back(findChild<QComboBox *>(QString("cb_quan_type_%1").arg(i+1)));
    use_quan_list.push_back(findChild<QCheckBox *>(QString("cb_buyQuantitative_%1").arg(i+1)));
    param1_list.push_back(findChild<QLineEdit *>(QString("edt_param_1_%1").arg(i+1)));
    param2_list.push_back(findChild<QComboBox *>(QString("cb_param_2_%1").arg(i+1)));
    param3_list.push_back(findChild<QRadioButton *>(QString("radio_param_31_%1").arg(i+1)));
    param41_list.push_back(findChild<QLineEdit *>(QString("edt_Param_41_%1").arg(i+1)));
    param42_list.push_back(findChild<QLineEdit *>(QString("edt_Param_42_%1").arg(i+1)));
    param5_list.push_back(findChild<QLineEdit *>(QString("edt_Param_5_%1").arg(i+1)));
  }
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
  //深度条件
  if(ui->cb_buy_depth1->isChecked() == true){
    std::vector<std::string> param_list;
    if(ui->cb_param_depth_1_1->currentIndex() == 0){
      param_list.push_back("1");
    }else{
      param_list.push_back("2");
    }
    param_list.push_back(ui->edt_param_depth_2_1->text().toStdString());
    QuantitativeTransactionItem quan_item;
    quan_item.Init(QuantitativeComputeType::QC_DepthBuySellMargin, param_list);
    huobi_market_.buy_quan_list_.push_back(quan_item);
  }
  if(ui->cb_buy_depth2->isChecked() == true){
    std::vector<std::string> param_list;
    param_list.push_back(ui->edt_param_depth_1_2->text().toStdString());
    if(ui->cb_param_depth_2_2->currentIndex() == 0){
      param_list.push_back("1");
    }else{
      param_list.push_back("2");
    }
    param_list.push_back(ui->edt_param_depth_3_2->text().toStdString());
    QuantitativeTransactionItem quan_item;
    quan_item.Init(QuantitativeComputeType::QC_DepthBuyCount, param_list);
    huobi_market_.buy_quan_list_.push_back(quan_item);
  }

  if(ui->cb_buy_depth3->isChecked() == true){
    std::vector<std::string> param_list;
    param_list.push_back(ui->edt_param_depth_1_3->text().toStdString());
    if(ui->cb_param_depth_2_3->currentIndex() == 0){
      param_list.push_back("1");
    }else{
      param_list.push_back("2");
    }
    param_list.push_back(ui->edt_param_depth_3_3->text().toStdString());
    QuantitativeTransactionItem quan_item;
    quan_item.Init(QuantitativeComputeType::QC_DepthBuySellMargin, param_list);
    huobi_market_.buy_quan_list_.push_back(quan_item);
  }
}

void MainWidget::on_btn_apply_config_clicked(){
  QNetworkProxy proxy;
  proxy.setType(QNetworkProxy::Socks5Proxy);
  proxy.setHostName(ui->edt_proxy_ip->text());
  proxy.setPort(ui->edt_proxy_port->text().toInt());
  QNetworkProxy::setApplicationProxy(proxy);
}

void MainWidget::on_btn_huobi_simulate_clicked(){
  huobi_market_.SetSimulate(true);
  huobi_market_.ClearBalance();
  huobi_market_.SetBalance("btc",ui->edt_huobi_simulate_balance->text().toDouble());
}
