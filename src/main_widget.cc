#include "main_widget.h"
#include "ui_main_widget.h"

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
