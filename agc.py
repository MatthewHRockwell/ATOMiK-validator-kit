import json
import struct
import sys
import os

# --- THE BASE-4 PHYSICS ENGINE ---
# We map human concepts to 2-bit DNA logic
TAG_VOID   = 0  # 00
TAG_INGEST = 1  # 01
TAG_MUTATE = 2  # 10
TAG_EMIT   = 3  # 11

def translate_to_dna(instruction):
    """
    Translates a high-level command into a sequence of Base-4 tags.
    This is where the 'Compression' happens.
    """
    cmd = instruction.upper()
    dna_seq = []

    # Example Translation Logic
    if "FILTER" in cmd:
        # Filter = INGEST + MUTATE + EMIT
        dna_seq = [TAG_INGEST, TAG_MUTATE, TAG_EMIT]
    elif "ENCRYPT" in cmd:
        # Encrypt = INGEST + MUTATE + MUTATE + EMIT
        dna_seq = [TAG_INGEST, TAG_MUTATE, TAG_MUTATE, TAG_EMIT]
    elif "STORE" in cmd:
        # Store = INGEST + VOID + EMIT (No processing)
        dna_seq = [TAG_INGEST, TAG_VOID, TAG_EMIT]
    else:
        # Default: Just pass it through (Void)
        dna_seq = [TAG_VOID]
        
    return dna_seq

def compile_genome(json_file):
    print(f"--- ATOMiK GENOME TRANSLATOR v1.0 ---")
    print(f" [IN]  Reading Schema: {json_file}")
    
    with open(json_file, 'r') as f:
        schema = json.load(f)

    # 1. Extract Meta Data
    name = schema['meta']['name']
    security = schema['policy']['security_level']
    print(f" [NFO] Target: {name} ({security})")

    # 2. Synthesize the DNA (The 1-of-4 Map)
    raw_dna = []
    
    # Process the 'dna' block from the JSON
    for reg, op in schema['dna'].items():
        # Translate English -> Base-4 Sequence
        seq = translate_to_dna(op)
        raw_dna.extend(seq)
        
    # 3. Pack into Binary (2 bits per tag)
    # We pack 4 tags into a single byte (Tightest possible compression)
    packed_bytes = bytearray()
    current_byte = 0
    shift = 6 # Start at top bits (11 00 00 00)
    
    for tag in raw_dna:
        current_byte |= (tag << shift)
        shift -= 2
        if shift < 0:
            packed_bytes.append(current_byte)
            current_byte = 0
            shift = 6
            
    # Flush remaining bits
    if shift != 6:
        packed_bytes.append(current_byte)

    # 4. Save the .gnm File
    output_filename = schema['meta']['id'] + ".gnm"
    
    # Header: Magic "ATOM" + 1 byte Version + 1 byte Polymorph Freq
    poly_freq = schema['mutation'].get('scramble_freq_ms', 0)
    # Simple Header Struct
    header = struct.pack('4sBI', b'ATOM', 1, poly_freq)
    
    with open(output_filename, 'wb') as out:
        out.write(header)
        out.write(packed_bytes)

    print(f" [OUT] Synthesized: {output_filename}")
    print(f" [DNA] Length: {len(raw_dna)} tags -> Packed to {len(packed_bytes)} bytes")
    print(f" [RATIO] Compression: {len(raw_dna)*4 / len(packed_bytes) if len(packed_bytes) > 0 else 0:.1f}x (Effective)")
    print("-" * 40)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python agc.py <schema.json>")
    else:
        compile_genome(sys.argv[1])