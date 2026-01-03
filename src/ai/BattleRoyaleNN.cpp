/**
 * BattleRoyaleNN.cpp - Deep Recurrent Neural Network implementation
 *
 * 10 hidden layers with recurrent connections for temporal memory
 * Uses Accelerate framework for BLAS optimization on M1
 */

#include "ai/BattleRoyaleNN.h"
#include <Accelerate/Accelerate.h>
#include <algorithm>

namespace AI {

// Activation functions
static inline void applyTanh(float *data, size_t size) {
  vvtanhf(data, data, reinterpret_cast<const int *>(&size));
}

static inline void applySigmoid(float *data, size_t size) {
  for (size_t i = 0; i < size; ++i) {
    data[i] = 1.0f / (1.0f + std::exp(-data[i]));
  }
}

BattleRoyaleNN::BattleRoyaleNN() { resetHiddenState(); }

void BattleRoyaleNN::resetHiddenState() {
  for (auto &layer : hiddenState) {
    layer.fill(0.0f);
  }
}

void BattleRoyaleNN::randomize(std::mt19937 &rng) {
  // Xavier initialization for better gradient flow
  auto initLayer = [&rng](auto &weights, size_t fanIn, size_t fanOut) {
    float scale = std::sqrt(2.0f / (fanIn + fanOut));
    std::normal_distribution<float> dist(0.0f, scale);
    for (auto &w : weights) {
      w = dist(rng);
    }
  };

  // Input weights
  initLayer(weightsInput, BR_INPUT_SIZE, BR_HIDDEN_SIZE);

  // Recurrent and layer weights
  for (size_t l = 0; l < BR_NUM_LAYERS; ++l) {
    initLayer(weightsRecurrent[l], BR_HIDDEN_SIZE, BR_HIDDEN_SIZE);
    if (l < BR_NUM_LAYERS - 1) {
      initLayer(weightsLayer[l], BR_HIDDEN_SIZE, BR_HIDDEN_SIZE);
    }
    biases[l].fill(0.0f);
  }

  // Output weights
  initLayer(weightsOutput, BR_HIDDEN_SIZE, BR_OUTPUT_SIZE);
  biasOutput.fill(0.0f);

  resetHiddenState();
}

std::array<float, BR_OUTPUT_SIZE>
BattleRoyaleNN::forward(const std::array<float, BR_INPUT_SIZE> &input) {

  // Temporary buffer for layer computation
  std::array<float, BR_HIDDEN_SIZE> newHidden;

  // === LAYER 0: Input + Recurrent ===
  // newHidden = W_input * input + W_recurrent * hidden[0] + bias
  newHidden = biases[0];

  // Add input contribution
  cblas_sgemv(CblasRowMajor, CblasTrans, BR_INPUT_SIZE, BR_HIDDEN_SIZE, 1.0f,
              weightsInput.data(), BR_HIDDEN_SIZE, input.data(), 1, 1.0f,
              newHidden.data(), 1);

  // Add recurrent contribution
  cblas_sgemv(CblasRowMajor, CblasTrans, BR_HIDDEN_SIZE, BR_HIDDEN_SIZE, 1.0f,
              weightsRecurrent[0].data(), BR_HIDDEN_SIZE, hiddenState[0].data(),
              1, 1.0f, newHidden.data(), 1);

  // Apply tanh activation
  applyTanh(newHidden.data(), BR_HIDDEN_SIZE);
  hiddenState[0] = newHidden;

  // === LAYERS 1-9: Layer-to-layer + Recurrent ===
  for (size_t l = 1; l < BR_NUM_LAYERS; ++l) {
    newHidden = biases[l];

    // Add contribution from previous layer
    cblas_sgemv(CblasRowMajor, CblasTrans, BR_HIDDEN_SIZE, BR_HIDDEN_SIZE, 1.0f,
                weightsLayer[l - 1].data(), BR_HIDDEN_SIZE,
                hiddenState[l - 1].data(), 1, 1.0f, newHidden.data(), 1);

    // Add recurrent contribution
    cblas_sgemv(CblasRowMajor, CblasTrans, BR_HIDDEN_SIZE, BR_HIDDEN_SIZE, 1.0f,
                weightsRecurrent[l].data(), BR_HIDDEN_SIZE,
                hiddenState[l].data(), 1, 1.0f, newHidden.data(), 1);

    applyTanh(newHidden.data(), BR_HIDDEN_SIZE);
    hiddenState[l] = newHidden;
  }

  // === OUTPUT LAYER ===
  std::array<float, BR_OUTPUT_SIZE> output = biasOutput;
  cblas_sgemv(CblasRowMajor, CblasTrans, BR_HIDDEN_SIZE, BR_OUTPUT_SIZE, 1.0f,
              weightsOutput.data(), BR_OUTPUT_SIZE,
              hiddenState[BR_NUM_LAYERS - 1].data(), 1, 1.0f, output.data(), 1);

  applySigmoid(output.data(), BR_OUTPUT_SIZE);
  return output;
}

std::vector<float> BattleRoyaleNN::serialize() const {
  std::vector<float> data;
  data.reserve(BR_TOTAL_WEIGHTS);

  // Input weights
  data.insert(data.end(), weightsInput.begin(), weightsInput.end());

  // Recurrent and layer weights
  for (size_t l = 0; l < BR_NUM_LAYERS; ++l) {
    data.insert(data.end(), weightsRecurrent[l].begin(),
                weightsRecurrent[l].end());
    if (l < BR_NUM_LAYERS - 1) {
      data.insert(data.end(), weightsLayer[l].begin(), weightsLayer[l].end());
    }
    data.insert(data.end(), biases[l].begin(), biases[l].end());
  }

  // Output weights
  data.insert(data.end(), weightsOutput.begin(), weightsOutput.end());
  data.insert(data.end(), biasOutput.begin(), biasOutput.end());

  return data;
}

void BattleRoyaleNN::deserialize(const std::vector<float> &data) {
  size_t idx = 0;

  // Input weights
  std::copy(data.begin() + idx, data.begin() + idx + weightsInput.size(),
            weightsInput.begin());
  idx += weightsInput.size();

  // Recurrent and layer weights
  for (size_t l = 0; l < BR_NUM_LAYERS; ++l) {
    std::copy(data.begin() + idx,
              data.begin() + idx + weightsRecurrent[l].size(),
              weightsRecurrent[l].begin());
    idx += weightsRecurrent[l].size();

    if (l < BR_NUM_LAYERS - 1) {
      std::copy(data.begin() + idx, data.begin() + idx + weightsLayer[l].size(),
                weightsLayer[l].begin());
      idx += weightsLayer[l].size();
    }

    std::copy(data.begin() + idx, data.begin() + idx + biases[l].size(),
              biases[l].begin());
    idx += biases[l].size();
  }

  // Output weights
  std::copy(data.begin() + idx, data.begin() + idx + weightsOutput.size(),
            weightsOutput.begin());
  idx += weightsOutput.size();
  std::copy(data.begin() + idx, data.begin() + idx + biasOutput.size(),
            biasOutput.begin());

  resetHiddenState();
}

BattleRoyaleNN BattleRoyaleNN::crossover(const BattleRoyaleNN &a,
                                         const BattleRoyaleNN &b,
                                         std::mt19937 &rng) {
  BattleRoyaleNN child;
  std::uniform_real_distribution<float> dist(0.0f, 1.0f);

  auto crossoverArray = [&](auto &target, const auto &parentA,
                            const auto &parentB) {
    for (size_t i = 0; i < target.size(); ++i) {
      target[i] = dist(rng) < 0.5f ? parentA[i] : parentB[i];
    }
  };

  crossoverArray(child.weightsInput, a.weightsInput, b.weightsInput);

  for (size_t l = 0; l < BR_NUM_LAYERS; ++l) {
    crossoverArray(child.weightsRecurrent[l], a.weightsRecurrent[l],
                   b.weightsRecurrent[l]);
    if (l < BR_NUM_LAYERS - 1) {
      crossoverArray(child.weightsLayer[l], a.weightsLayer[l],
                     b.weightsLayer[l]);
    }
    crossoverArray(child.biases[l], a.biases[l], b.biases[l]);
  }

  crossoverArray(child.weightsOutput, a.weightsOutput, b.weightsOutput);
  crossoverArray(child.biasOutput, a.biasOutput, b.biasOutput);

  child.resetHiddenState();
  return child;
}

void BattleRoyaleNN::mutate(float rate, float strength, std::mt19937 &rng) {
  std::uniform_real_distribution<float> chance(0.0f, 1.0f);
  std::normal_distribution<float> mutation(0.0f, strength);

  auto mutateArray = [&](auto &weights) {
    for (auto &w : weights) {
      if (chance(rng) < rate) {
        w += mutation(rng);
      }
    }
  };

  mutateArray(weightsInput);

  for (size_t l = 0; l < BR_NUM_LAYERS; ++l) {
    mutateArray(weightsRecurrent[l]);
    if (l < BR_NUM_LAYERS - 1) {
      mutateArray(weightsLayer[l]);
    }
    mutateArray(biases[l]);
  }

  mutateArray(weightsOutput);
  mutateArray(biasOutput);
}

float BattleRoyaleNN::geneticDistance(const BattleRoyaleNN &other) const {
  float dist = 0.0f;
  size_t count = 0;

  for (size_t i = 0; i < weightsInput.size(); ++i) {
    dist += std::abs(weightsInput[i] - other.weightsInput[i]);
    ++count;
  }

  for (size_t i = 0; i < weightsOutput.size(); ++i) {
    dist += std::abs(weightsOutput[i] - other.weightsOutput[i]);
    ++count;
  }

  return dist / count;
}

} // namespace AI
