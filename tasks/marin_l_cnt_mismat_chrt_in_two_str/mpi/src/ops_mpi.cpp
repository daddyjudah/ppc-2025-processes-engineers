#include "marin_l_cnt_mismat_chrt_in_two_str/mpi/include/ops_mpi.hpp"

#include <mpi.h>
#include <algorithm>
#include <numeric>
#include <vector>

#include "marin_l_cnt_mismat_chrt_in_two_str/common/include/common.hpp"
#include "util/include/util.hpp"

namespace marin_l_cnt_mismat_chrt_in_two_str {

MarinLCntMismatChrtInTwoStrMPI::MarinLCntMismatChrtInTwoStrMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool MarinLCntMismatChrtInTwoStrMPI::ValidationImpl() {
  return !GetInput().first.empty() && !GetInput().second.empty();
}

bool MarinLCntMismatChrtInTwoStrMPI::PreProcessingImpl() {
  GetOutput() = 0;
  return true;
}

bool MarinLCntMismatChrtInTwoStrMPI::RunImpl() {
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  const std::string& s1 = GetInput().first;
  const std::string& s2 = GetInput().second;

  size_t n = std::min(s1.size(), s2.size());
  size_t chunk = n / size;
  size_t start = rank * chunk;
  size_t end = (rank == size - 1) ? n : start + chunk;

  int local_count = 0;
  for (size_t i = start; i < end; i++) {
    if (s1[i] != s2[i])
      local_count++;
  }

  int global_count = 0;
  MPI_Reduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    GetOutput() = global_count;
  }
  return true;
}

bool MarinLCntMismatChrtInTwoStrMPI::PostProcessingImpl() {
  return GetOutput() >= 0;
}

}  // namespace marin_l_cnt_mismat_chrt_in_two_str
