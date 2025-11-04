#include "marin_l_cnt_mismat_chrt_in_two_str/seq/include/ops_seq.hpp"

#include <algorithm>
#include <numeric>
#include <vector>

#include "marin_l_cnt_mismat_chrt_in_two_str/common/include/common.hpp"
#include "util/include/util.hpp"

namespace marin_l_cnt_mismat_chrt_in_two_str {

MarinLCntMismatChrtInTwoStrSEQ::MarinLCntMismatChrtInTwoStrSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool MarinLCntMismatChrtInTwoStrSEQ::ValidationImpl() {
  return !GetInput().first.empty() && !GetInput().second.empty();
}

bool MarinLCntMismatChrtInTwoStrSEQ::PreProcessingImpl() {
  GetOutput() = 0;
  return true;
}

bool MarinLCntMismatChrtInTwoStrSEQ::RunImpl() {
  const std::string& s1 = GetInput().first;
  const std::string& s2 = GetInput().second;

  int count = 0;
  for (size_t i = 0; i < std::min(s1.size(), s2.size()); i++) {
    if (s1[i] != s2[i])
      count++;
  }
  GetOutput() = count;
  return true;
}

bool MarinLCntMismatChrtInTwoStrSEQ::PostProcessingImpl() {
  return GetOutput() >= 0;
}

}  // namespace marin_l_cnt_mismat_chrt_in_two_str
