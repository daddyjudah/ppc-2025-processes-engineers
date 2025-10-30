#pragma once

#include <string>
#include <tuple>

#include "task/include/task.hpp"

namespace marin_l_cnt_mismat_chrt_in_two_str {

using InType = int;
using OutType = int;
using TestType = std::tuple<int, std::string>;
using BaseTask = ppc::task::Task<InType, OutType>;

// static const char* PPC_ID_marin_l_cnt_mismat_chrt_in_two_str = "marin_l_cnt_mismat_chrt_in_two_str";

}  // namespace marin_l_cnt_mismat_chrt_in_two_str
