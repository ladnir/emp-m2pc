#include <emp-tool.h>
#include <cryptoTools/Common/CLP.h>

int mal_main(int port, EmpParty party);
int mal_decompose(int port, EmpParty party);
int mal2pc(int port, EmpParty party);
int mal2pc_other(int port, EmpParty party);
int mal2pc_comb(int port, EmpParty party);
int ot_xor_tree(int port, EmpParty party);
int xor_tree(int port, EmpParty party);

typedef std::pair < std::string, std::function<int(int, EmpParty)>>  Test;

std::vector<Test> tests{
     Test{ "mal2pc_other  ", mal2pc_other },
     Test{ "xor_tree      ", xor_tree },
     Test{ "mal           ", mal_main },
     Test{ "ot_xor_tree   ", ot_xor_tree },
     Test{ "mal2pc        ", mal2pc },
     Test{ "mal2pc_comb   ", mal2pc_comb },
     Test{ "mal_decompose ", mal_decompose }
};

void doTests(osuCrypto::CLP& cmd, int port, EmpParty party, bool print = true)
{
    bool hasTestFlags = false;
    for (auto& test : tests)
        if (cmd.isSet(test.first)) hasTestFlags = true;

    for (auto& test : tests)
    {
        if (hasTestFlags == false || cmd.isSet(test.first))
        {
            if (print)std::cout << test.first << std::flush;
            test.second(port, party);
            if (print)std::cout << "passed" << std::endl;
        }
    }
}

int main(int argc, char** argv) {

    osuCrypto::CLP cmd(argc, argv);
    cmd.setDefault("port", "1212");
    auto port = cmd.get<int>("port");

    if (cmd.isSet("party"))
    {
        auto party = (EmpParty)cmd.get<int>("party");
        doTests(cmd, port, party);
    }
    else
    {
        auto thrd = std::thread([&]() { doTests(cmd, port, BOB, false); });
        doTests(cmd, port, ALICE);
        thrd.join();
    }
}
