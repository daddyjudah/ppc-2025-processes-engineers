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
    const auto &count = std::get<0>(test_param);
    const auto &root = std::get<1>(test_param);
    const auto &datatype = std::get<2>(test_param);
    const auto &name = std::get<3>(test_param);

    std::string type_str;
    if (datatype == MPI_INT) {
      type_str = "int";
    } else if (datatype == MPI_FLOAT) {
      type_str = "float";
    } else if (datatype == MPI_DOUBLE) {
      type_str = "double";
    }

    return "count" + std::to_string(count) + "_root" + std::to_string(root) + "_" + type_str + "_" + name;
  }

 protected:
  void SetUp() override {
    TestType params = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());

    const auto &count = std::get<0>(params);
    const auto &root = std::get<1>(params);
    const auto &datatype = std::get<2>(params);

    int size = 1;
    std::string test_name = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kNameTest)>(GetParam());
    bool is_mpi = test_name.find("_mpi_") != std::string::npos;
    if (is_mpi) {
      MPI_Comm_size(MPI_COMM_WORLD, &size);
    }

    if (is_mpi && root >= size) {
      GTEST_SKIP();
    }

    int type_size = 0;
    if (datatype == MPI_INT) {
      type_size = sizeof(int);
    } else if (datatype == MPI_FLOAT) {
      type_size = sizeof(float);
    } else if (datatype == MPI_DOUBLE) {
      type_size = sizeof(double);
    }

    size_t total_size = static_cast<size_t>(count) * type_size;
    if (total_size == 0) {
      total_size = 1;
    }
    std::vector<char> data(total_size);

    if (datatype == MPI_INT) {
      int *data_ptr = reinterpret_cast<int *>(data.data());
      for (int i = 0; i < count; ++i) {
        data_ptr[i] = i + 1;
      }
    } else if (datatype == MPI_FLOAT) {
      float *data_ptr = reinterpret_cast<float *>(data.data());
      for (int i = 0; i < count; ++i) {
        data_ptr[i] = static_cast<float>(i) + 0.5f;
      }
    } else if (datatype == MPI_DOUBLE) {
      double *data_ptr = reinterpret_cast<double *>(data.data());
      for (int i = 0; i < count; ++i) {
        data_ptr[i] = static_cast<double>(i) + 0.25;
      }
    }

    input_data_ = {data, count, datatype, root};
  }

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

  bool CheckTestOutputData(OutType &output_data) final {
    const auto &input = input_data_;
    std::string test_name = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kNameTest)>(GetParam());
    bool is_mpi = test_name.find("_mpi_") != std::string::npos;

    int size = 1;
    if (is_mpi) {
      MPI_Comm_size(MPI_COMM_WORLD, &size);
      int rank = 0;
      MPI_Comm_rank(MPI_COMM_WORLD, &rank);
      if (rank != input.root) {
        return true;
      }
    }

    if (output_data.empty()) {
      return false;
    }

    int type_size = GetTypeSize(input.datatype);
    size_t expected_size = static_cast<size_t>(input.count * size * type_size);

    return output_data.size() == expected_size;
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_;
};

namespace {

TEST_P(MarinLGenerTransmFrAllToOneGatherFuncTests, GatherCheck) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 6> kTestParam = {
    std::make_tuple(3, 0, MPI_INT, "SmallIntRoot0"),       std::make_tuple(5, 1, MPI_INT, "MediumIntRoot1"),
    std::make_tuple(4, 0, MPI_FLOAT, "SmallFloatRoot0"),   std::make_tuple(2, 2, MPI_FLOAT, "TinyFloatRoot2"),
    std::make_tuple(3, 0, MPI_DOUBLE, "SmallDoubleRoot0"), std::make_tuple(1, 1, MPI_DOUBLE, "SingleDoubleRoot1")};

const auto kTestTasksList = std::tuple_cat(ppc::util::AddFuncTask<MarinLGenerTransmFrAllToOneGatherMPI, InType>(
                                               kTestParam, PPC_SETTINGS_marin_l_gener_transm_fr_all_to_one_gather),
                                           ppc::util::AddFuncTask<MarinLGenerTransmFrAllToOneGatherSEQ, InType>(
                                               kTestParam, PPC_SETTINGS_marin_l_gener_transm_fr_all_to_one_gather));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName =
    MarinLGenerTransmFrAllToOneGatherFuncTests::PrintFuncTestName<MarinLGenerTransmFrAllToOneGatherFuncTests>;

INSTANTIATE_TEST_SUITE_P(GatherTests, MarinLGenerTransmFrAllToOneGatherFuncTests, kGtestValues, kPerfTestName);

TEST(MarinLGenerTransmFrAllToOneGatherMPITest, BasicMPIGather) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  std::vector<char> data(3 * sizeof(int));
  int *data_ptr = reinterpret_cast<int *>(data.data());
  data_ptr[0] = 1;
  data_ptr[1] = 2;
  data_ptr[2] = 3;

