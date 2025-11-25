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

  size_t len1 = 0;
  size_t len2 = 0;

  std::string s1; 
  std::string s2;

  if (rank == 0) {
      s1 = GetInput().first;
      s2 = GetInput().second;
      len1 = s1.size();
      len2 = s2.size();
    }

  int len1_i = static_cast<int>(len1);
  int len2_i = static_cast<int>(len2);
  MPI_Bcast(&len1, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);
  MPI_Bcast(&len2, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);

  len1 = static_cast<size_t>(len1_i);
  len2 = static_cast<size_t>(len2_i);

  if (rank != 0) {
      s1.resize(len1);
      s2.resize(len2);
    }

  MPI_Bcast(s1.data(), len1_i, MPI_CHAR, 0, MPI_COMM_WORLD);
  MPI_Bcast(s2.data(), len2_i, MPI_CHAR, 0, MPI_COMM_WORLD);

  size_t min_len = std::min(len1, len2);
  size_t max_len = std::max(len1, len2);

  size_t chunk = min_len / size;
  size_t start = rank * chunk;
  size_t end = (rank == size - 1) ? min_len : start + chunk;

  int local_count = 0;

  for (size_t i = start; i < end; ++i) {
      if (s1[i] != s2[i]) {
        local_count++;
      }
  }

  int global_count = 0;
  MPI_Allreduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

  if (rank == 0) {
      global_count += static_cast<int>(max_len - min_len);
  }

  GetOutput() = global_count;
  return true;
}

bool MarinLCntMismatChrtInTwoStrMPI::PostProcessingImpl() {
  return true;
}

}  // namespace marin_l_cnt_mismat_chrt_in_two_str
