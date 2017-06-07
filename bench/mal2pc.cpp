#include <emp-tool.h>
#include "bench/bench_mal2pc.h"
#include <iomanip>
#ifdef _MSC_VER
const string circuit_file_location = EMP_CIRCUIT_PATH;
#else
const string circuit_file_location = macro_xstr(EMP_CIRCUIT_PATH);
#endif
static thread_local CircuitFile *cf;
static thread_local int len1;
static thread_local int len2;
static thread_local int len3;
static thread_local NetIO *io;
static thread_local int runs = 20;
void mal2pc_compute(Bit* res, Bit * in, Bit * in2) {
	cf->compute((block*)res, (block*)in, (block*)in2);
}
void compute2(Bit * res, Bit * in, Bit * in2) {
	Bit * tin = new Bit[len1+len2];
	memcpy(tin, in, len1*16);
	memcpy(tin+len1, in2, len2*16);
	cf->compute((block*)res, (block*)tin, nullptr);
	delete[] tin;
}

void bench_sha1(int port, EmpParty party) {
	len1 = 256;
	len2 = 256;
	len3 = 160;
	string file = circuit_file_location+"/sha-1.txt";
	cf = new CircuitFile(file.c_str());
	void * f = (void *)&compute2;
	cout <<bench_mal2pc_all_online<>(f, len1, len2, len3, io, runs, party)<<"\t";
	double t[3];
	bench_mal2pc_with_offline<>(t, f, len1, len2, len3, io, runs, party);
	cout << t[0]<<"\t"<<t[1]<<"\t"<<t[2];
	delete cf;
}

void bench_sha2(int port, EmpParty party) {
	len1 = 256;
	len2 = 256;
	len3 = 256;
	string file = circuit_file_location+"/sha-256.txt";
	cf = new CircuitFile(file.c_str());
	void * f = (void *)&compute2;
	cout <<bench_mal2pc_all_online<>(f, len1, len2, len3, io, runs, party)<<"\t";
	double t[3];
	bench_mal2pc_with_offline<>(t, f, len1, len2, len3, io, runs, party);
	cout << t[0]<<"\t"<<t[1]<<"\t"<<t[2];
	delete cf;
}

void bench_aes(int port, EmpParty party) {
	len1 = 128;
	len2 = 128;
	len3 = 128;
	string file = circuit_file_location+"/AES-non-expanded.txt";
	cf = new CircuitFile(file.c_str());
	void * f = (void *)&mal2pc_compute;
	cout <<bench_mal2pc_all_online<>(f, len1, len2, len3, io, runs, party)<<"\t";
	double t[3];
	bench_mal2pc_with_offline<>(t, f, len1, len2, len3, io, runs, party);
	cout << t[0]<<"\t"<<t[1]<<"\t"<<t[2];
	delete cf;
}

void bench_add(int port, EmpParty party) {
	len1 = 32;
	len2 = 32;
	len3 = 33;

	string file = circuit_file_location+"/adder_32bit.txt";
	cf = new CircuitFile(file.c_str());
	void * f = (void *)&mal2pc_compute;
	cout <<bench_mal2pc_all_online<>(f, len1, len2, len3, io, runs, party)<<"\t";
	double t[3];
	bench_mal2pc_with_offline<>(t, f, len1, len2, len3, io, runs, party);
	cout << t[0]<<"\t"<<t[1]<<"\t"<<t[2];
	delete cf;
}

int mal2pc(int port, EmpParty party)
{

    io = new NetIO(party == ALICE ? nullptr : SERVER_IP, port);
    cout << "ADD\t"; bench_add(port,party);
    cout << endl;
    cout << "AES\t"; bench_aes(port, party);
    cout << endl;
    cout << "SHA1\t"; bench_sha1(port, party);
    cout << endl;
    cout << "SHA2\t"; bench_sha2(port, party);
    cout << endl;
    delete io;
    return 0;
}

#ifndef _MSC_VER
int main(int argc, char** argv) {
    int port, party;
	parse_party_and_port(argv,argc, &party, &port);
    mal2pc(port, (EmpParty)party);
	return 0;	
}
#endif
