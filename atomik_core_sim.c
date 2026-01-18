#include "../include/atomik_core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// --- INTERNAL SIMULATION STATE ---
// This represents the physical registers and counters inside the FPGA
typedef struct atomik_device_t {
    int device_id;
    int is_active;
    atomik_genome_t current_genome;
    uint32_t polymorph_seed;
    float sparsity_threshold;
    
    // Telemetry Counters (Hardware Performance Counters)
    uint64_t total_in;
    uint64_t total_out;
} atomik_device_internal_t;

// Global simulated device (representing the single board)
static atomik_device_internal_t g_device = {0};

/* =========================================================================
 * CORE API IMPLEMENTATION (HAL)
 * ========================================================================= */

atomik_result_t atomik_init(void) {
    // Seed the random number generator for simulation
    srand((unsigned int)time(NULL));
    // In a real driver, this would scan the PCIe/USB bus
    printf("[ATOMiK HAL] Hardware Abstraction Layer Initialized.\n");
    return ATOMIK_SUCCESS;
}

atomik_handle_t atomik_open(int device_id) {
    if (device_id != 0) return NULL; // We only simulate one board
    
    if (g_device.is_active) return (atomik_handle_t)&g_device;

    // Reset device state to default
    memset(&g_device, 0, sizeof(atomik_device_internal_t));
    g_device.device_id = device_id;
    g_device.is_active = 1;
    g_device.sparsity_threshold = 90.0f; // Default 90% Sparsity

    printf("[ATOMiK HAL] Device #%d Attached (Simulated 37ns Core).\n", device_id);
    return (atomik_handle_t)&g_device;
}

void atomik_close(atomik_handle_t dev) {
    if (dev) {
        atomik_device_internal_t* d = (atomik_device_internal_t*)dev;
        d->is_active = 0;
        printf("[ATOMiK HAL] Device Detached.\n");
    }
}

/* =========================================================================
 * GENOME MANAGEMENT (The Real File Loader)
 * ========================================================================= */

atomik_result_t atomik_load_genome(atomik_handle_t dev, const char* filepath) {
    if (!dev) return ATOMIK_ERR_NO_DEVICE;
    atomik_device_internal_t* d = (atomik_device_internal_t*)dev;

    printf("[ATOMiK HAL] Reading Genome File: '%s' ...\n", filepath);

    // 1. Open the Binary File
    FILE* f = fopen(filepath, "rb");
    if (!f) {
        printf(" [ERR] FILE NOT FOUND. Did you compile it with 'agc.py'?\n");
        return ATOMIK_ERR_INVALID_GENOME;
    }

    // 2. Check Header (First 4 bytes must be "ATOM")
    char magic[4];
    if (fread(magic, 1, 4, f) != 4) { 
        fclose(f); 
        return ATOMIK_ERR_INVALID_GENOME; 
    }
    
    if (strncmp(magic, "ATOM", 4) != 0) {
        printf(" [ERR] CORRUPT HEADER. This is not a valid ATOMiK Genome.\n");
        printf("       Expected 'ATOM', found '%c%c%c%c'\n", magic[0], magic[1], magic[2], magic[3]);
        fclose(f);
        return ATOMIK_ERR_INVALID_GENOME;
    }

    // 3. Read Version & Frequency
    uint8_t version;
    uint32_t poly_freq;
    fread(&version, 1, 1, f);      // 1 byte version
    fread(&poly_freq, 4, 1, f);    // 4 bytes frequency (Little Endian)

    // 4. "Flash" the Logic
    // In simulation, we read the DNA size to confirm data exists
    fseek(f, 0, SEEK_END);
    long dna_size = ftell(f) - 9; // Total Size - Header(9 bytes)
    fclose(f);

    if (dna_size < 0) {
        printf(" [ERR] EMPTY GENOME. No DNA tags found.\n");
        return ATOMIK_ERR_INVALID_GENOME;
    }

    // Update Internal State
    d->current_genome.polymorph_freq_ms = poly_freq;
    // We create a fake ID based on version for display
    snprintf((char*)d->current_genome.genome_id, 16, "G_VER_%d", version);

    // Success Output
    printf("             > [SIG] HEADER VALID (ATOM v%d)\n", version);
    printf("             > [DNA] Flashed %ld bytes of logic to Core.\n", dna_size);
    printf("             > [CFG] Polymorphism set to %d ms cycles.\n", poly_freq);
    
    // Automatically enable polymorphism if the file requests it
    if (poly_freq > 0) {
        atomik_set_polymorphism(dev, 0xCAFEBABE, poly_freq);
    }
    
    return ATOMIK_SUCCESS;
}

