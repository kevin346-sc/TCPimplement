#include "wrapping_integers.hh"
#include <math.h>
using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  return Wrap32 { zero_point.raw_value_ + static_cast<uint32_t>(n) };
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  // UINT32_MAX , zero_point 10 ,  checkpoint 3 * ( 1UL << 32 )
  // UINT32_MAX , zero_point 0 ,  checkpoint 0
  // 16 , 16, 0
  uint64_t offset = this->raw_value_ >= zero_point.raw_value_ ? raw_value_ - zero_point.raw_value_ : raw_value_ + (1UL << 32) - zero_point.raw_value_; // (1UL << 32) - 1
  uint64_t base = checkpoint & 0xFFFFFFFF00000000; // 取高32位,用来判断是否需要增加
  uint64_t seq1 = offset + base;
  uint64_t seq2 = seq1 + (1UL << 32);
  uint64_t seq3 = seq1 > (1UL << 32) ? seq1 - (1UL << 32) : UINT64_MAX; 
  // 相减时可能为负数，uint64_t 不存在负数，负数会重新在正数范围循环，但在abs运算过程中允许负数
  // 如seq1 = INT_MAX, seq3 = seq1 - (1UL << 32) = UINT64_MAX(-1)

  uint64_t res = seq1;
  if(std::abs(int64_t(checkpoint - res)) > abs(int64_t(checkpoint - seq2)))
  {
    res = seq2;
  }
  if(seq3 != UINT64_MAX &&  std::abs(int64_t(checkpoint - res)) > abs(int64_t(checkpoint - seq3)))
  {
    res = seq3;
  }
  return res;

  /*version 2*/
  // const uint64_t UINT32_RANGE = 1 << 32;
  // uint64_t offset = raw_value_ - zero_point.raw_value_;
  // if(checkpoint > offset)
  // {
  //   uint64_t real_num = (checkpoint - offset) + (UINT32_RANGE >> 1);
  //   uint64_t num = real_num / UINT32_RANGE;
  //   return offset + num * UINT32_RANGE;
  // }
  // else
  //   return offset;
}
