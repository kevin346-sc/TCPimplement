#include "tcp_receiver.hh"

using namespace std;

//1. Set the Initial Sequence Number if necessary
//2. Push any data to the Reassembler
const uint16_t WINDOW_SIZE_MAX = UINT16_MAX; // 定义常量

void TCPReceiver::receive(TCPSenderMessage message)
{
    if (message.RST)
    {
        reassembler_.reader().set_error();
        return;
    }

    if (!init_flag)
    {
        if (message.SYN)
        {
            zero_point = message.seqno;
            reassembler_.insert(0, message.payload, message.FIN);
            init_flag = true;
        }
        else
        {
            return;
        }
    }
    else
    {
        bool isLastPacket = message.FIN;
        first_index = message.seqno.unwrap(zero_point, writer().bytes_pushed());
        reassembler_.insert(first_index - 1, message.payload, isLastPacket);
    }
}

TCPReceiverMessage TCPReceiver::send() const
{
  TCPReceiverMessage tcpReceiverMessage{}; 
  if (reassembler_.reader().has_error())
  {
    tcpReceiverMessage.RST = true;
    return tcpReceiverMessage;
  }
  
  if (init_flag)
  {
    uint32_t pushedBytes = writer().bytes_pushed();
    uint32_t closedFlag = writer().is_closed() ? 1 : 0; 
    tcpReceiverMessage.ackno = Wrap32::wrap(pushedBytes + 1 + closedFlag, zero_point);
  }
  
  tcpReceiverMessage.window_size = writer().available_capacity() >= WINDOW_SIZE_MAX ? WINDOW_SIZE_MAX : writer().available_capacity();
  return tcpReceiverMessage;
}
