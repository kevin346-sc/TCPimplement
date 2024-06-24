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
    // Writer writer = Reassembler::writer(); // 不能获取到output_.writer()
    // Writer writer = output_.writer(); // 不能获取到output_.writer()
    if (is_last_substring)
    {
        last_string = true;
        max_index = first_index + data.size();
    }

    if(output_.writer().available_capacity() == 0)
    {
        return;
    }

    if(data.empty())
    {
        if(last_string && first_unassembled_index == max_index)
        {
            output_.writer().close();
        }
        return;
    }

    uint64_t last_index = first_index + data.size();
    uint64_t first_unaccepted_index = first_unassembled_index + output_.writer().available_capacity();

    if (last_index <= first_unassembled_index || first_index >= first_unaccepted_index) {
        return;
    }

    if (first_index < first_unassembled_index) {
        data = data.substr(first_unassembled_index - first_index);
        first_index = first_unassembled_index;
    }

    if (last_index > first_unaccepted_index) {
        data = data.substr(0, first_unaccepted_index - first_index);
        last_index = first_unaccepted_index;
    }

    /*
        1. seg_first_index == first_index && seg_last_index == last_index
            return
        2. seg_first_index <= first_index
            1. seg_last_index >= last_index
                data = seg_data
                first_index = seg_first_index
                last_index = seg_last_index
            2. seg_last_index < first_index
                it++
                continue;
            3. seg_last_index < last_index
                data = seg_data + data.substr(seg_last_index - first_index)
        3. seg_first_index > first_index
            1. seg_last_index <= last_index
                ---
            2. seg_first_index > last_index
                break, 直接输入
            3. seg_last_index > last_index
                data = data + seg_data.substr(last_index - seg_first_index)
    */    
    auto it = reassembler_buffer_.lower_bound(first_index);
    if (it != reassembler_buffer_.begin()) {
        it--;
    }
    while (it != reassembler_buffer_.end()) {
        auto [seg_first_index, seg_data] = *it;
        uint64_t seg_last_index = seg_first_index + seg_data.size();
        if (seg_first_index == first_index && seg_last_index == last_index)
            return;
        if (seg_first_index <= first_index)
        {
            if (seg_last_index < first_index)
            {
                it++;
                continue;
            }
            else if (seg_last_index < last_index)
            {
                data = seg_data + data.substr(seg_last_index - first_index);
                first_index = seg_first_index;
            }
            else 
            // if (seg_last_index >= last_index)
            {
                data = seg_data;
                first_index = seg_first_index;
                last_index = seg_last_index;
            }
        }
        else 
        // if (seg_first_index > first_index)
        {
            if (seg_first_index > last_index)
                break;
            // else if (seg_last_index <= last_index)
            // {

            // }
            else if (seg_last_index > last_index)
            {
                data = data + seg_data.substr(last_index - seg_first_index);
                last_index = seg_last_index;
            }
        }
        bytes_pending_ -= seg_data.size();
        it = reassembler_buffer_.erase(it);
    }

    reassembler_buffer_[first_index] = data;
    bytes_pending_ += data.size();

    while (!reassembler_buffer_.empty()) {
        auto [idx, str] = *reassembler_buffer_.begin();
        if (idx != first_unassembled_index) {
            break;
        }
        output_.writer().push(str);
        bytes_pending_ -= str.size();
        reassembler_buffer_.erase(reassembler_buffer_.begin());
        first_unassembled_index += str.size();
    }

    if(last_string && max_index <= first_unassembled_index)
    {
        output_.writer().close();
    }
}   

uint64_t Reassembler::bytes_pending() const
{
  return bytes_pending_;
}
