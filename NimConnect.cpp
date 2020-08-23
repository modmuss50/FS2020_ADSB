#include <sstream>
#include <strsafe.h>
#include <iostream>
#include <windows.h>
#include "SimConnect.h"

HANDLE hSimConnect = NULL;

int quit = 0;

const int SIM_UPDATE_EVENT = 0;
const int DATA_DEFINE_PLANE_VALUES = 0;
const int DATA_REQUEST_SIMOBJECT_TYPE_USER_REQUEST = 0;

struct PlaneData {
    char title[256];
    char tailnumber[64];
    char flightnum[8];
    double altitude;
    double latitude;
    double longitude;
    double verticalSpeed;
    double heading;
    double groundSpeed;
};

PlaneData* lastPlane;


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
                      lastPlane = pS;
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

extern "C" int connectToSim() {
    return SUCCEEDED(SimConnect_Open(&hSimConnect, "Request Data", NULL, 0, 0, 0));
}

extern "C" void subscribeToEvents() {
    HRESULT hr;
    // Set up the data definition, but do not yet do anything with it
    hr = SimConnect_AddToDataDefinition(hSimConnect, DATA_DEFINE_PLANE_VALUES, "Title", NULL, SIMCONNECT_DATATYPE_STRING256);
    hr = SimConnect_AddToDataDefinition(hSimConnect, DATA_DEFINE_PLANE_VALUES, "ATC id", NULL, SIMCONNECT_DATATYPE_STRING64);
    hr = SimConnect_AddToDataDefinition(hSimConnect, DATA_DEFINE_PLANE_VALUES, "ATC Flight Number", NULL, SIMCONNECT_DATATYPE_STRING8);
    //hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_PLANE_VALUES, "TRANSPONDER CODE:0", "BCO16", SIMCONNECT_DATATYPE_INT32);
    hr = SimConnect_AddToDataDefinition(hSimConnect, DATA_DEFINE_PLANE_VALUES, "Plane Altitude", "feet");
    hr = SimConnect_AddToDataDefinition(hSimConnect, DATA_DEFINE_PLANE_VALUES, "Plane Latitude", "degrees");
    hr = SimConnect_AddToDataDefinition(hSimConnect, DATA_DEFINE_PLANE_VALUES, "Plane Longitude", "degrees");
    hr = SimConnect_AddToDataDefinition(hSimConnect, DATA_DEFINE_PLANE_VALUES, "VERTICAL SPEED", "feet");
    hr = SimConnect_AddToDataDefinition(hSimConnect, DATA_DEFINE_PLANE_VALUES, "PLANE HEADING DEGREES TRUE", "degrees");
    hr = SimConnect_AddToDataDefinition(hSimConnect, DATA_DEFINE_PLANE_VALUES, "GPS GROUND SPEED", "knots");

    // Request an event when the simulation starts
    hr = SimConnect_SubscribeToSystemEvent(hSimConnect, SIM_UPDATE_EVENT, "1Sec");
}

extern "C" void callDispatch() {
  SimConnect_CallDispatch(hSimConnect, DispatchProcRD, NULL);
}

extern "C" int shouldQuit() {
  return quit;
}

extern "C" int getPlaneAltitude() {
  if (lastPlane == NULL) {
    return 0;
  }
  return (int) lastPlane->altitude;
}

extern "C" double getPlaneLatitude() {
  if (lastPlane == NULL) {
    return 0;
  }
  return lastPlane->latitude;
}

extern "C" double getPlaneLongitude() {
  if (lastPlane == NULL) {
    return 0;
  }
  return lastPlane->longitude;
}

extern "C" double getPlaneVerticalSpeed() {
  if (lastPlane == NULL) {
    return 0;
  }
  return lastPlane->verticalSpeed;
}

extern "C" double getPlaneHeading() {
  if (lastPlane == NULL) {
    return 0;
  }
  return lastPlane->heading;
}

extern "C" double getPlaneGroundSpeed() {
  if (lastPlane == NULL) {
    return 0;
  }
  return lastPlane->groundSpeed;
}