#include "watchy_solar_noon.h"

void WatchySolarNoon::drawWatchFace() {
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);

  float t = elapsedSecsToday(makeTime(currentTime) - settings.gmtOffset) / (60 * 60 * 24.0);
  int16_t bitmap_offset = static_cast<int16_t>( 320 * t + -220 );
  
  


  display.fillRect(0,23,200,160,GxEPD_BLACK);
//  display.drawXBitmap(bitmap_offset - RenderData_160_width, 23, RenderData_160_bits, RenderData_160_width, RenderData_160_height, GxEPD_WHITE);
  display.drawXBitmap(bitmap_offset, 23, RenderData_160_bits, RenderData_160_width, RenderData_160_height, GxEPD_WHITE);
  display.drawXBitmap(bitmap_offset + RenderData_160_width, 23, RenderData_160_bits, RenderData_160_width, RenderData_160_height, GxEPD_WHITE);


  float pp;
  int16_t kk;
  pp = percentOfYear(currentTime);
  // min 54 matches artic circle, max 354 for almost lines, ^6 is a guess that seems to fit end of night at end of february
  kk = floor(abs(pow(sin(pp * 2 * PI), 6.0)) * 300 + 54);

  // whiteout every other pixel
  fiftyRect(0, 23, 200, 160, 0, GxEPD_WHITE);

  if( pp > 0.25 && pp <= 0.75 ) {
    // south dark
    fiftyRect(0, 23 + 80, 200, 80, 0, GxEPD_BLACK);
    }
  else {
    // north dark
    fiftyRect(0, 23, 200, 80, 0, GxEPD_BLACK);
    }

  // left and right of night
  fillEllipse(-60, 23 + 80, 80, kk, 0, GxEPD_BLACK);
  fillEllipse(260, 23 + 80, 80, kk, 0, GxEPD_BLACK);

  // center of day
  fillEllipse(100, 100, 80, kk, 0, GxEPD_WHITE);

/*
  
  // left and right of lit hemisphere
  fillEllipse(-60, 23 + 80, 80, k, 0, GxEPD_BLACK);
  fillEllipse(260, 23 + 80, 80, k, 0, GxEPD_BLACK);

  // center of dark hmisphere
  fillEllipse(100, 100, 80, k, 0, GxEPD_WHITE);

*/


  //display.drawLine(100, 23, 100, 183, GxEPD_BLACK);
  for (int16_t j = 23; j < 183; j++) {
    if( j % 2 != 0) {
      display.drawPixel(100, j,  GxEPD_BLACK);
    }
  }



  display.fillRect(0,183,200,17,GxEPD_WHITE);



  display.setFont(&FreeSansBold12pt7b);

  display.setCursor(1, 18);
  display.print(dayShortStr(currentTime.Wday));
  display.print(' ');
  display.print(currentTime.Day);
  display.print(' ');
  display.print(monthShortStr(currentTime.Month));
  display.print(' ');
  display.print(tmYearToCalendar(currentTime.Year));

  display.setCursor(40, 199);

  int16_t  x_center, y_center;
  uint16_t w_center, h_center;


  char long_buffer[64];

  sprintf(long_buffer, "%1.4f %d", //"lon. %1.2f %1.4f %d",
    //(t * (-360.0)) + 180,
    pp,
    kk
    );

  display.getTextBounds(long_buffer, 0, 173, &x_center, &y_center, &w_center, &h_center);

  display.setCursor(100 - w_center / 2, 173);

 // display.println(long_buffer);



  // "Stack smashing protect failure!" ?"
  char time_buffer[64];

  sprintf(time_buffer, "%02d:%02d%+05d",
    currentTime.Hour,
    currentTime.Minute,
    settings.gmtOffset / 36
    );

  display.getTextBounds(time_buffer, 0, 199, &x_center, &y_center, &w_center, &h_center);

  display.setCursor(100 - w_center / 2, 199);

  display.println(time_buffer);


}


void WatchySolarNoon::fiftyRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t alt, uint16_t color) {
  display.startWrite();
  for (int16_t i = x; i < x + w; i++) {
    for (int16_t j = y; j < y + h; j++) {
      if(i % 2 != alt ^ j % 2 != alt) {
        display.drawPixel(i, j,  color);
      }
    }
  }
  display.endWrite();
}


// optimized versions gave me a rough diamond for no obvious reason
void WatchySolarNoon::fillEllipse(int16_t x0, int16_t y0, int16_t rx, int16_t ry, int16_t alt,  uint16_t color) {
  display.startWrite();
  int i;
  int j;
  for (int y = -ry; y <= ry; y++) {
    for (int x = -rx; x <= rx; x++) {
      if ((x * x * ry * ry) + (y * y * rx * rx) <= (rx * rx * ry * ry)) {
        i = x0 + x;
        j = y0 + y;
        if(i % 2 != alt ^ j % 2 != alt) {
          display.drawPixel(i, j, color);
        }
      }
    }
  }
  display.endWrite();
}
/*
int16_t WatchySolarNoon::julianDate(tmElements_t currentTime) {
  tmElements_t startOfYear;
  startOfYear.Year = currentTime.Year;
  startOfYear.Month = 1;
  startOfYear.Day = 1;
  startOfYear.Hour = 0;
  startOfYear.Minute = 0;
  startOfYear.Second = 0;
  
  int16_t jd = floor((makeTime(currentTime) - makeTime(startOfYear)) / 86400) + 1;

  return jd;
}
*/
float WatchySolarNoon::percentOfYear(tmElements_t currentTime) {
  return ((makeTime(currentTime) - settings.gmtOffset + 864000) % 31556952) / 31556952.0;
}

