#ifndef MAIN_WIDGET_H
#define MAIN_WIDGET_H

#include <QWidget>
#include <QTimer>
#include "market_huobi.h"
namespace Ui {
  class MainWidget;
}

class MainWidget : public QWidget
{
  Q_OBJECT

public:
  explicit MainWidget(QWidget *parent = 0);
  ~MainWidget();
signals:
  void DoDepthChanged(std::string coin_for, std::string coin_base, DepthInfo depth_info);
  void DoTradeHistoryChanged(std::string coin_for, std::string coin_base, TradeHistory history);
private slots:
  void on_pushButton_clicked();
  void on_tabHuobi_clicked(const QModelIndex &index);
private slots:
  void OnTimer100Out();
  void OnDepthChangedSlot(std::string coin_for, std::string coin_base, DepthInfo depth_info);
  void OnTradeHistoryChangedSlot(std::string coin_for, std::string coin_base, TradeHistory history);
private:
  void InitHuobiTable();
  void OnDepthChanged(std::string coin_for, std::string coin_base, DepthInfo depth_info);
  void OnTradeHistoryChanged(std::string coin_for, std::string coin_base, TradeHistory history);
private:
  Ui::MainWidget *ui;
  MarketHuobi market_huobi_;
  std::pair<std::string, std::string> current_coin_pair_;
  QTimer timer_100_;
};

#endif // MAIN_WIDGET_H
