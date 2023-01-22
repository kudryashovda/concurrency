#include "log_duration.h"
#include <cassert>
#include <numeric>
#include <vector>

// Compile in Debug mode to see a time difference

using namespace std;

size_t calVectorSumSeq(const vector<size_t> &vec) {
  size_t summ = 0;
  for (const auto &item : vec) {
    summ += item;
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

}