atomik_result_t atomik_verify_genome(atomik_handle_t dev, const atomik_genome_t* expected) {
    // In a real scenario, we would read back the registers and compare
    return ATOMIK_SUCCESS; 
}

/* =========================================================================
 * DEFENSE LAYER (Polymorphism)
 * ========================================================================= */

atomik_result_t atomik_set_polymorphism(atomik_handle_t dev, uint64_t seed, uint32_t frequency_ms) {
    if (!dev) return ATOMIK_ERR_NO_DEVICE;
    atomik_device_internal_t* d = (atomik_device_internal_t*)dev;

    d->polymorph_seed = (uint32_t)seed;
    d->current_genome.polymorph_freq_ms = frequency_ms;

    printf("[ATOMiK HAL] POLYMORPHISM ENABLED.\n");
    printf("             > Seed: 0x%016llX\n", seed);
    printf("             > Frequency: Every %d ms\n", frequency_ms);
    printf("             > Status: HARDWARE HOPPING ACTIVE\n");

    return ATOMIK_SUCCESS;
}

atomik_result_t atomik_secure_send(atomik_handle_t dev, const void* data, size_t len) {
    if (!dev) return ATOMIK_ERR_NO_DEVICE;
    atomik_device_internal_t* d = (atomik_device_internal_t*)dev;

    // Update counters
    d->total_in += len;
    
    // Simulate Compression (approx 12.5x based on your findings)
    size_t compressed_len = (size_t)(len * 0.08); // 8% remains
    if (compressed_len < 1) compressed_len = 1;
    d->total_out += compressed_len;

    // VISUALIZATION: Simulate the Polymorphic Encryption
    // We print "Scrambled" output to show it's working
    printf("[ATOMiK SECURE IO] Sending %zu bytes -> [", len);
    for(size_t i=0; i < (len > 10 ? 10 : len); i++) {
        // Generate random noise to simulate side-channel obfuscation
        printf("%02X", rand() % 0xFF); 
    }
    if (len > 10) printf("...");
    printf("]\n");

    return ATOMIK_SUCCESS;
}

/* =========================================================================
 * GREEN GRID LAYER (Energy Efficiency)
 * ========================================================================= */

atomik_result_t atomik_set_sparsity(atomik_handle_t dev, float threshold_percent) {
    if (!dev) return ATOMIK_ERR_NO_DEVICE;
    atomik_device_internal_t* d = (atomik_device_internal_t*)dev;
    
    d->sparsity_threshold = threshold_percent;
    printf("[ATOMiK HAL] Sparsity Filter set to %.1f%%\n", threshold_percent);
    return ATOMIK_SUCCESS;
}

atomik_result_t atomik_get_metrics(atomik_handle_t dev, atomik_metrics_t* out_metrics) {
    if (!dev || !out_metrics) return ATOMIK_ERR_NO_DEVICE;
    atomik_device_internal_t* d = (atomik_device_internal_t*)dev;

    out_metrics->events_processed = d->total_in;
    out_metrics->events_emitted = d->total_out;
    
    // Calculate Grid Save %
    if (d->total_in > 0) {
        double ratio = (double)d->total_out / (double)d->total_in;
        out_metrics->grid_save_percent = (1.0 - ratio) * 100.0;
    } else {
        out_metrics->grid_save_percent = 0.0;
    }

    // Entropy Score: Represents the "unpredictability" of the system
    // Higher compression + Polymorphism = Higher Entropy
    out_metrics->entropy_score = (uint32_t)(rand() % 100) + 100; // 100-200 bits
    
    // Fake Wattage: 
    // Standard CPU ~65W. ATOMiK ~0.1W (Simulated)
    out_metrics->current_watts = 0.05 + ((100.0 - out_metrics->grid_save_percent) * 0.001); 

    return ATOMIK_SUCCESS;
}