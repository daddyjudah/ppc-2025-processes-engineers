#include "marin_l_gener_transm_fr_all_to_one_gather/seq/include/ops_seq.hpp"

#include <numeric>
#include <vector>

#include "marin_l_gener_transm_fr_all_to_one_gather/common/include/common.hpp"
#include "util/include/util.hpp"

namespace marin_l_gener_transm_fr_all_to_one_gather {

MarinLGenerTransmFrAllToOneGatherSEQ::MarinLGenerTransmFrAllToOneGatherSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = {};
}

bool MarinLGenerTransmFrAllToOneGatherSEQ::ValidationImpl() {
  return GetInput() > 0;
}

bool MarinLGenerTransmFrAllToOneGatherSEQ::PreProcessingImpl() {
  return true;
}

bool MarinLGenerTransmFrAllToOneGatherSEQ::RunImpl() {
  const int count = GetInput();
  if (count <= 0) {
    return false;
  }

  const int num_proc = ppc::util::GetNumThreads();

  const int total_size = num_proc * count;

  GetOutput().reserve(total_size);

  for (int proc_id = 0; proc_id < num_proc; ++proc_id) {
    for (int i = 0; i < count; ++i) {
      GetOutput().push_back(proc_id * 1000 + i);
    }
  }

  return GetOutput().size() == static_cast<size_t>(total_size);
}

bool MarinLGenerTransmFrAllToOneGatherSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace marin_l_gener_transm_fr_all_to_one_gather
