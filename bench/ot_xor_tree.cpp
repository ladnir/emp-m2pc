#include <emp-ot.h>
#include <emp-tool.h>
#include "emp-m2pc/xor_tree_naive.h"
#include <iostream>
using namespace std;
template<typename IO, template<typename> typename T>
double test_ot(IO * io, int party, int length, T<IO>* ot = nullptr, int TIME = 10) {
	if(ot == nullptr) 
		ot = new T<IO>(io, toBlock(party));
	block *b0 = new block[length], *b1 = new block[length], *r = new block[length];
	PRG prg(fix_key);
	prg.random_block(b0, length);
	prg.random_block(b1, length);
	bool *b = new bool[length];
	for(int i = 0; i < length; ++i) {
		b[i] = (rand()%2)==1;
	}

	long long t1 = 0, t = 0;
	for(int i = 0; i < TIME; ++i) {
		io->sync();
		t1 = timeStamp();
		if (party == ALICE) {
			ot->send(b0, b1, length);
		} else {
			ot->recv(r, b, length);
		}
		t += timeStamp()-t1;
	}
	delete ot;
	delete[] b0;
	delete[] b1;
	delete[] r;
	delete[] b;
	return (double)t/TIME;
}
int main2(int argc, char** argv) {
	int port, party;
	parse_party_and_port(argv, argc, &party, &port);
	NetIO * io = new NetIO(party==ALICE ? nullptr:SERVER_IP, port);
	XorTree<40, 192> tree(65535, 40);
	MOTExtension<NetIO> * ot = new MOTExtension<NetIO>(io, toBlock(party));
	double t2 = test_ot<NetIO, MOTExtension>(io, party, tree.output_size(), ot);
	block* blocks = new block[tree.input_size()];
	block* blocks2 = new block[tree.output_size()];
	double t1 = timeStamp();
	tree.circuit(blocks, blocks2);
	t1 = 40*(timeStamp()-t1);
	io->set_nodelay();
	cout <<t1<<"\t"<<t2<<"\t"<<t1+t2<<endl;
	delete io;

    return 0;
}

int ot_xor_tree(int port, EmpParty party)
{
    double t1;
    int n[] = { 128, 1024, 8192, 65536 };
    int m[] = { 448, 1384, 8632, 66096 };
    for (int i = 0; i < 4; i++) {
        XorTree<> tree(n[i], 40);
        //	XorTreeNaive tree(n[i], m[i], 40);
        block* blocks = new block[tree.input_size()];
        block* blocks2 = new block[tree.output_size()];
        t1 = timeStamp();
        tree.circuit(blocks, blocks2);
        t1 = timeStamp() - t1;
        delete[] blocks;
        delete[] blocks2;
        cout << n[i] << "\t" << t1 * 40 << endl;
    }


    NetIO * io = new NetIO(party == ALICE ? nullptr : SERVER_IP, port);
    for (int i = 0; i<4; ++i) {
        MOTExtension<NetIO> * ot = new MOTExtension<NetIO>(io, toBlock(party), 40);
        io->set_nodelay();
        double t3 = test_ot<NetIO, MOTExtension>(io, party, 2 * n[i], ot);
        cout << n[i] << "\t" << t3 << endl;
    }
    delete io;
    return 0;
}

#ifndef _MSC_VER

int main(int argc, char** argv) {
	int port, party;
	parse_party_and_port(argv, argc, &party, &port);

    return ot_xor_tree(port, EmpParty(party));
}
#endif