#include "CLI.h"

CLI::CLI(DefaultIO *dio) {
    this->dio = dio;
}

void CLI::start() {
    CLIData *cliData;
    bool *running = new bool;
    *running = true;
    cliData = new CLIData(dio, running, 0.9);
    /*string menu = "Welcome to the Anomaly Detection Server.\n"
                  "Please choose an option:\n"
                  "1.upload a time series csv file\n"
                  "2.algorithm settings\n"
                  "3.detect anomalies\n"
                  "4.display results\n"
                  "5.upload anomalies and analyze results\n"
                  "6.exit\n";*/
    int opt;
    while (cliData->getRunning()) {
        cliData->printMenu();
        opt = (int) stof(dio->read());
        cliData->executeCommand(opt);
        /*switch ((int) opt) {
            case 1: {
                Upload upload(dio);
                upload.execute();
                break;
            }
            case 2: {
                ChangeSettings changeSettings(dio);
                changeSettings.execute();
                break;
            }
            case 3: {
                //AnomalyDetection ad(dio);
                ad.execute();
                reports = ad.getReports();
                break;
            }
            case 4: {
                if (reports.empty()) {
                    ad.execute();
                    reports = ad.getReports();
                    break;
                }
                GetResults gr(dio, reports);
                gr.execute();
                break;
            }
            case 5: {
                Analyze analyze(dio, reports, (int) ad.tsTest.getTable().at(0).size());
                analyze.execute();
                break;
            }
            default: {}
        }*/
    }
    delete cliData;
}

CLI::~CLI() {
    //delete dio;
}

