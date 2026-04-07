import sys, struct

if len(sys.argv) < 3:
    print("Usage: make_uf2.py <input.bin> <output.uf2>")
    sys.exit(1)

with open(sys.argv[1], 'rb') as f:
    data = f.read()

out = bytearray()
addr = 0x20000000             #for ram
family_id = 0xe48bff56        # ID rp2040

for i in range(0, len(data), 256):
    chunk = data[i:i+256]
    chunk += b'\x00' * (256 - len(chunk))
    
    # header 32b
    block = struct.pack('<LLLLLLLL', 
        0x0A324655, 0x9E5D5157, 0x2000, addr, 256, 
        i//256, (len(data)+255)//256, family_id
    )
    
    # add payload (256 bytes)
    block += chunk 
    
    # add for 512b
    block += b'\x00' * 220 
    
    # last 4 bytes
    block += struct.pack('<L', 0x0AB16F30) 
    
    out += block
    addr += 256

with open(sys.argv[2], 'wb') as f:
    f.write(out)
    
print(f"Generated {sys.argv[2]} (RAM Target - 512 Byte Aligned)")