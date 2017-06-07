#include <emp-tool.h>
#include <iostream>

using namespace std;

int xor_tree(int port, EmpParty party)
{
    PRG prg;
    XorTree<> xortree(1 << 20);
    block * blocks = new block[xortree.output_size()];
    block * blocks2 = new block[xortree.input_size()];
    prg.random_block(blocks, xortree.output_size());
    double t1 = timeStamp();
    for (int i = 0; i < 20; ++i)
        xortree.circuit(blocks2, blocks);
    cout << (timeStamp() - t1) / 20 << endl;
    return 0;
}

#ifndef _MSC_VER
int main(int argc, char** argv) {
    xor_tree(1, ALICE);
    return 0;
}
#endif