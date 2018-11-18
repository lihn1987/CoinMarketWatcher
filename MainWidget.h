#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include "HuobiMarket.h"
namespace Ui {
  class MainWidget;
}

class MainWidget : public QWidget
{
  Q_OBJECT
public:
  explicit MainWidget(QWidget *parent = 0);
  ~MainWidget();

private slots:
  void on_btnHuobiTransactionStart_clicked();
private:
  void InitMarketPair();
private:
  Ui::MainWidget *ui;
  HuobiMarket huobi_market_;
};

#endif // MAINWIDGET_H
