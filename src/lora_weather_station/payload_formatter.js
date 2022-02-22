const SPEED_PER_CLICK = 2.4 // km/h
const RAIN_PER_CLICK = 0.2794 // mm

const directions = [
  [ 112.5, "ESE" ],
  [  67.5, "ENE" ],
  [  90.0,   "E" ],
  [ 157.5, "SSE" ],
  [ 135.0,  "SE" ],
  [ 202.5, "SSW" ],
  [ 180.0,   "S" ],
  [  22.5, "NNE" ],
  [  45.0,  "NE" ],
  [ 247.5, "WSW" ],
  [ 225.0,  "SW" ],
  [ 337.5, "NNW" ],
  [   0.0,   "N" ],
  [ 292.5, "WNW" ],
  [ 315.0,  "NW" ],
  [ 270.0,   "W" ],
]

function decodeUplink(input) {
  let data = []
  let wind_direction_index, rain_amount, wind_speed, max_wind_speed
  
  for (let i = 0; i < input.bytes.length; i += 3) {
    /* read payload */

    // payload representation
    /* [      0]  [      1]  [      2] */ // byte count
    /* 0000 0000  0000 0000  0000 0000 */ // bit count
    /* [WV] [..R..][..AN..]  0[.M_AN.] */ // value allocation
    // WV = windVaneDirectionIndex, R = rainAmount, AN = avgWindSpeed, M_AN = maxWindSpeed

    // byte 0
    wind_direction_index = input.bytes[i] >> 4
    rain_amount = (input.bytes[i] << 1) & 0x1E
    
    // byte 1
    rain_amount |= input.bytes[i + 1] >> 7
    wind_speed = input.bytes[i + 1] & 0x7F
    
    // byte 2
    max_wind_speed = input.bytes[i + 2] & 0x7F
    
    
    /* format data */
    wind_speed *= SPEED_PER_CLICK
    max_wind_speed *= SPEED_PER_CLICK
    rain_amount *= RAIN_PER_CLICK
    
    
    data.push({
      wind_direction: directions[wind_direction_index][1],
      wind_speed: wind_speed.toFixed(2)+ " km/h",
      max_wind_speed: max_wind_speed.toFixed(2) + " km/h",
      rain_amount: rain_amount + " mm"
    })
  }
  
  return {
    data,
    warnings: [],
    errors: []
  };
}