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
  ui->process_huobi->setValue(huobi_delay > 2000?2000:huobi_delay);
}

void MainWidget::on_btnHuobiTransactionStart_clicked(){
  huobi_market_.StartWatch();
}

void MainWidget::InitMarketPair()
{
}

void MainWidget::on_tabHuobi_activated(const QModelIndex &index){

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
