#include <emp-tool.h>
#include <string>
#include "emp-m2pc/malicious.h"
#ifdef _MSC_VER
const string circuit_file_location2 = EMP_CIRCUIT_PATH;
#else
const string circuit_file_location2 = macro_xstr(EMP_CIRCUIT_PATH);
#endif
#define AES
#ifdef AES
static char file[] = "AES-non-expanded.txt";
static const int l1 = 128;
static const int l2 = 128;
static const int l3 = 128;
#endif
#ifdef SHA1
static char file[] = "sha-1.txt";
static const int l1 = 512;
static const int l2 = 512;
static const int l3 = 160;
#endif
#ifdef ADD
static char file[] = "adder_32bit.txt";
static const  int l1 = 32;
static const  int l2 = 32;
static const  int l3 = 33;
#endif
static CircuitFile cf((circuit_file_location2 +"/" +file).c_str());
static int num_runs = 100;
void mal_dec_compute(Bit * res, Bit * in, Bit * in2) {
    cf.compute((block*)res, (block*)in, (block*)in2);
}

static string name[] = { "","setup","bobInput", "aliceInput", "gc", "recover" };

int mal_decompose(int port, EmpParty party)
{

    NetIO * io = new NetIO(party == ALICE ? nullptr : SERVER_IP, port);

    bool in[l1]; bool * output = new bool[l3];

    for (int i = 0; i < l1; ++i)
        in[i] = true;//(i%2 == 0);

    void * func = (void*)(mal_dec_compute);
    block seed = toBlock(party);

    double tt[] = { 0,0,0,0,0,0 };
    for (int k = 0; k < num_runs; ++k) {
        Malicious2PC<NetIO> mal(io, (EmpParty)party, l1, l2, l3, seed);
        double t[6];
        t[0] = wallClock();
        if (party == ALICE) {
            mal.setupAlice();
            t[1] = wallClock();
            mal.aliceInputAlice(in);
            t[2] = wallClock();
            mal.bobInputAlice();
            t[3] = wallClock();
            mal.gcAlice(func);
            t[4] = wallClock();
            mal.recoverAlice();
            t[5] = wallClock();
        }
        else {
            mal.setupBob();
            t[1] = wallClock();
            
            if (mal.aliceInputBob()) throw std::runtime_error(LOCATION);
            t[2] = wallClock();
            if (mal.bobInputBob(in)) throw std::runtime_error(LOCATION);
            t[3] = wallClock();
            if (mal.gcBob(func, output)) throw std::runtime_error(LOCATION);
            t[4] = wallClock(); 
            if (mal.recoverBob()) throw std::runtime_error(LOCATION);
            t[5] = wallClock();
        }
        io->flush();
        for (int i = 1; i <= 5; ++i)
            tt[i] += (t[i] - t[i - 1]);

    }
    double total = 0;
    for (int i = 1; i <= 5; ++i) {
        cout << party << "\t\t" << name[i] << "\t\t" << 1000 * (tt[i]) / num_runs << endl;
        total += 1000 * (tt[i]) / num_runs;
    }
    cout << party << "\t\t" << total << endl;
    delete[] output;
    delete io;
    return 0;
}


#ifndef _MSC_VER
int main(int argc, char** argv) {
    int port, party;
    parse_party_and_port(argv, argc, &party, &port);
    mal_decompose(port, (EmpParty)party);
}
#endif