#include "main_widget.h"
#include "ui_main_widget.h"
#include "market_interface.h"
#include <QComboBox>
#include <QTableWidget>
MainWidget::MainWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::MainWidget)
{
  ui->setupUi(this);
  auto market_list = MarketInterface::GetMarketList();
  for(auto market_item: market_list){
    ui->cmbMarket->addItem(market_item.first.c_str(), market_item.second);
  }
  InitHuobiTable();
}

MainWidget::~MainWidget(){
  delete ui;
}

void MainWidget::on_pushButton_clicked(){

}

void MainWidget::InitHuobiTable()
{
  std::map<std::string, std::vector<std::string>> market_pair_list = market_huobi.GetMarketPairMap();
  for(auto& item_tmp:market_pair_list){
    for(auto& item:item_tmp.second){
      int idx = ui->tabHuobi->rowCount();
      ui->tabHuobi->insertRow(idx);
      QString str = QString().sprintf("%s/%s", item.c_str(), item_tmp.first.c_str());
      ui->tabHuobi->setItem(idx, 0, new QTableWidgetItem(str));

    }
  }
}
