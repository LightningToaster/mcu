#include <Arduino.h>
#include "hardware/sha256.h"

// Struct for internal SHA tracking
struct SHA256_Context {
  uint64_t total_bytes;
  uint8_t buffer[64];
  uint32_t buf_len;
};

// Initialize the hash context
void sha256_init_ctx(SHA256_Context* ctx) {
  ctx->total_bytes = 0;
  ctx->buf_len = 0;
  sha256_set_bswap(false);
  sha256_set_dma_size(0);
  sha256_start(); // Prepares hardware engine registers
}

// Process 64-byte chunks directly using the RP2350 Hardware accelerator
void sha256_process_block(const uint8_t* block) {
  sha256_wait_ready_blocking();
  for (int i = 0; i < 64; i++) {
    sha256_put_byte(block[i]);
  }
}

// Feed arbitrary lengths of data into the engine
void sha256_update_ctx(SHA256_Context* ctx, const uint8_t* data, size_t len) {
  for (size_t i = 0; i < len; i++) {
    ctx->buffer[ctx->buf_len++] = data[i];
    ctx->total_bytes++;
    
    if (ctx->buf_len == 64) {
      sha256_process_block(ctx->buffer);
      ctx->buf_len = 0;
    }
  }
}

// Manually append mandatory SHA-256 padding specs to close out the engine
void sha256_final_ctx(SHA256_Context* ctx, sha256_result_t* out) {
  uint64_t total_bits = ctx->total_bytes * 8;
  
  // 1. Append the mandatory '1' bit (0x80 byte)
  uint8_t bit_1 = 0x80;
  sha256_update_ctx(ctx, &bit_1, 1);
  
  // 2. Pad with zeros until we are exactly 8 bytes away from a 64-byte block boundary
  while (ctx->buf_len != 56) {
    uint8_t zero = 0x00;
    sha256_update_ctx(ctx, &zero, 1);
  }
  
  // 3. Append total bit length as a 64-bit big-endian integer
  uint8_t len_bytes[8];
  for (int i = 0; i < 8; i++) {
    len_bytes[i] = (total_bits >> (56 - i * 8)) & 0xFF;
  }
  sha256_update_ctx(ctx, len_bytes, 8);
  
  // 4. Wait for processing, then capture final calculated hardware registers
  sha256_wait_valid_blocking();
  sha256_get_result(out, SHA256_BIG_ENDIAN);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  Serial.println("\n--- RP2350 Hardware SHA-256 Fixed ---");

  const char* dataToHash = "Hello RP2350!";
  size_t dataLen = strlen(dataToHash);
  
  SHA256_Context ctx;
  sha256_result_t hashOutput;

  // Execute hash routine
  sha256_init_ctx(&ctx);
  sha256_update_ctx(&ctx, (const uint8_t*)dataToHash, dataLen);
  sha256_final_ctx(&ctx, &hashOutput);

  // Print results
  Serial.print("Data: ");
  Serial.println(dataToHash);
  Serial.print("SHA-256: ");
  for (int i = 0; i < 32; i++) {
    if (hashOutput.bytes[i] < 0x10) Serial.print("0");
    Serial.print(hashOutput.bytes[i], HEX);
  }
  Serial.println();
}

void loop() {}
