#include "reassembler.hh"

using namespace std;
/*
1. Bytes that are the next bytes in the stream. The Reassembler should push these to
the stream (output .writer()) as soon as they are known.
2. Bytes that fit within the stream’s available capacity but can’t yet be written, because
earlier bytes remain unknown. These should be stored internally in the Reassembler.
3. Bytes that lie beyond the stream’s available capacity. These should be discarded. The 
Reassembler’s will not store any bytes that can’t be pushed to the ByteStream either immediately, 
or as soon as earlier bytes become known.
*/
void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  // Your code here.
  (void)first_index;
  (void)data;
  (void)is_last_substring;
  // output_.writer().push
}

uint64_t Reassembler::bytes_pending() const
{
  return 0;
}
