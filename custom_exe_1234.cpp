#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {
    cout << "Program was passed " << argc << " args (including program name).\n";
    for (int i = 0; i < argc; i++) {
        cout << "Arg #" << i << ": " << argv[i] << "\n";
    }
    cout << "Program Signature: 5998595441\n";
    return 0;
}

