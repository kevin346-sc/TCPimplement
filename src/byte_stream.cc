#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : queue_(), capacity_( capacity ), closed_flag(false), wsize_(0), rsize_(0),  error_(false) {}

bool Writer::is_closed() const
{
  // Your code here.
  return closed_flag;
}

void Writer::push( string data )
{
  // Your code here.
  if(closed_flag || has_error())
    return;
  uint64_t size_can_write = min(data.size(), available_capacity());
  wsize_ += size_can_write;
  if(size_can_write < data.size())
    data = data.substr(0, size_can_write);
  for(uint64_t i = 0; i < size_can_write; i++)
  {
    queue_.push_back(data[i]);
  }  

  // queue_.push_back(data);
}

void Writer::close()
{
  closed_flag = true;
}

uint64_t Writer::available_capacity() const
{
  return capacity_ - (uint64_t)queue_.size();
}

uint64_t Writer::bytes_pushed() const
{
  return wsize_;
}

bool Reader::is_finished() const
{
  return closed_flag && queue_.empty();
}

uint64_t Reader::bytes_popped() const
{
  return rsize_;
}

string_view Reader::peek() const
{
  uint64_t peek_size = min<uint64_t>(8, queue_.size());
  return string_view(&queue_.front(), peek_size);

  // return string_view(queue_.front());

}

void Reader::pop( uint64_t len )
{
  uint64_t size_can_read = min(len, queue_.size());
  rsize_ += size_can_read;

  for(uint64_t i = 0; i < size_can_read; i++)
  {
    queue_.pop_front();
  }

  // while(size_can_read)
  // {
  //   size_can_read -= queue_.front().size();
  //   queue_.pop_front();
  // }

  // string buf = "";
  // Reader rd = Reader::reader();
  // read(rd, size_can_read, buf);

  
}

uint64_t Reader::bytes_buffered() const
{
  return queue_.size();
}
