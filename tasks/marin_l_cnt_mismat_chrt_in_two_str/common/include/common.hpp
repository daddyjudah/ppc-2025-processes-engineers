#pragma once

#include <string>
#include <tuple>

#include "task/include/task.hpp"

namespace marin_l_cnt_mismat_chrt_in_two_str {

using InType = int;
using OutType = int;
using TestType = std::tuple<int, std::string>;
using BaseTask = ppc::task::Task<InType, OutType>;

}  // namespace marin_l_cnt_mismat_chrt_in_two_str
