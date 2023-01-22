#include "log_duration.h"
#include <cassert>
#include <future>
#include <numeric>
#include <vector>

// Compile in Debug mode to see a time difference

using namespace std;

void calcPartialSum(const vector<size_t> &vec, size_t &result, size_t idx_beg,
                    size_t idx_end) {
  size_t summ = 0;
  for (size_t i = idx_beg; i < idx_end; ++i) {
    summ += vec[i];
  }
  result = summ;
}

size_t calVectorPartialSum(const vector<size_t> &vec, size_t idx_beg,
                           size_t idx_end) {
  size_t summ = 0;
  calcPartialSum(vec, summ, idx_beg, idx_end);

  return summ;
}

size_t calVectorSumSeq(const vector<size_t> &vec) {
  return calVectorPartialSum(vec, 0, vec.size());
}

size_t calcVectorSumParThread(const vector<size_t> &vec) {
  const size_t idx_vec_mid = vec.size() / 2;
  const size_t idx_vec_end = vec.size();

  size_t summ1 = 0;
  thread t1(calcPartialSum, ref(vec), ref(summ1), 0, idx_vec_mid);

  size_t summ2 = 0;
  calcPartialSum(ref(vec), summ2, idx_vec_mid, idx_vec_end);

  t1.join();

  return summ1 + summ2;
}

size_t calVectorSumParAsync(const vector<size_t> &vec) {
  const size_t idx_vec_mid = vec.size() / 2;
  const size_t idx_vec_end = vec.size();

  auto f1 = async(calVectorPartialSum, ref(vec), 0, idx_vec_mid);
  const size_t summ2 = calVectorPartialSum(vec, idx_vec_mid, idx_vec_end);

  const size_t summ1 = f1.get();

  return summ1 + summ2;
}

int main() {
  constexpr size_t num_elements = 100'000'000;
  constexpr size_t expected_summ = num_elements * (num_elements + 1) / 2;

  vector<size_t> numbers(num_elements);
  iota(numbers.begin(), numbers.end(), 1);

  {
    LOG_DURATION("calVectorSumSeq");
    const size_t summ = calVectorSumSeq(numbers);
    assert(summ == expected_summ);
  }
  {
    LOG_DURATION("calcVectorSumParThread");
    const size_t result = calcVectorSumParThread(numbers);
    assert(result == expected_summ);
  }
  {
    LOG_DURATION("calVectorSumParAsync");
    const size_t summ = calVectorSumParAsync(numbers);
    assert(summ == expected_summ);
  }
}