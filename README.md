# Simplex-ARQ-opp
Simulates the Simplex ARQ Data Link Layer protocol using omnetpp.

## Features
### Framing
**Byte Stuffing** is implemented as the framing technique.

### Simulating Channel Errors
- Single-bit errors are generated at random to simulate channel errors.
- Frames are occasionally dropped to simulate the channel dropping the frame.
- Duplicate frames are occasionally sent to simulate channel errors.
 
### Error Detection/Correction techniques
The project implements two techniques for error detection and correction:
1. Parity bit
2. Hamming Code
