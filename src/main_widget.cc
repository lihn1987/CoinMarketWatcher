#include "main_widget.h"
#include "ui_main_widget.h"
#include "market_interface.h"
#include "market_huobi.h"
#include <QComboBox>
#include <QTableWidget>
#include <QTime>
MainWidget::MainWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::MainWidget)
{
  ui->setupUi(this);
  auto market_list = MarketInterface::GetMarketList();
  for(auto market_item: market_list){
    ui->cmbMarket->addItem(market_item.first.c_str(), market_item.second);
  }
  //设置表头自动宽度
  ui->tabAsks->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  ui->tabBids->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  ui->tabTradeHistory->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  ui->tabTradeHistory->setRowCount(2000);

  qRegisterMetaType<std::string>("std::string");
  qRegisterMetaType<std::string>("std::string");
  qRegisterMetaType<DepthInfo>("DepthInfo");
  qRegisterMetaType<TradeHistory>("TradeHistory");

  connect(&timer_100_, SIGNAL(timeout()), this, SLOT(OnTimer100Out()));
  connect(this, SIGNAL(DoDepthChanged(std::string,std::string,DepthInfo)), this, SLOT(OnDepthChangedSlot(std::string,std::string,DepthInfo)));
  connect(this, SIGNAL(DoTradeHistoryChanged(std::string,std::string,TradeHistory)), this, SLOT(OnTradeHistoryChangedSlot(std::string,std::string,TradeHistory)));


  InitHuobiTable();
  market_huobi_.AddDepthChangeWatcher(boost::bind(&MainWidget::OnDepthChanged, this, _1, _2, _3));
  market_huobi_.AddTradeHistroyChangeWatcher(boost::bind(&MainWidget::OnTradeHistoryChanged, this, _1, _2, _3));

}

MainWidget::~MainWidget(){
  delete ui;
}

void MainWidget::on_pushButton_clicked(){
  market_huobi_.StartWatch();
}

void MainWidget::InitHuobiTable()
{
  std::map<std::string, std::vector<std::string>> market_pair_list = market_huobi_.GetMarketPairMap();
  for(auto& item_tmp:market_pair_list){
    for(auto& item:item_tmp.second){
      int idx = ui->tabHuobi->rowCount();
      ui->tabHuobi->insertRow(idx);
      QString str = QString().sprintf("%s/%s", item.c_str(), item_tmp.first.c_str());
      ui->tabHuobi->setItem(idx, 0, new QTableWidgetItem(str));
    }
  }
  timer_100_.start(100);
}

void MainWidget::OnDepthChanged(std::string coin_for, std::string coin_base, DepthInfo depth_info){
  emit DoDepthChanged(coin_for, coin_base, depth_info);
}

void MainWidget::OnTradeHistoryChanged(std::string coin_for, std::string coin_base, TradeHistory history){
  emit DoTradeHistoryChanged(coin_for, coin_base, history);
}

void MainWidget::on_tabHuobi_clicked(const QModelIndex &index){
  QTableWidgetItem *item = ui->tabHuobi->item(index.row(), 0);
  QString str_pair = item->text();
  QStringList str_list = str_pair.split("/");
  if(str_list.size() == 2){
    current_coin_pair_.first = str_list[0].toStdString();
    current_coin_pair_.second = str_list[1].toStdString();
  }

  if(item){
    QString str_pair = item->text();
    QStringList str_list = str_pair.split("/");
    if(str_list.size() == 2){
      {//对深度信息处理
        DepthInfo depth_info = market_huobi_.GetCoinDepthInfo(str_list[0].toStdString(), str_list[1].toStdString());
        int line_idx = 0;
        //显示买价
        for(std::pair<std::string, std::string> pair_item:depth_info.bids_){
          ui->tabBids->setItem(line_idx, 0, new QTableWidgetItem(pair_item.first.c_str()));
          ui->tabBids->setItem(line_idx++, 1, new QTableWidgetItem(pair_item.second.c_str()));
          if(line_idx > 10)break;
        }
        //显示卖价
        line_idx = 9;
        for(std::pair<std::string, std::string> pair_item:depth_info.asks_){
          ui->tabAsks->setItem(line_idx, 0, new QTableWidgetItem(pair_item.first.c_str()));
          ui->tabAsks->setItem(line_idx--, 1, new QTableWidgetItem(pair_item.second.c_str()));
          if(line_idx < 0)break;
        }
      }
      {//对交易信息处理
        TradeHistory trade_history = market_huobi_.GetCoinTradeHistory(str_list[0].toStdString(), str_list[1].toStdString());
        int line_idx = 0;
        for(const TradeItem& trade_item:trade_history.trade_list_){
          ui->tabTradeHistory->setItem(line_idx, 0, new QTableWidgetItem(trade_item.id_.c_str()));
          ui->tabTradeHistory->setItem(line_idx, 1, new QTableWidgetItem(trade_item.price_.c_str()));
          ui->tabTradeHistory->setItem(line_idx, 2, new QTableWidgetItem(trade_item.amount_.c_str()));
          ui->tabTradeHistory->setItem(line_idx, 3, new QTableWidgetItem(trade_item.direction_.c_str()));

          time_t tmp_time = QString(trade_item.ts.c_str()).toLongLong()/1000;
          struct tm *time = localtime(&tmp_time);
          ui->tabTradeHistory->setItem(line_idx++, 4, new QTableWidgetItem(QString().sprintf("%d-%02d-%02d %02d:%02d:%02d",time->tm_year+1900,time->tm_mon, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec)));

          //boost::posix_time::ptime posix_time = boost::posix_time::ptime_from_tm(*time);
          //posix_time.base_time

          if(line_idx>2000)break;
        }
      }
    }
    std::cout<<item->text().toStdString()<<std::endl;
  }
}

