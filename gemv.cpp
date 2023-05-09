#include <cstdlib>
#include <iostream>
#include <random>

#include <benchmark/benchmark.h>

#define GEMV_RESTRICT __restrict__

template <typename T> struct Input {
  std::vector<size_t> blocks; // pointers to block starts in `vals`
  std::vector<T> vals;
  std::vector<T> x;
  std::vector<T> y;

  size_t blockSize_;
  size_t stride_;

  /*! produce some space to hold a result
   */
  std::vector<T> result_space() const { return std::vector<T>(blocks * blockSize_); }

  size_t bytes() const {
    return blocks.size() * (sizeof(T) * blockSize_ * blockSize_ + sizeof(T) * blockSize_ +
                            sizeof(T) * 2 * blockSize_);
  }

  /*! blockSize:
      nBlocks:
      stride: stride between rows in the blocks
      blockAlign: alignment on which to start each block
  */
  Input(size_t blockSize, size_t nBlocks, size_t stride, size_t blockAlign, int seed = 17)
      : blockSize_(blockSize), stride_(stride) {

    std::random_device rd;
    std::mt19937 gen(seed);
    std::uniform_real_distribution<T> dis(T(-10), T(10));

    for (size_t n = 0; n < nBlocks; ++n) {

      // pointer to block start
      //   std::cerr << __FILE__ << ":" << __LINE__ << " block starts at entry " << vals.size() <<
      //   "\n";
      blocks.push_back(vals.size());

      // generate block
      for (size_t i = 0; i < blockSize; ++i) {
        size_t j;
        // row entries
        // std::cerr << __FILE__ << ":" << __LINE__ << " row starts at entry " << vals.size() <<
        // "\n";
        for (j = 0; j < blockSize; ++j) {
          vals.push_back(dis(gen));
        }
        // fill out stride
        for (; j < stride; ++j) {
          vals.resize(vals.size() + 1);
        }
      }

      // pad out values array before next block
      while ((blockAlign != 0) && (vals.size() % blockAlign)) {
        vals.resize(vals.size() + 1);
      }
    }

    for (size_t n = 0; n < nBlocks; ++n) {
      for (size_t j = 0; j < blockSize; ++j) {
        x.push_back(dis(gen));
      }
      for (size_t i = 0; i < blockSize; ++i) {
        y.push_back(dis(gen));
      }
    }
  }
};

template <typename T>
void gemv(const T alpha, const T *GEMV_RESTRICT a, const size_t aStride0, const T *GEMV_RESTRICT x,
          const T beta, T *GEMV_RESTRICT y, const size_t m) {

  for (size_t i = 0; i < m; ++i) {
    T acc = 0;
    for (size_t j = 0; j < m; ++j) {
      acc += a[i * aStride0 + j] * x[j];
    }
    y[i] = alpha * acc + beta * y[i];
  }
}

template <typename T> void bsr(Input<T> &in) {

  const size_t blockSize = in.blockSize_;
  const size_t aStride0 = in.stride_;

  for (size_t n = 0; n < in.blocks.size(); ++n) {

    const size_t off = in.blocks[n];
    const auto *GEMV_RESTRICT a = &in.vals[off];
    const auto *GEMV_RESTRICT x = &in.x[n * blockSize];
    auto *GEMV_RESTRICT y = &in.y[n * blockSize];

    gemv(1.7, a, aStride0, x, -1.5, y, blockSize);
  }
}

static void Naive(benchmark::State &state) {
  const size_t blockSize = state.range(0);

  Input<double> input(blockSize, 100000, blockSize, 0);

  for (auto _ : state) {
    bsr(input);
  }

  state.SetBytesProcessed(state.iterations() * input.bytes());
}

template <unsigned MB, typename T>
void multidot(const T alpha, const T *GEMV_RESTRICT a, const size_t aStride0,
              const T *GEMV_RESTRICT x, const T beta, T *GEMV_RESTRICT y, const size_t m) {

  // accumulators for MB rows
  T acc[MB] = {0};

  // accumulate rows
  for (size_t j = 0; j < m; ++j) {
    const T xj = x[j];
#pragma unroll(MB)
    for (size_t i = 0; i < MB; ++i) {
      acc[i] += a[i * aStride0 + j] * xj;
    }
  }

  // write back
#pragma unroll(MB)
  for (size_t i = 0; i < MB; ++i) {
    y[i] = alpha * acc[i] + beta * y[i];
  }
}

