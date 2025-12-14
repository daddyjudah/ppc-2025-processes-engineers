#include <gtest/gtest.h>
#include <stb/stb_image.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "marin_l_gener_transm_fr_all_to_one_gather/common/include/common.hpp"
#include "marin_l_gener_transm_fr_all_to_one_gather/mpi/include/ops_mpi.hpp"
#include "marin_l_gener_transm_fr_all_to_one_gather/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace marin_l_gener_transm_fr_all_to_one_gather {

class MarinLGenerTransmFrAllToOneGatherFuncTests : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return std::to_string(std::get<0>(test_param)) + "_" + std::get<1>(test_param);
  }

 protected:
  void SetUp() override {
    TestType params = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());
    input_data_ = std::get<0>(params);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return !output_data.empty();
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_ = 0;
};

namespace {

TEST_P(MarinLGenerTransmFrAllToOneGatherFuncTests, GatherCheck) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 3> kTestParam = {std::make_tuple(10, "gather_size_10"),
                                            std::make_tuple(50, "gather_size_50"),
                                            std::make_tuple(100, "gather_size_100")};

const auto kTestTasksList = std::tuple_cat(ppc::util::AddFuncTask<MarinLGenerTransmFrAllToOneGatherMPI, InType>(
                                               kTestParam, PPC_SETTINGS_marin_l_gener_transm_fr_all_to_one_gather),
                                           ppc::util::AddFuncTask<MarinLGenerTransmFrAllToOneGatherSEQ, InType>(
                                               kTestParam, PPC_SETTINGS_marin_l_gener_transm_fr_all_to_one_gather));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName =
    MarinLGenerTransmFrAllToOneGatherFuncTests::PrintFuncTestName<MarinLGenerTransmFrAllToOneGatherFuncTests>;

INSTANTIATE_TEST_SUITE_P(PicMatrixTests, MarinLGenerTransmFrAllToOneGatherFuncTests, kGtestValues, kPerfTestName);

}  // namespace

}  // namespace marin_l_gener_transm_fr_all_to_one_gather
