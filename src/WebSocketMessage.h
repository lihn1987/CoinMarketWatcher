#ifndef __WEB_SOCKET_MESSAGE__H__
#define __WEB_SOCKET_MESSAGE__H__
#include <string>
#include <stdlib.h>
#include <limits>
struct WebSocketHead{
  unsigned int op_code_:4;/*%x0：表示一个延续帧。当Opcode为0时，表示本次数据传输采用了数据分片，当前收到的数据帧为其中一个数据分片。
                            %x1：表示这是一个文本帧（frame）
                            %x2：表示这是一个二进制帧（frame）
                            %x3-7：保留的操作代码，用于后续定义的非控制帧。
                            %x8：表示连接断开。
                            %x9：表示这是一个ping操作。
                            %xA：表示这是一个pong操作。
                            %xB-F：保留的操作代码，用于后续定义的控制帧。
                            */
  unsigned int rsv3_:1;
  unsigned int rsv2_:1;
  unsigned int rsv1_:1;
  unsigned int fin_:1;//是否是最后一个分片，1是，0不是
  unsigned int len1:7;//长度
  unsigned int mask_:1;//永远为0
  unsigned short int len2;//长度
  unsigned short int len3;//长度
  unsigned short int len4;//长度
  unsigned short int len5;//长度
  void  Init(){
    memset(this, 0, sizeof(*this));
  }
  uint64_t GetLen(){
    if(len1 < 126){
      return len1;
    }else if(len1 == 126){
      return len2;
    }else/* if(len1 == 127)*/{
      return ((uint64_t)len2<<48)|((uint64_t)len3<<32)|((uint64_t)len4<<16)|(uint64_t)len5;
    }
  }
  int Decode(const std::string &str_in){
    const char* p = str_in.data();
    if(str_in.size() < 2)return 0;
    memcpy(this, p, 2);
    if(len1 < 126)return 2;
    if(str_in.size() < 4)return 0;
    if(len1 == 126){
      memcpy(this, p, 4);
      return 4;
    }

    if(str_in.size() < 10)return 0;
    if(len1 == 127){
      memcpy(this, p, 10);
      return 10;
    }
    return 0;
  }
};
struct WebSocketMessage{
  WebSocketHead head_;
  std::string body_;
  bool Decode(std::string& str_in){
    int head_len = head_.Decode(str_in);
    if(head_len > 0 && str_in.size() >= head_.GetLen()+head_len){
      body_.assign(str_in, head_len, head_.GetLen()+head_len);
      str_in.erase(0, head_.GetLen()+head_len);
      return true;
    }
    return false;
  }
  static std::string Encode(const std::string& str_in){
    std::string rtn;
    WebSocketHead head;
    head.op_code_ = 0x02;
    head.rsv3_ = 0x00;
    head.rsv2_ = 0x00;
    head.rsv1_ = 0x00;
    head.fin_ = 0x01;
    head.mask_ = 0x00;
    if(str_in.size()<127){
      head.len1 = str_in.size();
      rtn.assign((char*)&head, 2);
    }else if(str_in.size() < std::numeric_limits<uint16_t>::max()){
      head.len1 = 126;
      head.len2 = str_in.size();
      rtn.assign((char*)&head, 4);
    }else{
      head.len1 = 127;
      head.len2 = 0;
      head.len3 = 0;
      head.len4 = 0;
      head.len5 = 0;

      head.len2 |= str_in.size()>>48;
      head.len3 |= str_in.size()>>32;
      head.len4 |= str_in.size()>>16;
      head.len5 |= str_in.size();
      rtn.assign((char*)&head, 10);
    }
    return rtn;
  }
};

#endif
