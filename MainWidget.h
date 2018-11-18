#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QTimer>
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
  void OnTimer();
  void on_btnHuobiTransactionStart_clicked();
  void on_tabHuobi_activated(const QModelIndex &index);

  void on_tabHuobi_clicked(const QModelIndex &index);

private:
  void InitMarketPair();
private:
  Ui::MainWidget *ui;
  QTimer timer_;
  HuobiMarket huobi_market_;
};

#endif // MAINWIDGET_H
