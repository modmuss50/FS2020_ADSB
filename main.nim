{.compile: "NimConnect.cpp".}
{.passL: "SimConnect.lib".}

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
import joiner
import strutils
import strformat
import parseopt

type
    RunOptions = object
        icao: string
        squawk: string # I do want to pull this from the aircraft but have failed so far

type
    Plane = object
        altitude: int
        latitude: float64
        longitude: float64
        verticalSpeed: float64
        heading: float64
        groundSpeed: float64

proc processUpdate(plane : Plane, options: RunOptions): void =
    let sj = StringJoiner(delimiter: ",")
    sj.append("MSG")
    sj.append("3") # Message type
    sj.append("1")
    sj.append("1")

    if options.icao == "":
        sj.append("43c813") # Hex
    else:
        sj.append(options.icao) # Hex

    sj.append("1")
    sj.append("2019/12/10") # message reception time and date
    sj.append("19:10:46.320")
    sj.append("2019/12/10") # current time and date
    sj.append("19:10:46.320")
    sj.append("MSFS2020") # Callsign
    sj.append(intToStr(plane.altitude)) # altitude
    sj.append(&"{plane.groundSpeed}") # Ground speed
    sj.append(&"{plane.heading}") # Ground Heading
    sj.append(&"{plane.latitude}") # Lat
    sj.append(&"{plane.longitude}") # Lng
    sj.append(&"{plane.verticalSpeed}") # Vertical Rate

    # Squawk
    if options.squawk == "":
        sj.append("")
    else:
        sj.append(options.squawk)

    sj.append("7000") # Squawk
    sj.append("1") # Squawk change flag, always set to 1
    sj.append("") # Squawk Emergency flag
    sj.append("") # Squawk Ident flag
    sj.append("") # OnTheGround flag

    tcp.send(sj.value() & "\r\n")

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
            let plane = Plane(altitude: getPlaneAltitude(), latitude: getPlaneLatitude(), longitude: getPlaneLongitude(), verticalSpeed: getPlaneVerticalSpeed(), heading: getPlaneHeading(), groundSpeed: getPlaneGroundSpeed())
            processUpdate(plane, options)
            sleep(1000)

        echo "Flight simulator quit"
    else:
      echo "Start flight simulator first"

