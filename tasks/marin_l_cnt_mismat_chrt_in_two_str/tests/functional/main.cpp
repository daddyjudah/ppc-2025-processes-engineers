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

#include "marin_l_cnt_mismat_chrt_in_two_str/common/include/common.hpp"
#include "marin_l_cnt_mismat_chrt_in_two_str/mpi/include/ops_mpi.hpp"
#include "marin_l_cnt_mismat_chrt_in_two_str/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace marin_l_cnt_mismat_chrt_in_two_str {

class MarinLCntMismatChrtInTwoStrFuncTests : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return std::to_string(std::get<0>(test_param)) + "_" + std::get<1>(test_param);
  }

 protected:
  void SetUp() override {
    TestType params = std::get<
        static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(
        GetParam());
    std::string combined = std::get<1>(params);
    auto pos = combined.find('_');
    str1_ = combined.substr(0, pos);
    str2_ = combined.substr(pos + 1);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    int expected = 0;
    for (size_t i = 0; i < std::min(str1_.size(), str2_.size()); i++) {
      if (str1_[i] != str2_[i])
        expected++;
    }
    expected += static_cast<int>(
        std::max(str1_.size(), str2_.size()) - std::min(str1_.size(), str2_.size()));
    return output_data == expected;
  }

  InType GetTestInputData() final {
    return std::make_pair(str1_, str2_);
  }

 private:
  std::string str1_;
  std::string str2_;
};

namespace {

