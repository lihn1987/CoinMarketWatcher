#include "MainWidget.h"
#include "ui_MainWidget.h"

MainWidget::MainWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::MainWidget)
{
  ui->setupUi(this);
}

MainWidget::~MainWidget()
{
  delete ui;
}

void MainWidget::on_btnHuobiTransactionStart_clicked(){
  huobi_market_.StartWatch();
}

void MainWidget::InitMarketPair()
{
}
