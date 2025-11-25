#include "marin_l_cnt_mismat_chrt_in_two_str/seq/include/ops_seq.hpp"

#include <algorithm>
#include <cstddef>
#include <string>

#include "marin_l_cnt_mismat_chrt_in_two_str/common/include/common.hpp"

namespace marin_l_cnt_mismat_chrt_in_two_str {

MarinLCntMismatChrtInTwoStrSEQ::MarinLCntMismatChrtInTwoStrSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool MarinLCntMismatChrtInTwoStrSEQ::ValidationImpl() {
  return true;
}

bool MarinLCntMismatChrtInTwoStrSEQ::PreProcessingImpl() {
  GetOutput() = 0;
  return true;
}

bool MarinLCntMismatChrtInTwoStrSEQ::RunImpl() {
  const std::string &s1 = GetInput().first;
  const std::string &s2 = GetInput().second;

  size_t max_len = std::max(s1.size(), s2.size());
  int count = 0;
  for (size_t i = 0; i < max_len; ++i) {
    char c1 = (i < s1.size()) ? s1[i] : '\0';
    char c2 = (i < s2.size()) ? s2[i] : '\0';
    if (c1 != c2) {
      count++;
    }
  }

  GetOutput() = count;
  return true;
}

bool MarinLCntMismatChrtInTwoStrSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace marin_l_cnt_mismat_chrt_in_two_str
