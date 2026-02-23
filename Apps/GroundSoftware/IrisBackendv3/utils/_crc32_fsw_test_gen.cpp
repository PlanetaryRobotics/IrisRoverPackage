// Creates reference test data set for crc32_fsw in crc32.py
#include <iostream>
#include <cstring>
#include <iostream>
#include <sstream>
#include <bitset>
#include <stdlib.h>
#include <iomanip>

using namespace std;

static uint32_t computeCrc32(const uint8_t *bufferData, const int32_t bufferLen)
{
    int i, j;
    unsigned int byte, crc, c;
    const unsigned int      //-
        g0 = 0xED'B8'83'20, //-
        g1 = g0 >> 1,       //-
        g2 = g0 >> 2,       //-
        g3 = g0 >> 3;

    crc = 0xFF'FF'FF'FF;
    for (i = 0; i < bufferLen; i++)
    { // Get next byte.
        byte = bufferData[i];
        crc = crc ^ byte;
        for (j = 1; j >= 0; j--)
        { // Do two times.
            // clang-format off
            switch(crc & 0xF) {
                case  0: c = 0;                  break;
                case  1: c =                g3;  break;
                case  2: c =           g2;       break;
                case  3: c =           g2 ^ g3;  break;
                case  4: c =      g1;            break;
                case  5: c =      g1 ^      g3;  break;
                case  6: c =      g1 ^ g2;       break;
                case  7: c =      g1 ^ g2 ^ g3;  break;
                case  8: c = g0;                 break;
                case  9: c = g0 ^           g3;  break;
                case 10: c = g0 ^      g2;       break;
                case 11: c = g0 ^      g2 ^ g3;  break;
                case 12: c = g0 ^ g1;            break;
                case 13: c = g0 ^ g1 ^      g3;  break;
                case 14: c = g0 ^ g1 ^ g2;       break;
                case 15: c = g0 ^ g1 ^ g2 ^ g3;  break;
            }
            // clang-format on
            crc = (crc >> 4) ^ c;
            // cout << unsigned(byte) << '\t' << crc << endl;
        }
    }

    // For CRC32 we need to return the one's compliment of the result:
    return ~crc;
}

int main()
{
    cout << "Hello, world!" << endl;

    // const char data[] = {char(0xDE), char(0xAD), char(0xBE), char(0xEF), char(0x00)};
    const char data[] = "\xDE\xAD\xBE\xEF";
    // const char data[] = {char(0x48), char(0x00)};
    cout << endl
         << endl;
    uint32_t crc32 = computeCrc32((uint8_t *)data, strlen((char *)data));
    cout << endl
         << endl;
    cout << 0xDE << endl;

    cout << strlen((char *)data) << endl;
    cout << crc32 << endl;
    cout << "0x" << hex << crc32 << endl;

    // Stress test CRC32:
    cout << endl
         << endl
         << endl
         << "[" << endl;
    const unsigned T_STRING_MAX_LEN = 10;
    char tdat[T_STRING_MAX_LEN];
    const unsigned N_TESTS = 255;
    for (int i = 0; i < N_TESTS; i++)
    {
        // Pick a random length up to buffer storage max:
        unsigned len = (rand() % T_STRING_MAX_LEN) + 1;
        // Populate test buffer with N random bytes:
        for (int j = 0; j < len; j++)
        {
            tdat[j] = rand() % 0xFF;
        }
        // Compute CRC:
        uint32_t crc = computeCrc32((uint8_t *)tdat, len);

        // Print Results line (for python ingest):
        cout << "\t("
             << "0x" << setw(8) << setfill('0') << hex << crc << ",\tb'";
        for (int j = 0; j < len; j++)
        {
            cout << "\\x" << setw(2) << setfill('0') << hex << (unsigned(tdat[j]) & 0xFF);
        }
        cout << "')," << endl;
    }
    cout << "]" << endl;

    return 0;
}