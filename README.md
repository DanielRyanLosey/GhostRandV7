 GhostRand V7

GhostRand V7 is a custom-built recursive entropy engine developed in C++ by Daniel Losey.  
It combines chaotic bit-level RNG, multi-state feedback systems, and polish-layer mutation with cryptographic SHA-256 finalization. It is designed for use in secure systems, video games, procedural generators, and blockchain environments.

---

 Features

- ✅ Multi-state chaotic RNG (6 evolving internal states)
- ✅ Feedback-based byte polish layer
- ✅ Bitwise entropy injection and rotation
- ✅ Printable output with polish-dependent structure
- ✅ SHA-256 sealing for cryptographic finalization
- ✅ Shannon entropy score ≈ 3.99999 (near-max)
- ✅ Verifiable, deterministic, and cheat-resistant

---

 Use Cases

- Blockchain: verifiable, deterministic randomness
- Games: replayable loot tables, tile maps, gacha, and procedural content
- Simulations: consistent seed-based behaviors
- Security: unique entropy streams and key material

---

 License

Licensed under the [Apache License 2.0](http://www.apache.org/licenses/LICENSE-2.0).  
You may use, modify, and distribute this code freely **with attribution**.

> Created by **Daniel Losey**, 2025.  
> This repository and codebase serve as the original public release and timestamp of authorship.

---

 Coming Soon

- `examples/loot_system.cpp`
- `examples/tile_map.cpp`
- GhostRand V8 performance-optimized version
- C++ header-only integration
