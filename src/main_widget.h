#ifndef MAIN_WIDGET_H
#define MAIN_WIDGET_H

#include <QWidget>
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

private slots:
  void on_pushButton_clicked();

private:
  void InitHuobiTable();
private:
  Ui::MainWidget *ui;
  MarketHuobi market_huobi;
};

#endif // MAIN_WIDGET_H
