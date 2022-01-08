/*
 * Author: 208994285 Yehuda Schwartz
 * and 318960168 Avital Gololobov
 */
#include "CLI.h"

CLI::CLI(DefaultIO *dio) {
    this->dio = dio;
}
/**
 * start CLI by creating CLIData instance and executing commands in loop
 */
void CLI::start() {
    bool *running=new bool;
    *running = true;
    CLIData *cliData = new CLIData(*dio, running);
    float opt;
    /**
     * while cliData.running is true
     * read option from client and execute the command
     */
    while (*running) {
        cliData->printMenu();
        dio->read(&opt);
        cliData->executeCommand((int) opt);
    }
    delete running;
    delete cliData;
}

CLI::~CLI() {

}

