#include "marin_l_gener_transm_fr_all_to_one_gather/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <cmath>
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

MarinLGenerTransmFrAllToOneGatherMPI::MarinLGenerTransmFrAllToOneGatherMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
}

bool MarinLGenerTransmFrAllToOneGatherMPI::ValidationImpl() {
  const auto &input = GetInput();

  if (input.data.empty()) {
    return false;
  }

  if (input.count <= 0) {
    return false;
  }

  if (input.datatype != MPI_INT && input.datatype != MPI_FLOAT && input.datatype != MPI_DOUBLE) {
    return false;
  }

  if (input.root < 0) {
    return false;
  }

  int type_size = GetTypeSize(input.datatype);
  if (input.data.size() != static_cast<size_t>(input.count * type_size)) {
    return false;
  }

#ifdef USE_MPI
  int size = 1;
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (input.root >= size) {
    return false;
  }
#endif

  return true;
}

bool MarinLGenerTransmFrAllToOneGatherMPI::PreProcessingImpl() {
  return true;
}

int MarinLGenerTransmFrAllToOneGatherMPI::TreeGatherImpl(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
                                                         void *recvbuf, int recvcount, MPI_Datatype recvtype, int root,
                                                         MPI_Comm comm) {
  int rank = 0;
  int size = 1;
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &size);

  if (sendcount != recvcount) {
    return MPI_ERR_COUNT;
  }

  if (sendtype != recvtype) {
    return MPI_ERR_TYPE;
  }

  int type_size;
  MPI_Type_size(sendtype, &type_size);
  const int block_sz = sendcount * type_size;

  if (rank == root && recvbuf == nullptr) {
    return MPI_ERR_BUFFER;
  }

  int rel_rank = (rank - root + size) % size;

  std::vector<char> tree_buffer(size * block_sz, 0);
  std::vector<int> rank_buffer(size, -1);
  std::memcpy(tree_buffer.data(), sendbuf, block_sz);
  rank_buffer[0] = rank;

  int current_blocks = 1;

  for (int step = 1; step < size; step <<= 1) {
    if (rel_rank % (2 * step) == 0) {
      int src_rel = rel_rank + step;
      if (src_rel < size) {
        int src_rank = (src_rel + root) % size;
        int blocks_to_recv = std::min(step, size - src_rel);

        MPI_Recv(tree_buffer.data() + current_blocks * block_sz, blocks_to_recv * block_sz, MPI_BYTE, src_rank, 0, comm,
                 MPI_STATUS_IGNORE);

        MPI_Recv(rank_buffer.data() + current_blocks, blocks_to_recv, MPI_INT, src_rank, 1, comm, MPI_STATUS_IGNORE);

        current_blocks += blocks_to_recv;
      }
    } else {
      int dst_rel = rel_rank - step;
      int dst_rank = (dst_rel + root) % size;

      MPI_Send(tree_buffer.data(), current_blocks * block_sz, MPI_BYTE, dst_rank, 0, comm);

      MPI_Send(rank_buffer.data(), current_blocks, MPI_INT, dst_rank, 1, comm);
      return MPI_SUCCESS;
    }
  }

  if (rank == root) {
    char *out = static_cast<char *>(recvbuf);
    for (int i = 0; i < current_blocks; ++i) {
      int r = rank_buffer[i];
      if (r >= 0 && r < size) {
        std::memcpy(out + r * block_sz, tree_buffer.data() + i * block_sz, block_sz);
      }
    }
  }

  return MPI_SUCCESS;
}

bool MarinLGenerTransmFrAllToOneGatherMPI::RunImpl() {
#ifdef USE_MPI
  int rank = 0;
  int size = 1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  const auto &input = GetInput();

  int type_size = GetTypeSize(input.datatype);

  std::vector<char> recv_buffer;
  if (rank == input.root) {
    recv_buffer.resize(input.count * size * type_size);
  }

  int result =
      TreeGatherImpl(input.data.data(), input.count, input.datatype, rank == input.root ? recv_buffer.data() : nullptr,
                     input.count, input.datatype, input.root, MPI_COMM_WORLD);

  if (rank == input.root) {
    GetOutput() = std::move(recv_buffer);
  } else {
    GetOutput() = std::vector<char>();
  }

  return true;
#else
  const auto &input = GetInput();
  GetOutput() = input.data;
  return true;
#endif
}

bool MarinLGenerTransmFrAllToOneGatherMPI::PostProcessingImpl() {
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
