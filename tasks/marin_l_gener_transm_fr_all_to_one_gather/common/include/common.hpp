#pragma once

#include <mpi.h>

#include <string>
#include <tuple>
#include <vector>

#include "task/include/task.hpp"

namespace marin_l_gener_transm_fr_all_to_one_gather {

struct GatherInput {
  std::vector<char> data;
  int count;
  MPI_Datatype datatype;
  int root;
};

using InType = GatherInput;
using OutType = std::vector<char>;
using TestType = std::tuple<int, int, MPI_Datatype, std::string>;
using BaseTask = ppc::task::Task<InType, OutType>;

}  // namespace marin_l_gener_transm_fr_all_to_one_gather
