#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ), available_capacity_( capacity ), bytes_pushed_(0), bytes_popped_(0), bytes_buffered_(0) {}

bool Writer::is_closed() const
{
  // Your code here.
  return closed_;
}

void Writer::push( string data )
{
  // Your code here.
  auto len = min( available_capacity_, data.size() );
  if ( len < data.size() ) {
    data = data.substr( 0, len );
  }
  if ( len > 0 ) {
    bytes_pushed_ += len;
    bytes_buffered_ += len;
    available_capacity_ -= len;
    buffer_.emplace_back( move( data ) );
  }
}

void Writer::close()
{
  closed_ = true;
}

uint64_t Writer::available_capacity() const
{
    return available_capacity_;
}

uint64_t Writer::bytes_pushed() const
{
  return bytes_pushed_;
}

bool Reader::is_finished() const
{
  return closed_ && bytes_buffered_ == 0;
}

uint64_t Reader::bytes_popped() const
{
  return bytes_popped_;
}

string_view Reader::peek() const // just return one char element 
{
  if(bytes_buffered_ == 0)
    return string_view();
  // const char c = buffer_.front()[0];
  // std::string str(1, c);
  // return string_view(str);
  const string& str = buffer_.front();
  return string_view(str.data(), str.size());
}

void Reader::pop( uint64_t len )
{
  len = min( len, bytes_buffered_ );
  auto sz = len;
  while ( len > 0 ) {
    auto size = buffer_.front().size();
    if ( len >= size ) {
      buffer_.pop_front();
    } else {
      std::string str = buffer_.front();
      buffer_.pop_front();
      str = str.substr(len);
      buffer_.emplace_front(str);
      break;
    }
    len -= size;
  }
  bytes_buffered_ -= sz;
  bytes_popped_ += sz;
  available_capacity_ += sz;
}

uint64_t Reader::bytes_buffered() const
{
  return bytes_buffered_;
}
