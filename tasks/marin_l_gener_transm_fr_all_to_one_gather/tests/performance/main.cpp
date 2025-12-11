#include <gtest/gtest.h>

#include "marin_l_gener_transm_fr_all_to_one_gather/common/include/common.hpp"
#include "marin_l_gener_transm_fr_all_to_one_gather/mpi/include/ops_mpi.hpp"
#include "marin_l_gener_transm_fr_all_to_one_gather/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace marin_l_gener_transm_fr_all_to_one_gather {

class ExampleRunPerfTestProcesses2 : public ppc::util::BaseRunPerfTests<InType, OutType> {
  const int kCount_ = 100;
  InType input_data_{};

  void SetUp() override {
    input_data_ = kCount_;
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return input_data_ == output_data;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(ExampleRunPerfTestProcesses2, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, MarinLGenerTransmFrAllToOneGatherMPI, MarinLGenerTransmFrAllToOneGatherSEQ>(
        PPC_SETTINGS_marin_l_gener_transm_fr_all_to_one_gather);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = ExampleRunPerfTestProcesses2::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, ExampleRunPerfTestProcesses2, kGtestValues, kPerfTestName);

}  // namespace marin_l_gener_transm_fr_all_to_one_gather
