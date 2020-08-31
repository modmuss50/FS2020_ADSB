import plane
import joiner
import strformat
import strutils

proc encodeSBS*(plane: Plane): string =
    let sj = StringJoiner(delimiter: ",")
    sj.append("MSG")
    sj.append("3") # Message type
    sj.append("1")
    sj.append("1")

    if plane.icao == "":
        sj.append("43c813") # Hex
    else:
        sj.append(plane.icao) # Hex

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

    sj.append(plane.squawk)

    # sj.append("7000") # Squawk
    sj.append("1") # Squawk change flag, always set to 1
    sj.append("") # Squawk Emergency flag
    sj.append("") # Squawk Ident flag
    sj.append("") # OnTheGround flag

    return sj.value()