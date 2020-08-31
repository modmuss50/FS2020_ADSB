type
    Plane* = object
        icao*: string
        squawk*: string
        altitude*: int
        latitude*: float64
        longitude*: float64
        verticalSpeed*: float64
        heading*: float64
        groundSpeed*: float64