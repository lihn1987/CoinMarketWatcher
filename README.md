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
- 优化短视内存占用
- 增加模拟卖盘，优化短视策略
- 为卖盘增加止赢止损策略

## 正在进行的计划
- 能够将数据保存到数据库以备用
- 能够实现对历史数据的模拟买卖计算利润
- 实现对账户的配置

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

##日志
- 2018-12-04 从凌晨1点到11点，长时间测试，没有奔溃了，稳定的一笔！
- 2018-12-04 22点开始测试，模拟策略看看有没有利润，哈哈
- 2018-12-05 10:00 昨天的测试亏了，策略为60s级别的策略，11个完成的交易对中，3赚，8亏。12小时比特币的跌幅为4.4%，而我相对于比特币的跌幅增加了3个点，失败，由于这次测试日志不够方便，没有更详细的记录了，下次记录更详细的参数～
- 2018-12-05 23:41 继续测试，相同的策略，60s级别的，修改了日志记录的bug
- 2018-12-05 08:12 亏了1个点，比特币从3875跌到了3806，跌了近2个点，而我的策略相对于btc又跌了1个点，但是这个策略我还是要测试下去，由于条件限制只能测一种策略，所以就测完这个星期吧。测试具体买卖历史为

2018:12:05-23-32-45	买	iostbtc	1.22094e-6	8190.4
2018:12:05-23-33-18	买	neobtc	0.00188377	5.30851
2018:12:05-23-43-31	买	bsvbtc	0.0232806	0.429543
2018:12:05-23-44-51	买	thetabtc	1.57816e-5	633.651
2018:12:06-00-18-17	买	steembtc	8.00501e-5	124.922
2018:12:06-00-32-12	买	eosbtc	0.000587935	17.0087
2018:12:06-00-32-33	买	bchbtc	0.0357331	0.279853
2018:12:06-00-41-22	卖	iostbtc	1.25694e-6	8174.02
2018:12:06-00-54-12	买	xrpbtc	8.97295e-5	111.446
2018:12:06-00-58-46	买	phxbtc	2.57515e-6	3883.27
2018:12:06-00-59-40	买	ontbtc	0.000179449	55.7262
2018:12:06-01-08-41	买	lskbtc	0.000350701	28.5143
2018:12:06-01-15-17	卖	eosbtc	0.00060492	16.9747
2018:12:06-01-35-40	买	dashbtc	0.021042	0.47524
2018:12:06-04-19-56	卖	thetabtc	1.529e-5	632.383
2018:12:06-04-46-22	买	adabtc	9.23848e-6	1082.43
2018:12:06-07-08-07	卖	bchbtc	0.0345848	0.279293
2018:12:06-07-28-40	卖	steembtc	7.73253e-5	124.672
2018:12:06-07-47-39	买	omgbtc	0.000367735	27.1935