template <typename T>
void gemv_blocked_m(const T alpha, const T *GEMV_RESTRICT a, const size_t aStride0,
                    const T *GEMV_RESTRICT x, const T beta, T *GEMV_RESTRICT y, const size_t m) {

  size_t i = 0;
  for (; i + 5 <= m; i += 5) {
    multidot<5>(alpha, &a[i * aStride0], aStride0, x, beta, &y[i], m);
  }
  for (; i + 4 <= m; i += 4) {
    multidot<4>(alpha, &a[i * aStride0], aStride0, x, beta, &y[i], m);
  }
  for (; i + 3 <= m; i += 3) {
    multidot<3>(alpha, &a[i * aStride0], aStride0, x, beta, &y[i], m);
  }
  for (; i + 2 <= m; i += 2) {
    multidot<2>(alpha, &a[i * aStride0], aStride0, x, beta, &y[i], m);
  }
  for (; i + 1 <= m; i += 1) {
    multidot<1>(alpha, &a[i * aStride0], aStride0, x, beta, &y[i], m);
  }
}

template <typename T> void bsr_blocked_m(Input<T> &in) {

  const size_t blockSize = in.blockSize_;

  for (size_t n = 0; n < in.blocks.size(); ++n) {

    const size_t off = in.blocks[n];
    const auto *GEMV_RESTRICT a = &in.vals[off];
    const auto *GEMV_RESTRICT x = &in.x[n * blockSize];
    auto *GEMV_RESTRICT y = &in.y[n * blockSize];
    const size_t aStride0 = in.stride_;

    gemv_blocked_m(1.7, a, aStride0, x, -1.5, y, blockSize);
  }
}

static void BlockedM(benchmark::State &state) {
  const size_t blockSize = state.range(0);

  Input<double> input(blockSize, 100000, blockSize, 0);

  for (auto _ : state) {
    bsr_blocked_m(input);
  }

  state.SetBytesProcessed(state.iterations() * input.bytes());
}

static void BlockedM_Align128(benchmark::State &state) {
  const size_t blockSize = state.range(0);

  Input<double> input(blockSize, 100000, blockSize, 128 / sizeof(double));

  for (auto _ : state) {
    bsr_blocked_m(input);
  }

  state.SetBytesProcessed(state.iterations() * input.bytes());
}

template <unsigned NB, typename T>
void gemv_blocked_n(const T alpha, const T *GEMV_RESTRICT a, const size_t aStride0,
                    const T *GEMV_RESTRICT x, const T beta, T *GEMV_RESTRICT y, const size_t m) {

  for (size_t i = 0; i < m; ++i) {

    T acc[NB] = {0};

    size_t j;
    for (j = 0; j + NB <= m; j += NB) {
#pragma unroll(NB)
      for (size_t jj = 0; jj < NB; ++jj) {
        acc[jj] += a[i * aStride0 + j + jj] * x[j + jj];
      }
    }

    T acc0 = 0;
    for (; j < m; ++j) {
      acc0 += a[i * aStride0 + j] * x[j];
    }

    // accumulate vector into
#pragma unroll(NB)
    for (size_t jj = 0; jj < NB; ++jj) {
      acc0 += acc[jj];
    }

    y[i] = alpha * acc0 + beta * y[i];
  }
}

template <typename T> void bsr_blocked_n(Input<T> &in) {

  const size_t blockSize = in.blockSize_;

  for (size_t n = 0; n < in.blocks.size(); ++n) {

    const size_t off = in.blocks[n];
    const auto *GEMV_RESTRICT a = &in.vals[off];
    const auto *GEMV_RESTRICT x = &in.x[n * blockSize];
    auto *GEMV_RESTRICT y = &in.y[n * blockSize];
    const size_t aStride0 = in.stride_;

    gemv_blocked_n<8>(1.7, a, aStride0, x, -1.5, y, blockSize);
  }
}

static void BlockedN(benchmark::State &state) {
  const size_t blockSize = state.range(0);

  Input<double> input(blockSize, 100000, blockSize, 0);

  for (auto _ : state) {
    bsr_blocked_n(input);
  }

  state.SetBytesProcessed(state.iterations() * input.bytes());
}

BENCHMARK(Naive)->DenseRange(4, 13);
BENCHMARK(BlockedM)->DenseRange(4, 13);
BENCHMARK(BlockedM_Align128)->DenseRange(4, 13);
BENCHMARK(BlockedN)->DenseRange(4, 13);