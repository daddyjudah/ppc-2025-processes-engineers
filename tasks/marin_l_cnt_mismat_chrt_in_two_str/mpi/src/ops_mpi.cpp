#include "marin_l_cnt_mismat_chrt_in_two_str/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

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

  const std::string &s1_full = GetInput().first;
  const std::string &s2_full = GetInput().second;

  size_t max_len = std::max(s1_full.size(), s2_full.size());

  if (max_len == 0) {
    GetOutput() = 0;
    return true;
  }

  std::vector<int> sendcounts(size);
  std::vector<int> displs(size);

  size_t chunk = max_len / size;
  size_t rem = max_len % size;

  size_t offset = 0;
  for (int i = 0; i < size; i++) {
    size_t len = chunk + (i < rem ? 1 : 0);
    sendcounts[i] = static_cast<int>(len);
    displs[i] = static_cast<int>(offset);
    offset += len;
  }

  std::string s1_local(sendcounts[rank], '\0');
  std::string s2_local(sendcounts[rank], '\0');

  MPI_Scatterv(s1_full.data(), sendcounts.data(), displs.data(), MPI_CHAR, s1_local.data(), sendcounts[rank], MPI_CHAR,
               0, MPI_COMM_WORLD);

  MPI_Scatterv(s2_full.data(), sendcounts.data(), displs.data(), MPI_CHAR, s2_local.data(), sendcounts[rank], MPI_CHAR,
               0, MPI_COMM_WORLD);

  int local_count = 0;
  for (size_t i = 0; i < s1_local.size(); i++) {
    char c1 = s1_local[i];
    char c2 = s2_local[i];
    if (c1 != c2) {
      local_count++;
    }
  }

  int global_count = 0;
  MPI_Reduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  MPI_Bcast(&global_count, 1, MPI_INT, 0, MPI_COMM_WORLD);

  GetOutput() = global_count;
  return true;
}

bool MarinLCntMismatChrtInTwoStrMPI::PostProcessingImpl() {
  return true;
}

}  // namespace marin_l_cnt_mismat_chrt_in_two_str
