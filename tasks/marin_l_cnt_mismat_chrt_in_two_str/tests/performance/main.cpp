#include <gtest/gtest.h>

#include "marin_l_cnt_mismat_chrt_in_two_str/common/include/common.hpp"
#include "marin_l_cnt_mismat_chrt_in_two_str/mpi/include/ops_mpi.hpp"
#include "marin_l_cnt_mismat_chrt_in_two_str/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace marin_l_cnt_mismat_chrt_in_two_str {

class MarinLCntMismatChrtInTwoStrPerfTests : public ppc::util::BaseRunPerfTests<InType, OutType> {
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

TEST_P(MarinLCntMismatChrtInTwoStrPerfTests, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, MarinLCntMismatChrtInTwoStrMPI, MarinLCntMismatChrtInTwoStrSEQ>(PPC_SETTINGS_marin_l_cnt_mismat_chrt_in_two_str);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = MarinLCntMismatChrtInTwoStrPerfTests::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, MarinLCntMismatChrtInTwoStrPerfTests, kGtestValues, kPerfTestName);

}  // namespace marin_l_cnt_mismat_chrt_in_two_str
