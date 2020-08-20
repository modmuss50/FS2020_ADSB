#pragma once

#include <sstream>
#include <strsafe.h>
#include <iostream>

#include "tcp_server.h"
#include "SimConnect.h"

int quit = 0;
HANDLE hSimConnect = NULL;
tcp::Server socketServer = tcp::Server(30003);

struct PlaneData {
    char title[256];
    char tailnumber[64];
    char flightnum[8];
    double altitude;
    double latitude;
    double longitude;
};

const int SIM_UPDATE_EVENT = 0;

const int DATA_DEFINE_PLANE_VALUES = 0;
const int DATA_REQUEST_SIMOBJECT_TYPE_USER_REQUEST = 0;

void CALLBACK DispatchProcRD(SIMCONNECT_RECV *pData, DWORD cbData, void *pContext) {
    HRESULT hr;

    switch (pData->dwID) {
        case SIMCONNECT_RECV_ID_EVENT: {
            SIMCONNECT_RECV_EVENT *evt = (SIMCONNECT_RECV_EVENT *) pData;

            switch (evt->uEventID) {
                case SIM_UPDATE_EVENT:

                    // Now the sim is running, request information on the user aircraft
                    hr = SimConnect_RequestDataOnSimObjectType(hSimConnect, DATA_REQUEST_SIMOBJECT_TYPE_USER_REQUEST, DATA_DEFINE_PLANE_VALUES, 0,
                                                               SIMCONNECT_SIMOBJECT_TYPE_USER);

                    break;

                default:
                    break;
            }
            break;
        }

        case SIMCONNECT_RECV_ID_SIMOBJECT_DATA_BYTYPE: {
            SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE *pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE *) pData;

            switch (pObjData->dwRequestID) {
                case DATA_REQUEST_SIMOBJECT_TYPE_USER_REQUEST: {
                    DWORD ObjectID = pObjData->dwObjectID;
                    PlaneData *pS = (PlaneData *) &pObjData->dwData;

                    if (SUCCEEDED(StringCbLengthA(&pS->title[0], sizeof(pS->title), NULL))) {
                        // http://woodair.net/sbs/Article/Barebones42_Socket_Data.htm
                        std::ostringstream positionMessage;
                        positionMessage << "MSG,3,1,1,43c813,1,";
                        positionMessage << "2019/12/10,19:10:46.320,"; // message reception time and date
                        positionMessage << "2019/12/10,19:10:47.789,"; // current time and date
                        positionMessage << "MSFS2020,"; // Callsign
                        positionMessage << (int) pS->altitude << ","; // Alt
                        positionMessage << ","; // Ground speed
                        positionMessage << ","; // Ground Heading
                        positionMessage << pS->latitude << ","; // Lat
                        positionMessage << pS->longitude << ","; // Lng
                        positionMessage << ","; // Vertical Rate
                        positionMessage << "7000,"; // Squawk
                        positionMessage << ","; // Squawk Changing Alert
                        positionMessage << ","; // Squawk Emergency flag
                        positionMessage << ","; // Squawk Ident flag
                        positionMessage << ","; // OnTheGround flag
                        positionMessage << "\r\n";

                        socketServer.sendMessage(positionMessage.str());
                        std::cout << positionMessage.str();
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        }


        case SIMCONNECT_RECV_ID_QUIT: {
            quit = 1;
            break;
        }

        default:
            printf("Received:%d\n", pData->dwID);
            break;
    }
}

int main() {
    printf("Starting connection \n");
    socketServer.startNoneBlocking();

    if (SUCCEEDED(SimConnect_Open(&hSimConnect, "Request Data", NULL, 0, 0, 0))) {
        printf("\nConnected to Flight Simulator!");

        HRESULT hr;
        // Set up the data definition, but do not yet do anything with it
        hr = SimConnect_AddToDataDefinition(hSimConnect, DATA_DEFINE_PLANE_VALUES, "Title", NULL, SIMCONNECT_DATATYPE_STRING256);
        hr = SimConnect_AddToDataDefinition(hSimConnect, DATA_DEFINE_PLANE_VALUES, "ATC id", NULL, SIMCONNECT_DATATYPE_STRING64);
        hr = SimConnect_AddToDataDefinition(hSimConnect, DATA_DEFINE_PLANE_VALUES, "ATC Flight Number", NULL, SIMCONNECT_DATATYPE_STRING8);
        //hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_PLANE_VALUES, "TRANSPONDER CODE:0", "BCO16", SIMCONNECT_DATATYPE_INT32);
        hr = SimConnect_AddToDataDefinition(hSimConnect, DATA_DEFINE_PLANE_VALUES, "Plane Altitude", "feet");
        hr = SimConnect_AddToDataDefinition(hSimConnect, DATA_DEFINE_PLANE_VALUES, "Plane Latitude", "degrees");
        hr = SimConnect_AddToDataDefinition(hSimConnect, DATA_DEFINE_PLANE_VALUES, "Plane Longitude", "degrees");

        // Request an event when the simulation starts
        hr = SimConnect_SubscribeToSystemEvent(hSimConnect, SIM_UPDATE_EVENT, "1Sec");

        printf("\nLaunch a flight.");

        while (0 == quit) {
            SimConnect_CallDispatch(hSimConnect, DispatchProcRD, NULL);
            Sleep(1);
        }

        hr = SimConnect_Close(hSimConnect);

        return 0;
    } else {
        printf("Failed to connect, please launch FS2020 first\n");
        return 1;
    }
}