#include "marin_l_gener_transm_fr_all_to_one_gather/mpi/include/ops_mpi.hpp"

#include <algorithm>
#include <cstring>
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
  if (type_size <= 0) {
    return false;
  }

  if (input.data.size() != static_cast<size_t>(input.count * type_size)) {
    return false;
  }

  int size = 1;
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (input.root >= size) {
    return false;
  }

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

  int type_size = 0;
  ;
  MPI_Type_size(sendtype, &type_size);
  const int block_sz = sendcount * type_size;

  if (rank == root && recvbuf == nullptr) {
    return MPI_ERR_BUFFER;
  }

  int rel_rank = (rank - root + size) % size;

  std::vector<char> current_data(block_sz);
  std::vector<int> current_ranks = {rank};

  const auto *send_ptr = static_cast<const char *>(sendbuf);
  std::copy(send_ptr, send_ptr + block_sz, current_data.begin());

  int step = 1;
  while (step < size) {
    if (rel_rank % (2 * step) == 0) {
      int sender_rel = rel_rank + step;
      if (sender_rel < size) {
        int sender_rank = (sender_rel + root) % size;

        int num_ranks;
        MPI_Recv(&num_ranks, 1, MPI_INT, sender_rank, 100, comm, MPI_STATUS_IGNORE);

        std::vector<char> recv_data(num_ranks * block_sz);
        MPI_Recv(recv_data.data(), num_ranks * block_sz, MPI_BYTE, sender_rank, 101, comm, MPI_STATUS_IGNORE);

        std::vector<int> recv_ranks(num_ranks);
        MPI_Recv(recv_ranks.data(), num_ranks, MPI_INT, sender_rank, 102, comm, MPI_STATUS_IGNORE);

        current_data.insert(current_data.end(), recv_data.begin(), recv_data.end());
        current_ranks.insert(current_ranks.end(), recv_ranks.begin(), recv_ranks.end());
      }
    } else {
      int receiver_rel = rel_rank - step;
      int receiver_rank = (receiver_rel + root) % size;

      int num_ranks = static_cast<int>(current_ranks.size());

      MPI_Send(&num_ranks, 1, MPI_INT, receiver_rank, 100, comm);

      MPI_Send(current_data.data(), num_ranks * block_sz, MPI_BYTE, receiver_rank, 101, comm);

      MPI_Send(current_ranks.data(), num_ranks, MPI_INT, receiver_rank, 102, comm);

      break;
    }
    step *= 2;
  }

  if (rank == root) {
    char *out = static_cast<char *>(recvbuf);
    std::vector<char> full_data(static_cast<size_t>(size) * block_sz, 0);

    for (size_t i = 0; i < current_ranks.size(); ++i) {
      int r = current_ranks[i];
      if (r >= 0 && r < size) {
        std::copy(current_data.begin() + (i * block_sz), current_data.begin() + ((i + 1) * block_sz),
                  full_data.begin() + (r * block_sz));
      }
    }

    std::copy(full_data.begin(), full_data.end(), out);
  }

  return MPI_SUCCESS;
}

bool MarinLGenerTransmFrAllToOneGatherMPI::RunImpl() {
  int rank = 0;
  int size = 1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  const auto &input = GetInput();

  const int type_size = GetTypeSize(input.datatype);

  std::vector<char> recv_buffer;
  if (rank == input.root) {
    recv_buffer.resize(static_cast<size_t>(input.count) * size * type_size);
  }

  int result =
      TreeGatherImpl(input.data.data(), input.count, input.datatype, rank == input.root ? recv_buffer.data() : nullptr,
                     input.count, input.datatype, input.root, MPI_COMM_WORLD);

  if (result != MPI_SUCCESS) {
    return false;
  }

  if (rank == input.root) {
    GetOutput() = std::move(recv_buffer);
  } else {
    GetOutput() = std::vector<char>();
  }

  return true;
}

bool MarinLGenerTransmFrAllToOneGatherMPI::PostProcessingImpl() {
  return true;
}

}  // namespace marin_l_gener_transm_fr_all_to_one_gather