  void CntMismatChrtInTwoStr(const std::string& str1, const std::string& str2, int result) {
    int expected = 0;
    for (size_t i = 0; i < std::min(str1.size(), str2.size()); i++) {
      if (str1[i] != str2[i])
        expected++;
    }
    expected += static_cast<int>(std::max(str1.size(), str2.size()) - std::min(str1.size(), str2.size()));
    EXPECT_EQ(result, expected) << "Failed in: '" << str1 << "' vs '" << str2 << "'";
  }



TEST_P(MarinLCntMismatChrtInTwoStrFuncTests, FindsExpectedDiffer) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 10> kFunctionalParams = {
    std::make_tuple(1, "a_a"),
    std::make_tuple(2, "a_b"),
    std::make_tuple(3, "abc_adc"),
    std::make_tuple(4, "abcd_abcf"),
    std::make_tuple(5, "abcd_abcde"),
    std::make_tuple(6, "kitten_sitting"),
    std::make_tuple(7, "12345_54321"),
    std::make_tuple(8, "_empty_abc"),
    std::make_tuple(9, "hello_HELLO"),
    std::make_tuple(10, "longstring_short")
};

const auto kTaskMatrix = std::tuple_cat(
  ppc::util::AddFuncTask<MarinLCntMismatChrtInTwoStrMPI, InType>(kFunctionalParams, PPC_SETTINGS_marin_l_cnt_mismat_chrt_in_two_str),
  ppc::util::AddFuncTask<MarinLCntMismatChrtInTwoStrSEQ, InType>(kFunctionalParams, PPC_SETTINGS_marin_l_cnt_mismat_chrt_in_two_str));

const auto kParameterizedValues = ppc::util::ExpandToValues(kTaskMatrix);

const auto kFunctionalTestName = MarinLCntMismatChrtInTwoStrFuncTests::PrintFuncTestName<MarinLCntMismatChrtInTwoStrFuncTests>;

INSTANTIATE_TEST_SUITE_P(
    MarinStringMismatchSuite,
    MarinLCntMismatChrtInTwoStrFuncTests,
    kParameterizedValues,
    kFunctionalTestName);

TEST(marin_l_cnt_mismat_chrt_in_two_str_mpi, identical_strings) {
  InType input = {"hello", "hello"};
  MarinLCntMismatChrtInTwoStrMPI task(input);
  ASSERT_TRUE(task.Validation());
  ASSERT_TRUE(task.PreProcessing());
  ASSERT_TRUE(task.Run());
  ASSERT_TRUE(task.PostProcessing());
  CntMismatChrtInTwoStr("hello", "hello", task.GetOutput());
}

TEST(marin_l_cnt_mismat_chrt_in_two_str_mpi, one_difference) {
  InType input = {"world", "worle"};
  MarinLCntMismatChrtInTwoStrMPI task(input);
  ASSERT_TRUE(task.Validation());
  ASSERT_TRUE(task.PreProcessing());
  ASSERT_TRUE(task.Run());
  ASSERT_TRUE(task.PostProcessing());
  CntMismatChrtInTwoStr("world", "worle", task.GetOutput());
}

TEST(marin_l_cnt_mismat_chrt_in_two_str_mpi, different_length) {
  InType input = {"abc", "abcdef"};
  MarinLCntMismatChrtInTwoStrMPI task(input);
  ASSERT_TRUE(task.Validation());
  ASSERT_TRUE(task.PreProcessing());
  ASSERT_TRUE(task.Run());
  ASSERT_TRUE(task.PostProcessing());
  CntMismatChrtInTwoStr("abc", "abcdef", task.GetOutput());
}

TEST(marin_l_cnt_mismat_chrt_in_two_str_mpi, both_empty) {
  InType input = {"", ""};
  MarinLCntMismatChrtInTwoStrMPI task(input);
  ASSERT_TRUE(task.Validation());
  ASSERT_TRUE(task.PreProcessing());
  ASSERT_TRUE(task.Run());
  ASSERT_TRUE(task.PostProcessing());
  CntMismatChrtInTwoStr("", "", task.GetOutput());
}

TEST(marin_l_cnt_mismat_chrt_in_two_str_mpi, empty_vs_non_empty) {
  InType input = {"", "abc"};
  MarinLCntMismatChrtInTwoStrMPI task(input);
  ASSERT_TRUE(task.Validation());
  ASSERT_TRUE(task.PreProcessing());
  ASSERT_TRUE(task.Run());
  ASSERT_TRUE(task.PostProcessing());
  CntMismatChrtInTwoStr("", "abc", task.GetOutput());
}

TEST(marin_l_cnt_mismat_chrt_in_two_str_mpi, case_sensitive) {
  InType input = {"Hello", "hello"};
  MarinLCntMismatChrtInTwoStrMPI task(input);
  ASSERT_TRUE(task.Validation());
  ASSERT_TRUE(task.PreProcessing());
  ASSERT_TRUE(task.Run());
  ASSERT_TRUE(task.PostProcessing());
  CntMismatChrtInTwoStr("Hello", "hello", task.GetOutput());
}

TEST(marin_l_cnt_mismat_chrt_in_two_str_mpi, numeric_chars) {
  InType input = {"12345", "12995"};
  MarinLCntMismatChrtInTwoStrMPI task(input);
  ASSERT_TRUE(task.Validation());
  ASSERT_TRUE(task.PreProcessing());
  ASSERT_TRUE(task.Run());
  ASSERT_TRUE(task.PostProcessing());
  CntMismatChrtInTwoStr("12345", "12995", task.GetOutput());
}

TEST(marin_l_cnt_mismat_chrt_in_two_str_mpi, very_long_strings) {
  std::string a(1000, 'a');
  std::string b(1000, 'b');
  InType input = {a, b};
  MarinLCntMismatChrtInTwoStrMPI task(input);
  ASSERT_TRUE(task.Validation());
  ASSERT_TRUE(task.PreProcessing());
  ASSERT_TRUE(task.Run());
  ASSERT_TRUE(task.PostProcessing());
  CntMismatChrtInTwoStr(a, b, task.GetOutput());
}

TEST(marin_l_cnt_mismat_chrt_in_two_str_seq, single_char_difference) {
  InType input = {"A", "B"};
  MarinLCntMismatChrtInTwoStrSEQ task(input);
  ASSERT_TRUE(task.Validation());
  ASSERT_TRUE(task.PreProcessing());
  ASSERT_TRUE(task.Run());
  ASSERT_TRUE(task.PostProcessing());
  CntMismatChrtInTwoStr("A", "B", task.GetOutput());
}

TEST(marin_l_cnt_mismat_chrt_in_two_str_seq, long_identical) {
  std::string s(200, 'x');
  InType input = {s, s};
  MarinLCntMismatChrtInTwoStrSEQ task(input);
  ASSERT_TRUE(task.Validation());
  ASSERT_TRUE(task.PreProcessing());
  ASSERT_TRUE(task.Run());
  ASSERT_TRUE(task.PostProcessing());
  CntMismatChrtInTwoStr(s, s, task.GetOutput());
}

TEST(marin_l_cnt_mismat_chrt_in_two_str_seq, swapped_chars) {
  InType input = {"abc", "acb"};
  MarinLCntMismatChrtInTwoStrSEQ task(input);
  ASSERT_TRUE(task.Validation());
  ASSERT_TRUE(task.PreProcessing());
  ASSERT_TRUE(task.Run());
  ASSERT_TRUE(task.PostProcessing());
  CntMismatChrtInTwoStr("abc", "acb", task.GetOutput());
}

TEST(marin_l_cnt_mismat_chrt_in_two_str_seq, uneven_length) {
  InType input = {"short", "shorter"};
  MarinLCntMismatChrtInTwoStrSEQ task(input);
  ASSERT_TRUE(task.Validation());
  ASSERT_TRUE(task.PreProcessing());
  ASSERT_TRUE(task.Run());
  ASSERT_TRUE(task.PostProcessing());
  CntMismatChrtInTwoStr("short", "shorter", task.GetOutput());
}

TEST(marin_l_cnt_mismat_chrt_in_two_str_seq, spaces_included) {
  InType input = {"a b c", "abc "};
  MarinLCntMismatChrtInTwoStrSEQ task(input);
  ASSERT_TRUE(task.Validation());
  ASSERT_TRUE(task.PreProcessing());
  ASSERT_TRUE(task.Run());
  ASSERT_TRUE(task.PostProcessing());
  CntMismatChrtInTwoStr("a b c", "abc ", task.GetOutput());
}

TEST(marin_l_cnt_mismat_chrt_in_two_str_seq, unicode_like_chars) {
  InType input = {"ñandú", "nandu"};
  MarinLCntMismatChrtInTwoStrSEQ task(input);
  ASSERT_TRUE(task.Validation());
  ASSERT_TRUE(task.PreProcessing());
  ASSERT_TRUE(task.Run());
  ASSERT_TRUE(task.PostProcessing());
  CntMismatChrtInTwoStr("ñandú", "nandu", task.GetOutput());
}

}  // namespace

}  // namespace marin_l_cnt_mismat_chrt_in_two_str