  GatherInput input{data, 3, MPI_INT, 0};
  MarinLGenerTransmFrAllToOneGatherMPI task(input);

  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());
  EXPECT_TRUE(task.Run());
  EXPECT_TRUE(task.PostProcessing());

  if (rank == 0) {
    const auto &result = task.GetOutput();
    EXPECT_FALSE(result.empty());
  }
}

TEST(MarinLGenerTransmFrAllToOneGatherSEQTest, BasicSEQGather) {
  std::vector<char> data(3 * sizeof(int));
  int *data_ptr = reinterpret_cast<int *>(data.data());
  data_ptr[0] = 1;
  data_ptr[1] = 2;
  data_ptr[2] = 3;

  GatherInput input{data, 3, MPI_INT, 0};
  MarinLGenerTransmFrAllToOneGatherSEQ task(input);

  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());
  EXPECT_TRUE(task.Run());
  EXPECT_TRUE(task.PostProcessing());

  const auto &result = task.GetOutput();
  ASSERT_EQ(result.size(), 3 * sizeof(int));

  const int *res_ptr = reinterpret_cast<const int *>(result.data());
  EXPECT_EQ(res_ptr[0], 1);
  EXPECT_EQ(res_ptr[1], 2);
  EXPECT_EQ(res_ptr[2], 3);
}

TEST(MarinLGenerTransmFrAllToOneGatherMPITest, InvalidValidation) {
  std::vector<char> data(sizeof(int));
  GatherInput input_neg_count{data, -1, MPI_INT, 0};
  MarinLGenerTransmFrAllToOneGatherMPI task_1(input_neg_count);
  EXPECT_FALSE(task_1.Validation());

  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  GatherInput input_invalid_root{data, 1, MPI_INT, size + 1};
  MarinLGenerTransmFrAllToOneGatherMPI task_2(input_invalid_root);
  EXPECT_FALSE(task_2.Validation());

  GatherInput input_wrong_type{data, 1, MPI_CHAR, 0};
  MarinLGenerTransmFrAllToOneGatherMPI task_3(input_wrong_type);
  EXPECT_FALSE(task_3.Validation());
}

TEST(MarinLGenerTransmFrAllToOneGatherMPITest, MiddleRootGather) {
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int root = size / 2;
  int count = 2;
  std::vector<char> data(count * sizeof(float));
  float *d_ptr = reinterpret_cast<float *>(data.data());
  for (int i = 0; i < count; ++i) {
    d_ptr[i] = static_cast<float>(rank);
  }

  GatherInput input{data, count, MPI_FLOAT, root};
  MarinLGenerTransmFrAllToOneGatherMPI task(input);

  ASSERT_TRUE(task.Validation());
  task.PreProcessing();
  task.Run();
  task.PostProcessing();

  if (rank == root) {
    const auto &result = task.GetOutput();
    ASSERT_EQ(result.size(), count * size * sizeof(float));
    const float *res_ptr = reinterpret_cast<const float *>(result.data());
    for (int r = 0; r < size; ++r) {
      for (int i = 0; i < count; ++i) {
        EXPECT_FLOAT_EQ(res_ptr[r * count + i], static_cast<float>(r));
      }
    }
  }
}

TEST(MarinLGenerTransmFrAllToOneGatherMPITest, LargeDataGather) {
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int count = 10000;
  std::vector<char> data(count * sizeof(double));
  double *d_ptr = reinterpret_cast<double *>(data.data());
  std::fill(d_ptr, d_ptr + count, static_cast<double>(rank));

  GatherInput input{data, count, MPI_DOUBLE, 0};
  MarinLGenerTransmFrAllToOneGatherMPI task(input);

  task.Validation();
  task.PreProcessing();
  task.Run();
  task.PostProcessing();

  if (rank == 0) {
    const auto &result = task.GetOutput();
    EXPECT_EQ(result.size(), count * size * sizeof(double));
  }
}

}  // namespace

}  // namespace marin_l_gener_transm_fr_all_to_one_gather
