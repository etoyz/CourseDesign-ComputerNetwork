# 计算机网络课程设计
## 课设要求
- 实现一个基于socket的物联网服务器，可以接收来自客户端的数据。
- 实现一个物联网客户端，用于模拟物联网设备。客户端产生数据，发送至服务器，服务器根据协议对数据做出处理。

## 协议设计
<img src="https://user-images.githubusercontent.com/60183283/221099326-c2475fe6-98fc-40d5-812d-f747fcf6a34e.jpg" alt="drawing" width="480"/>

## 报文格式
- **客户端到服务器的请求报文**
<img src="https://user-images.githubusercontent.com/60183283/221100460-32d5344f-2bc2-4754-88dc-2caa7d0cbe8b.png" alt="drawing" width="650"/>

*其中，前4字节为首部，表示请求方法。后17字节为数据部分，表示具体请求数据。目前有两种请求方法，分别是DATA方法和STAT方法，DATA方法表示发送的是当前的传感器数据，STAT方法表示发送的是对应电子设备是否成功执行了来自服务器的指令。*
  
- **服务器到客户端的请求报文**
<img src="https://user-images.githubusercontent.com/60183283/221100669-e4cb334e-52e1-4d86-b232-1fd31c38f2f3.png" alt="drawing" width="650"/>

*其中，前3字节为首部，表示请求方法。后9字节为数据部分，表示具体请求数据。目前有两种请求方法，分别是SET方法和GET方法。*  

