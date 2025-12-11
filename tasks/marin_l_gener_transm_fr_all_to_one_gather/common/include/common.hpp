#pragma once

#include <string>
#include <tuple>

#include "task/include/task.hpp"

namespace marin_l_gener_transm_fr_all_to_one_gather {

using InType = int;
using OutType = int;
using TestType = std::tuple<int, std::string>;
using BaseTask = ppc::task::Task<InType, OutType>;

}  // namespace marin_l_gener_transm_fr_all_to_one_gather
