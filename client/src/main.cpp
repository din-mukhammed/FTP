#include "../include/client.h"

int main() {
    TClient client("Din");
    for (auto file : client.ListFiles())
        printf(">>> filename: %s\n", file.c_str());
    // client.DownloadFile("newfile.cpp");
    return 0;
}
