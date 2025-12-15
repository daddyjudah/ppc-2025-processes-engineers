#include "marin_l_gener_transm_fr_all_to_one_gather/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <numeric>
#include <vector>

#include "marin_l_gener_transm_fr_all_to_one_gather/common/include/common.hpp"
#include "util/include/util.hpp"

namespace marin_l_gener_transm_fr_all_to_one_gather {

MarinLGenerTransmFrAllToOneGatherMPI::MarinLGenerTransmFrAllToOneGatherMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = {};
}

bool MarinLGenerTransmFrAllToOneGatherMPI::ValidationImpl() {
  return GetInput() > 0;
}

bool MarinLGenerTransmFrAllToOneGatherMPI::PreProcessingImpl() {
  return true;
}

bool MarinLGenerTransmFrAllToOneGatherMPI::RunImpl() {
  const int count = GetInput();
  if (count <= 0) {
    return false;
  }

  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int num_proc = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &num_proc);

  const int root = 0;
  const int total_size = num_proc * count;

  std::vector<int> send_buf(count);
  for (int i = 0; i < count; ++i) {
    send_buf[i] = rank * 1000 + i;
  }

  if (rank == root) {
    GetOutput().resize(total_size);
  }

  int send_count = count;
  int recv_count = count;

  void *recv_ptr = (rank == root) ? GetOutput().data() : nullptr;

  int mpi_result =
      MPI_Gather(send_buf.data(), send_count, MPI_INT, recv_ptr, recv_count, MPI_INT, root, MPI_COMM_WORLD);

  if (mpi_result != MPI_SUCCESS) {
    return false;
  }

  if (rank == root) {
    return GetOutput().size() == static_cast<size_t>(total_size);
  } else {
    return true;
  }
}

bool MarinLGenerTransmFrAllToOneGatherMPI::PostProcessingImpl() {
  return true;
}

}  // namespace marin_l_gener_transm_fr_all_to_one_gather
