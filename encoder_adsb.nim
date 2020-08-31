import plane
import strutils

let lookupTable = "#ABCDEFGHIJKLMNOPQRSTUVWXYZ#####_###############0123456789######"
let crcGen = "1111111111111010000001001"

#  4- Aircraft Identification
proc encodeIdent(plane: Plane): string =
   var buffer = ""

   buffer &= 4.toBin(5) # Message type code
   buffer &= 0.toBin(3) # Emitter category

   let callsign = "KLM1023_"
   for n in 0 .. 7:
      var index = lookupTable.find(callsign[n].toUpperAscii())
      buffer &= index.toBin(6)

   return buffer

# Many thanks to https://github.com/lyusupov/ADSB-Out/blob/master/ADSB_Encoder.py
proc encodeChecksum(message: string): string =
   var binary = message

   for i in 0 ..< binary.len - 24:
      if binary[i] == '1':
         for j in 0 ..< crcGen.len:
            let a = parseInt($binary[i+j])
            let b = parseInt($crcGen[j])
            binary[i+j] = (a xor b).intToStr[0]

   return fromBin[int64](binary[^24 .. ^1]).toHex(6)

# https://mode-s.org/decode/adsb/introduction.html
proc encodeADSB*(plane: Plane): string =
   var buffer = ""

   buffer &= 17.toBin(5) # Downlink format
   buffer &= 5.toBin(3) # Capability

   var icao = plane.icao;
   if icao == "" :
      icao= "4840D6"

   buffer &= fromHex[int64](icao.toUpperAscii()).toBin(24) # ICAO aircraft address

   # Reset the buffer and preare to build the data section of the message
   var headerBin = buffer
   var headerHex = fromBin[int64](headerBin).toHex(8)
   var dataBin = encodeIdent(plane)
   var dataHex = fromBin[int64](dataBin).toHex(14)

   return headerHex & dataHex & encodeChecksum(headerBin & dataBin & 0.toBin(24))