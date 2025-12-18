#include "marin_l_gener_transm_fr_all_to_one_gather/seq/include/ops_seq.hpp"

#include <algorithm>
#include <cstring>
#include <numeric>
#include <vector>

#include "marin_l_gener_transm_fr_all_to_one_gather/common/include/common.hpp"
#include "util/include/util.hpp"

namespace marin_l_gener_transm_fr_all_to_one_gather {

namespace {
int GetTypeSize(MPI_Datatype datatype) {
  if (datatype == MPI_INT) {
    return sizeof(int);
  }
  if (datatype == MPI_FLOAT) {
    return sizeof(float);
  }
  if (datatype == MPI_DOUBLE) {
    return sizeof(double);
  }
  return 0;
}
}  // namespace

MarinLGenerTransmFrAllToOneGatherSEQ::MarinLGenerTransmFrAllToOneGatherSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
}

bool MarinLGenerTransmFrAllToOneGatherSEQ::ValidationImpl() {
  const auto &input = GetInput();

  if (input.data.empty()) {
    return false;
  }
  if (input.count <= 0) {
    return false;
  }
  if (input.datatype == MPI_DATATYPE_NULL) {
    return false;
  }

  if (input.root < 0) {
    return false;
  }

  return true;
}

bool MarinLGenerTransmFrAllToOneGatherSEQ::PreProcessingImpl() {
  return true;
}

bool MarinLGenerTransmFrAllToOneGatherSEQ::RunImpl() {
  const auto &input = GetInput();
  int type_size = GetTypeSize(input.datatype);
  int rank = 0;
  int size = 1;
#ifdef USE_MPI
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
#else
  size = 1;
#endif
  if (rank == input.root) {
    std::vector<char> result(input.count * size * type_size);
    for (int r = 0; r < size; ++r) {
      std::memcpy(result.data() + r * input.count * type_size, input.data.data(), input.count * type_size);
    }
    GetOutput() = std::move(result);
  } else {
    GetOutput() = std::vector<char>();
  }

  return true;
}

bool MarinLGenerTransmFrAllToOneGatherSEQ::PostProcessingImpl() {
  int rank = 0;
#ifdef USE_MPI
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif
  const auto &input = GetInput();

  if (rank == input.root) {
    return !GetOutput().empty();
  }
  return true;
}

}  // namespace marin_l_gener_transm_fr_all_to_one_gather
