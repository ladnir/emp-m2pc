#include <emp-tool.h>
#include "emp-m2pc/malicious.h"
#define AES
#ifdef _MSC_VER
const string circuit_file_location = EMP_CIRCUIT_PATH;
#else
const string circuit_file_location = macro_xstr(EMP_CIRCUIT_PATH);
#endif
static string file = circuit_file_location + "/AES-non-expanded.txt";
static const int l1 = 128;
static const int l2 = 128;
static const int l3 = 128;
static CircuitFile cf(file.c_str());
void  mal_compute(Bit * res, Bit * in, Bit * in2) {
    for (int i = 0; i < 1; ++i)
        res[i] = in[i] & in2[i];
    //	cf.compute((block*)res, (block*)in, (block*)in2);
}


int mal_main(int port, EmpParty party)
{
    block seed = toBlock(party);

    NetIO * io = new NetIO(party == ALICE ? nullptr : "127.0.0.1", port);
    bool in[l1]; bool * output = new bool[l3];
    for (int i = 0; i < l1; ++i)
        in[i] = true;//(i%2 == 0);
    void * f = (void*)(mal_compute);
    Malicious2PC<NetIO, RTCktOpt::off> mal(io, party, l1, l2, l3, seed);
    double t1 = wallClock();
    if (party == ALICE) {
        mal.alice_run(f, in);
    }
    else {
        mal.bob_run(f, in, output);
    }
    double t2 = wallClock() - t1;
    cout << "time " << t2 << endl;
    delete[] output;
    delete io;

    return 0;
}

#ifndef _MSC_VER
int main(int argc, char** argv) {

    if (argc > 1)
    {
        int port, party;
        parse_party_and_port(argv, argc, &party, &port);
        mal_main(port, (EmpParty)party);
    }
    else
    {
#ifdef THREADING
        auto thrd = std::thread([&]() {mal_main(1212, EmpParty::ALICE); });
        mal_main(1212, EmpParty::BOB);
        thrd.join();
#else
        std::cout << "please provide two args, (port partyIdx)" << std::endl;
#endif
    }

    return 0;

}
#endif