#include "log_duration.h"
#include <cassert>
#include <future>
#include <numeric>
#include <vector>

// Compile in Debug mode to see a time difference

using namespace std;

size_t calVectorPartialSum(const vector<size_t> &vec, size_t beg_idx,
                           size_t end_idx) {
  size_t summ = 0;
  for (size_t idx = beg_idx; idx < end_idx; ++idx) {
    summ += vec[idx];
  }
  return summ;
}

size_t calVectorSumSeq(const vector<size_t> &vec) {
  return calVectorPartialSum(vec, 0, vec.size());
}

size_t calVectorSumParAsync(const vector<size_t> &vec) {
  size_t idx[5];
  idx[0] = 0;
  idx[1] = vec.size() / 4;
  idx[2] = vec.size() / 4 * 2;
  idx[3] = vec.size() / 4 * 3;
  idx[4] = vec.size();

  vector<future<size_t>> futures;
  for (int i = 0; i < 4; ++i) {
    futures.push_back(async(calVectorPartialSum, ref(vec), idx[i], idx[i + 1]));
  }

  size_t summ = 0;
  for (int i = 0; i < 4; ++i) {
    summ += futures[i].get();
  }

  return summ;
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
    LOG_DURATION("calVectorSumParAsync");
    const size_t summ = calVectorSumParAsync(numbers);
    assert(summ == expected_summ);
  }
}