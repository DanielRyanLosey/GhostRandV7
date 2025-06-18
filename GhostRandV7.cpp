#include <iostream>
#include <unordered_set>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <vector>
using namespace std;

// Typedefs for clarity
typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long long u64;

// GhostRand V6 Helpers
string buildCharset() {
    string set;
    for (int i = 32; i <= 126; ++i)
        set += static_cast<char>(i);
    return set;
}

unsigned int chaoticRand(unsigned int& state) {
    state ^= (state << 13);
    state ^= (state >> 17);
    state ^= (state << 5);
    return state;
}

string GhostRandString(unsigned int states[6], unsigned int& polishSeed, const string& charset) {
    string raw;
    for (int i = 0; i < 100; ++i) {
        for (int j = 0; j < 5; ++j)
            states[j] = chaoticRand(states[j]);
        unsigned int combined = states[0] ^ states[1] ^ states[2] ^ states[3] ^ states[4];
        unsigned int mixed = combined ^ states[5];
        states[5] = chaoticRand(mixed);
        int index = combined % charset.length();
        raw += charset[index];
    }

    // Polish Layer
    char previous = 0;
    unsigned int shuffleState = polishSeed ^ 0x3C3C3C3C;
    for (int i = 0; i < raw.size(); ++i) {
        polishSeed = chaoticRand(polishSeed);
        shuffleState = chaoticRand(shuffleState);
        unsigned char c = raw[i] ^ (polishSeed & 0xFF);
        c ^= previous;
        c ^= (shuffleState & 0xF0) >> 4;
        c = ((c << 3) | (c >> 5)) & 0xFF;
        previous = c;
        raw[i] = static_cast<char>((c % 95) + 32);
    }

    return raw;
}

// Pure C++ SHA-256 Implementation
const u32 k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

inline u32 rotr(u32 x, u32 n) { return (x >> n) | (x << (32 - n)); }
inline u32 ch(u32 x, u32 y, u32 z) { return (x & y) ^ (~x & z); }
inline u32 maj(u32 x, u32 y, u32 z) { return (x & y) ^ (x & z) ^ (y & z); }
inline u32 Sigma0(u32 x) { return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22); }
inline u32 Sigma1(u32 x) { return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25); }
inline u32 sigma0(u32 x) { return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3); }
inline u32 sigma1(u32 x) { return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10); }

string sha256(const string& input) {
    vector<u32> h = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372,
        0xa54ff53a, 0x510e527f, 0x9b05688c,
        0x1f83d9ab, 0x5be0cd19
    };

    vector<u8> msg(input.begin(), input.end());
    msg.push_back(0x80);

    while ((msg.size() * 8) % 512 != 448)
        msg.push_back(0);

    u64 bit_len = input.size() * 8;
    for (int i = 7; i >= 0; --i)
        msg.push_back((bit_len >> (i * 8)) & 0xFF);

    for (size_t i = 0; i < msg.size(); i += 64) {
        u32 w[64];
        for (int t = 0; t < 16; ++t) {
            w[t] = (msg[i + t * 4] << 24) | (msg[i + t * 4 + 1] << 16)
                | (msg[i + t * 4 + 2] << 8) | msg[i + t * 4 + 3];
        }
        for (int t = 16; t < 64; ++t)
            w[t] = sigma1(w[t - 2]) + w[t - 7] + sigma0(w[t - 15]) + w[t - 16];

        u32 a = h[0], b = h[1], c = h[2], d = h[3];
        u32 e = h[4], f = h[5], g = h[6], h0 = h[7];

        for (int t = 0; t < 64; ++t) {
            u32 T1 = h0 + Sigma1(e) + ch(e, f, g) + k[t] + w[t];
            u32 T2 = Sigma0(a) + maj(a, b, c);
            h0 = g; g = f; f = e; e = d + T1;
            d = c; c = b; b = a; a = T1 + T2;
        }

        h[0] += a; h[1] += b; h[2] += c; h[3] += d;
        h[4] += e; h[5] += f; h[6] += g; h[7] += h0;
    }

    stringstream result;
    for (int i = 0; i < 8; ++i)
        result << hex << setw(8) << setfill('0') << h[i];
    return result.str();
}

// Final Output Wrapper for GhostRand V7
string GhostRandV7(unsigned int states[6], unsigned int& polishSeed, const string& charset) {
    string v6 = GhostRandString(states, polishSeed, charset);
    return sha256(v6);
}

// Main
int main() {
    const string charset = buildCharset();
    unsigned int base = static_cast<unsigned int>(time(nullptr));
    unsigned int states[6] = { base, base + 1, base + 2, base + 3, base + 4, base + 5 };
    unsigned int polishSeed = base ^ 0xA5A5A5A5;

    for (int i = 0; i < 1000; ++i) {
        string output = GhostRandV7(states, polishSeed, charset);
        cout << "GhostRand V7 [" << i + 1 << "]: " << output << endl;
    }

    return 0;
}

