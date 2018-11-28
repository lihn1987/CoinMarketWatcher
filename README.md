# CoinMarketWatcher
![界面预览](preview.png)
通过监控各种数字货币市场数据的软件
我的联系方式:lihn1011@163.com
## 目前进度
- 能够监控历史交易数据，并在界面上显示
- 能够监控实时价格，并在界面上显示
- 能够非实时监控交易深度，并在界面上显示
- 能够将一段时间交易量作为买入条件
- 能够将一段时间交易量同另外一段时间交易量的比例作为买入条件
- 能够将一段时间交易次数作为买入条件
- 能够将一段时间交一次数同另外一段时间交易次数的比例作为买入条件
- 能够同时设置10个基于交易明细的买入条件
- 能够将一段时间内的买卖交易量比作为买入条件
- 能够将一段时间内的买卖交易量比同另外一段时间的买卖交易量比的比例作为买入条件
- 能够将一段时间的买卖交易次数比作为买入条件
- 能够将一段时间内的买卖交易次数比同另外一段时间的买卖交易次数比作为买入条件
- 能够在界面上进行代理设置
- 能够将买卖价格差作为买入条件
- 能够将百分比内能买到的btc数量作为买入条件
- 能够模拟买入

## 正在进行的计划
- 能够将数据保存到数据库以备用

## 未来计划
- 能够对卖出条件进行筛选
- 实现对账户的配置
- 能够模拟卖出
- 实现实盘买入功能
- 实现实盘卖出功能
- 实现对账户的监控
- 实现对交易历史的监控
- 实现对套利历史的分析

## 已知bug
- websocket超时时，无法直接重连，因为头几条消息的延时时间非常长，无法手动进行超时判断，而我又担心加了代理后，有时连接断开无法被发现
