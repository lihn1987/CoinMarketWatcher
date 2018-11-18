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
