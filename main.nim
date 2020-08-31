{.compile: "../simconnect/NimConnect.cpp".}
{.passL: "simconnect/SimConnect.lib".}

proc connectToSim(): cint {.importc.}
proc subscribeToEvents(): void {.importc.}
proc callDispatch(): void {.importc.}
proc shouldQuit(): cint {.importc.}

proc getPlaneAltitude(): cint {.importc.}
proc getPlaneLatitude(): cdouble {.importc.}
proc getPlaneLongitude(): cdouble {.importc.}
proc getPlaneVerticalSpeed(): cdouble {.importc.}
proc getPlaneHeading(): cdouble {.importc.}
proc getPlaneGroundSpeed(): cdouble {.importc.}

import os
import tcp
import encoder_sbs
import encoder_adsb
import parseopt
import plane

type
    RunOptions = object
        icao: string
        squawk: string # I do want to pull this from the aircraft but have failed so far

proc processUpdate(plane : Plane): void =
    tcp.send(encodeSBS(plane) & "\r\n")
    echo encodeADSB(plane)

proc getOptions(): RunOptions =
    var options = RunOptions()
    var parser = initOptParser(commandLineParams())

    while true:
        parser.next()
        case parser.kind
        of cmdEnd: break
        of cmdShortOption, cmdLongOption:
            if parser.val == "":
                echo "Unkown option: ", parser.key
                quit 1
            else:
                if parser.key == "icao" or parser.key == "i":
                    options.icao = parser.val
                elif parser.key == "squawk" or parser.key == "s":
                    options.squawk = parser.val
                else:
                    echo "Unkown option and value: ", parser.key, ", ", parser.val
                    quit 1
        of cmdArgument:
            echo "Argument: ", parser.key

    return options

when isMainModule:
    var options = getOptions()

    if connectToSim() == 1 :
        tcp.start()
        subscribeToEvents()
        echo "Connected to flight sim in nim"

        while shouldQuit() == 0 :
            callDispatch()
            let plane = Plane(icao: options.icao, squawk: options.squawk, altitude: getPlaneAltitude(), latitude: getPlaneLatitude(), longitude: getPlaneLongitude(), verticalSpeed: getPlaneVerticalSpeed(), heading: getPlaneHeading(), groundSpeed: getPlaneGroundSpeed())
            processUpdate(plane)
            sleep(1000)

        echo "Flight simulator quit"
    else:
      echo "Start flight simulator first"

