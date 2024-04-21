#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <math.h>

#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   128

#define SCLOCK_PIN      13
#define MOST_PIN        11
#define RESET_PIN       3
#define DC_PIN          7
#define CS_PIN          10

#define HEX_COLOR_BLACK              0x0000
#define HEX_COLOR_WHITE              0xFFFF
#define HEX_COLOR_RED                0xF800
#define HEX_COLOR_GREEN              0x7E00
#define HEX_COLOR_BLUE               0x001F
#define HEX_COLOR_CYAN               0x07FF
#define HEX_COLOR_MAGENTA            0xF81F
#define HEX_COLOR_YELLOW             0xFFE0
#define HEX_COLOR_GREY               0x8410

Adafruit_SSD1351 displ = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RESET_PIN);

float angle = 0.f;

uint16_t rgbToHex(int r, int g, int b) {

    byte R = (byte)r;
    byte G = (byte)g;
    byte B = (byte)b;

    uint16_t hexadecimal = ((R & 0xF8) << 8) | ((G & 0xFC) << 3) | ((B & 0xF8) >> 3);
    return hexadecimal;
}

class point {
public:
    float x;
    float y;

    point(float x, float y) {
        this->x = x;
        this->y = y;
    }
};

class ray {
public:
    float x;
    float y;
    float z;

    ray(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    ray operator+(ray b) {
        return ray(x + b.x, y + b.y, z + b.z);
    }
    ray operator*(float b) {
        return ray(x * b, y * b, z * b);
    }

    void normalize() {
        float magnitude = sqrt(x * x + y * y + z * z);
        x /= magnitude;
        y /= magnitude;
        z /= magnitude;
    }
};

struct point screenCoordToUV(float x, float y) {

    return point((x/(float)SCREEN_WIDTH) * 2.f - 1.f, (y/(float)SCREEN_HEIGHT) * 2.f - 1.f);
}

float dot(struct ray a, struct ray b) {
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

float radius = 1.f;

void setup() {
      // put your setup code here, to run once:
      Serial.begin(9600);
      displ.begin();
      displ.fillScreen(HEX_COLOR_BLACK);
} 

void loop() {
      // put your main code here, to run repeatedly:
      angle += 0.01f;

      ray origin = ray(0.f, 0.f, 2.f);

      for (int x = 0; x < SCREEN_WIDTH; x++) { for (int y = 0; y < SCREEN_HEIGHT; y++) {

          point uvPoint = screenCoordToUV(x, y);
          ray rayDirection = ray(uvPoint.x, uvPoint.y, -1.f);

          float a = dot(rayDirection, rayDirection);
          float b = 2.f * dot(origin, rayDirection);
          float c = dot(origin, origin) - pow(radius, 2);

          float delta = pow(b, 2.f) - (4.f * a * c);
          if (delta < 0) {
              displ.drawPixel(y, x, rgbToHex(0, 0, 0));
              continue;
          }
          
          float t1 = (-b - sqrt(delta))/(2.f * a);
          float t2 = (-b + sqrt(delta))/(2.f * a);

          ray hitClosest = origin + rayDirection * t1;
          hitClosest.normalize();

          float R = hitClosest.x;
          float G = hitClosest.y;
          float B = hitClosest.z;

          displ.drawPixel(y, x, rgbToHex(R * 255, G * 255, B * 255));
      }}

      //int hex = rgbToHex(255, 255, 0);
      //displ.fillScreen(hex);
}
