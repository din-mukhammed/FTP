#include "../include/client.h"

int main() {
    TClient client("din");
    client.DownloadFile("newfile.cpp");
    return 0;
}
