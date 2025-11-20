# Stockfish Engine Binaries

This directory contains the Stockfish chess engine executables for different platforms.

## Current Binaries

- **stockfish-windows-x86-64-avx2.exe**: Stockfish for Windows (x86-64 with AVX2)
- **stockfish-linux**: Stockfish 16 for Linux (x86-64 with AVX2)

## macOS Support

For macOS users, you have two options:

1. **Download Stockfish manually**:
   - Download from: https://stockfishchess.org/download/
   - Choose the macOS version appropriate for your system
   - Rename it to `stockfish-macos` and place it in this directory
   - Make it executable: `chmod +x stockfish-macos`

2. **Install via Homebrew**:
   ```bash
   brew install stockfish
   ```
   The application will automatically detect it in `/usr/local/bin/stockfish`

## About Stockfish

Stockfish is a free, open-source chess engine:
- **Website**: https://stockfishchess.org/
- **Source Code**: https://github.com/official-stockfish/Stockfish
- **License**: GPL-3.0
- **Strength**: Top-rated chess engine, Elo 3500+

## Version

The included binaries are Stockfish 16, released June 2023.

## Attribution

Stockfish is developed by The Stockfish developers (see AUTHORS file in the Stockfish repository).

This chess application uses Stockfish as its computer opponent, communicating via the Universal Chess Interface (UCI) protocol.
