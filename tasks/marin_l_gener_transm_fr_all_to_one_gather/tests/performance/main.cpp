#include <gtest/gtest.h>

#include <tuple>
#include <vector>

#include "marin_l_gener_transm_fr_all_to_one_gather/common/include/common.hpp"
#include "marin_l_gener_transm_fr_all_to_one_gather/mpi/include/ops_mpi.hpp"
#include "marin_l_gener_transm_fr_all_to_one_gather/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

#ifdef USE_MPI
#  include <mpi.h>
#endif

namespace marin_l_gener_transm_fr_all_to_one_gather {

class MarinLGenerTransmFrAllToOneGatherPerfTests : public ppc::util::BaseRunPerfTests<InType, OutType> {
 protected:
  static const size_t kDataCount = 10000000;
  static const MPI_Datatype kDataType = MPI_INT;

  InType input_data_{};

  void SetUp() override {
    const int type_size = sizeof(int);
    std::vector<char> data(kDataCount * type_size);

#ifdef USE_MPI
    int rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int *data_ptr = reinterpret_cast<int *>(data.data());
    for (size_t i = 0; i < kDataCount; ++i) {
      data_ptr[i] = static_cast<int>(rank * kDataCount + i);
    }
#else
    int *data_ptr = reinterpret_cast<int *>(data.data());
    for (size_t i = 0; i < kDataCount; ++i) {
      data_ptr[i] = static_cast<int>(i);
    }
#endif
    int root = 0;
    input_data_ = {data, static_cast<int>(kDataCount), kDataType, root};
  }

  bool CheckTestOutputData(OutType &output_data) final {
    const auto &input = input_data_;
#ifdef USE_MPI
    int size = 1;
    int rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank != input.root) {
      return output_data.empty();
    }

    const int type_size = sizeof(int);
    if (output_data.size() != static_cast<size_t>(input.count * size * type_size)) {
      return false;
    }

    const int *result_ptr = reinterpret_cast<const int *>(output_data.data());
    for (int proc = 0; proc < size; ++proc) {
      for (int i = 0; i < input.count; ++i) {
        int expected = proc * input.count + i;
        if (result_ptr[proc * input.count + i] != expected) {
          return false;
        }
      }
    }

    return true;

#else
    const int type_size = sizeof(int);
    return output_data.size() == static_cast<size_t>(input.count * type_size);
#endif
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(MarinLGenerTransmFrAllToOneGatherPerfTests, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, MarinLGenerTransmFrAllToOneGatherMPI, MarinLGenerTransmFrAllToOneGatherSEQ>(
        PPC_SETTINGS_marin_l_gener_transm_fr_all_to_one_gather);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = MarinLGenerTransmFrAllToOneGatherPerfTests::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, MarinLGenerTransmFrAllToOneGatherPerfTests, kGtestValues, kPerfTestName);

}  // namespace marin_l_gener_transm_fr_all_to_one_gather
