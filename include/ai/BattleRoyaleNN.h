/**
 * BattleRoyaleNN.h - Deep Recurrent Neural Network for Battle Royale AI
 *
 * Architecture: 38 inputs → 10 hidden layers (64 each) → 4 outputs
 * Each layer has recurrent connections (hidden state persists across steps)
 * This allows temporal memory - agents can learn speed, acceleration, trends
 */

#ifndef BATTLE_ROYALE_NN_H
#define BATTLE_ROYALE_NN_H

#include <array>
#include <cmath>
#include <random>
#include <vector>

namespace AI {

// Network dimensions
constexpr size_t BR_INPUT_SIZE =
    38; // 5 enemies × 6 + 5 self + 6 zone + 2 unused
constexpr size_t BR_HIDDEN_SIZE = 64; // Size per hidden layer
constexpr size_t BR_NUM_LAYERS = 10;  // Deep RNN with 10 layers
constexpr size_t BR_OUTPUT_SIZE = 4;

// Total weights for serialization
// For each layer: input_to_hidden + hidden_to_hidden + bias
// Layer 0: INPUT×HIDDEN + HIDDEN×HIDDEN + HIDDEN
// Layers 1-9: HIDDEN×HIDDEN + HIDDEN×HIDDEN + HIDDEN
// Output: HIDDEN×OUTPUT + OUTPUT
constexpr size_t BR_TOTAL_WEIGHTS =
    BR_INPUT_SIZE * BR_HIDDEN_SIZE + // Layer 0: input weights
    BR_NUM_LAYERS * BR_HIDDEN_SIZE *
        BR_HIDDEN_SIZE + // All layers: recurrent weights
    (BR_NUM_LAYERS - 1) * BR_HIDDEN_SIZE *
        BR_HIDDEN_SIZE +             // Layers 1-9: layer-to-layer
    BR_NUM_LAYERS * BR_HIDDEN_SIZE + // All layers: biases
    BR_HIDDEN_SIZE * BR_OUTPUT_SIZE +
    BR_OUTPUT_SIZE; // Output layer

/**
 * Input indices for easier access
 * 5 enemies × 6 values = 30 enemy inputs
 */
namespace BRInput {
// Enemy inputs: base + enemy_index * 6
constexpr int ENEMY_LOCAL_X = 0;  // +0: Forward distance
constexpr int ENEMY_LOCAL_Y = 1;  // +1: Side distance
constexpr int ENEMY_LOCAL_VX = 2; // +2: Forward velocity
constexpr int ENEMY_LOCAL_VY = 3; // +3: Side velocity
constexpr int ENEMY_HEALTH = 4;   // +4: Health
constexpr int ENEMY_RELOAD = 5;   // +5: Reload timer

// Self state (indices 30-34)
constexpr int SELF_AZIMUTH = 30;
constexpr int SELF_SPEED = 31;
constexpr int SELF_HEADING = 32;
constexpr int SELF_HEALTH = 33;
constexpr int SELF_RELOAD = 34;

// Safe zone (indices 35-40)
constexpr int ZONE_DIST = 35;
constexpr int ZONE_RADIUS = 36;
constexpr int ZONE_CENTER_X = 37; // Local X to zone center
} // namespace BRInput

/**
 * BattleRoyaleNN - Deep Recurrent Neural Network
 * 10 hidden layers with persistent hidden state for temporal memory
 */
class BattleRoyaleNN {
public:
  // === WEIGHTS ===
  // Layer 0: Input to first hidden
  std::array<float, BR_INPUT_SIZE * BR_HIDDEN_SIZE> weightsInput;

  // Recurrent weights for each layer (hidden(t-1) → hidden(t))
  std::array<std::array<float, BR_HIDDEN_SIZE * BR_HIDDEN_SIZE>, BR_NUM_LAYERS>
      weightsRecurrent;

  // Layer-to-layer weights (layer[i] → layer[i+1])
  std::array<std::array<float, BR_HIDDEN_SIZE * BR_HIDDEN_SIZE>,
             BR_NUM_LAYERS - 1>
      weightsLayer;

  // Biases for each layer
  std::array<std::array<float, BR_HIDDEN_SIZE>, BR_NUM_LAYERS> biases;

  // Output layer
  std::array<float, BR_HIDDEN_SIZE * BR_OUTPUT_SIZE> weightsOutput;
  std::array<float, BR_OUTPUT_SIZE> biasOutput;

  // === HIDDEN STATE (persists across timesteps) ===
  std::array<std::array<float, BR_HIDDEN_SIZE>, BR_NUM_LAYERS> hiddenState;

  // Evolution tracking
  float fitness = 0.0f;
  int speciesId = 0;
  int age = 0;

  BattleRoyaleNN();

  void randomize(std::mt19937 &rng);
  void resetHiddenState(); // Call at start of each round

  // Forward pass - updates hidden state, returns output
  std::array<float, BR_OUTPUT_SIZE>
  forward(const std::array<float, BR_INPUT_SIZE> &input);

  // Serialization
  std::vector<float> serialize() const;
  void deserialize(const std::vector<float> &data);

  // Genetic operators
  static BattleRoyaleNN crossover(const BattleRoyaleNN &a,
                                  const BattleRoyaleNN &b, std::mt19937 &rng);
  void mutate(float rate, float strength, std::mt19937 &rng);

  // Distance metric for speciation
  float geneticDistance(const BattleRoyaleNN &other) const;
};

} // namespace AI

#endif // BATTLE_ROYALE_NN_H
