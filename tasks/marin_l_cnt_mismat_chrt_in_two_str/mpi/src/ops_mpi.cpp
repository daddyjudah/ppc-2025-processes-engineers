#include "marin_l_cnt_mismat_chrt_in_two_str/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <cstddef>
#include <string>

#include "marin_l_cnt_mismat_chrt_in_two_str/common/include/common.hpp"

namespace marin_l_cnt_mismat_chrt_in_two_str {

MarinLCntMismatChrtInTwoStrMPI::MarinLCntMismatChrtInTwoStrMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool MarinLCntMismatChrtInTwoStrMPI::ValidationImpl() {
  return true;
}

bool MarinLCntMismatChrtInTwoStrMPI::PreProcessingImpl() {
  GetOutput() = 0;
  return true;
}

bool MarinLCntMismatChrtInTwoStrMPI::RunImpl() {
  int rank = 0;
  int size = 1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  const std::string &s1 = GetInput().first;
  const std::string &s2 = GetInput().second;

  size_t max_len = std::max(s1.size(), s2.size());
  size_t chunk = max_len / size;
  size_t start = rank * chunk;
  size_t end = (rank == size - 1) ? max_len : start + chunk;

  int local_count = 0;
  for (size_t i = start; i < end; i++) {
    char c1 = (i < s1.size()) ? s1[i] : '\0';
    char c2 = (i < s2.size()) ? s2[i] : '\0';

    if (c1 != c2) {
      local_count++;
    }
  }

  int global_count = 0;
  MPI_Allreduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

  GetOutput() = global_count;
  return true;
}

bool MarinLCntMismatChrtInTwoStrMPI::PostProcessingImpl() {
  return true;
}

}  // namespace marin_l_cnt_mismat_chrt_in_two_str