void MainWidget::OnTimer100Out(){
  static QTime time_prv = QTime::currentTime();
  QTime time_now = QTime::currentTime();
  int msec_distance = time_prv.msecsTo(time_now);
  time_prv = time_now;
  uint64_t depth_sum = 0, history_sum = 0;
  uint32_t depth_count = 0, history_count = 0;
  for(int i = 0; i < ui->tabHuobi->rowCount(); i++){
    QTableWidgetItem* item = ui->tabHuobi->item(i, 3);
    if(item){
      long long old_dis = item->text().toLongLong();
      if(old_dis > 0){
        depth_count ++;
        depth_sum += old_dis;
        item->setText(QString::number(old_dis + msec_distance));
      }
    }
    item = ui->tabHuobi->item(i, 4);
    if(item){
      long long old_dis = item->text().toLongLong();
      if(old_dis > 0){
        history_count ++;
        history_sum += old_dis;
        item->setText(QString::number(old_dis + msec_distance));
      }
    }
  }
  long long avg_depth_delay = depth_sum/(depth_count==0?1:depth_count);
  long long avg_history_delay = history_sum/(history_count==0?1:history_count);
  ui->edtAvgDepthDelay->setText(QString::number(avg_depth_delay));
  ui->edtAvgHistoryDelay->setText(QString::number(avg_history_delay));
}

void MainWidget::OnDepthChangedSlot(std::string coin_for, std::string coin_base, DepthInfo depth_info)
{
  {//更新延时
    QString pair_name = (coin_for+"/"+coin_base).c_str();
    for(int i = 0; i < ui->tabHuobi->rowCount(); i++){
      QTableWidgetItem* item = ui->tabHuobi->item(i, 0);
      if(item && item->text() == pair_name){
        ui->tabHuobi->setItem(i, 3, new QTableWidgetItem(QString().sprintf("%d", depth_info.active_info_.interval_)));
        break;
      }
    }
  }
  if(coin_for != current_coin_pair_.first || coin_base != current_coin_pair_.second){
    return;
  }
  //DepthInfo depth_info = market_huobi_.GetCoinDepthInfo(coin_for, coin_base);
  int line_idx = 0;
  //显示买价
  for(std::pair<std::string, std::string> pair_item:depth_info.bids_){
    ui->tabBids->setItem(line_idx, 0, new QTableWidgetItem(pair_item.first.c_str()));
    ui->tabBids->setItem(line_idx++, 1, new QTableWidgetItem(pair_item.second.c_str()));
    if(line_idx > 10)break;
  }
  //显示卖价
  line_idx = 9;
  for(std::pair<std::string, std::string> pair_item:depth_info.asks_){
    ui->tabAsks->setItem(line_idx, 0, new QTableWidgetItem(pair_item.first.c_str()));
    ui->tabAsks->setItem(line_idx--, 1, new QTableWidgetItem(pair_item.second.c_str()));
    if(line_idx < 0)break;
  }
}

void MainWidget::OnTradeHistoryChangedSlot(std::string coin_for, std::string coin_base, TradeHistory history){
  {//更新延时
    QString pair_name = (coin_for+"/"+coin_base).c_str();
    for(int i = 0; i < ui->tabHuobi->rowCount(); i++){
      QTableWidgetItem* item = ui->tabHuobi->item(i, 0);
      if(item && item->text() == pair_name){
        ui->tabHuobi->setItem(i, 4, new QTableWidgetItem(QString().sprintf("%d", history.active_info_.interval_)));
        break;
      }
    }
  }
  if(coin_for != current_coin_pair_.first || coin_base != current_coin_pair_.second){
    return;
  }
  int line_idx = 0;
  for(const TradeItem& trade_item:history.trade_list_){
    ui->tabTradeHistory->setItem(line_idx, 0, new QTableWidgetItem(trade_item.id_.c_str()));
    ui->tabTradeHistory->setItem(line_idx, 1, new QTableWidgetItem(trade_item.price_.c_str()));
    ui->tabTradeHistory->setItem(line_idx, 2, new QTableWidgetItem(trade_item.amount_.c_str()));
    ui->tabTradeHistory->setItem(line_idx, 3, new QTableWidgetItem(trade_item.direction_.c_str()));
    time_t tmp_time = QString(trade_item.ts.c_str()).toLongLong()/1000;
    struct tm *time = localtime(&tmp_time);
    ui->tabTradeHistory->setItem(line_idx++, 4, new QTableWidgetItem(QString().sprintf("%d-%02d-%02d %02d:%02d:%02d",time->tm_year+1900,time->tm_mon, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec)));
    //ui->tabTradeHistory->setItem(line_idx++, 4, new QTableWidgetItem(trade_item.ts.c_str()));
    if(line_idx>2000)break;
  }
}
