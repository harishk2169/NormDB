#include <bits/stdc++.h>
#include <unistd.h>
using namespace std;

int main(int argc, char *argv[])
{
    int count = 1;
    char *c;
    string cmd;
    for (int i = 0; i < 20; i++)
    {
        cout << "Generating Process... " << count << endl;
        count++;
        cmd = "gnome-terminal -- /bin/bash -c 'g++ client.cpp -o client && ./client 127.0.0.1 3030'";
        c = strcpy(new char[cmd.length() + 1], cmd.c_str());
        system(c);
        sleep(1);
    }
    return 0;
}
