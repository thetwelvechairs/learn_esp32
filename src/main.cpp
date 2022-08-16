#include <fabgl.h>
#include <Adafruit_NeoPixel.h>
#include <iostream>

#define PIN         0
#define NUMPIXELS   1

#define TFT_SCK     5
#define TFT_MOSI    19

#define TFT_CS      15
#define TFT_DC      33
#define TFT_RESET   32

#define TFT_SPIBUS  VSPI_HOST


fabgl::ST7789Controller DisplayController;
Canvas canvas(&DisplayController);

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DOUBLEBUFFERING 1


struct Test {
    virtual ~Test() { };
    virtual void update() = 0;
    virtual bool nextState() = 0;
    virtual int testState() = 0;
    virtual char const * name() = 0;
};

#include "ballstest.h"
#include "polygonstest.h"
#include "spritestest.h"


int color = 1;


void setup()
{
    Serial.begin(115200);

    // TFT_SCK, TFT_MOSI, TFT_DC, TFT_RESET, TFT_CS, TFT_SPIBUS
    DisplayController.begin(TFT_SCK, TFT_MOSI, TFT_DC, TFT_RESET, TFT_CS, TFT_SPIBUS);
    DisplayController.setResolution(TFT_170x320, -1, -1, DOUBLEBUFFERING);
    DisplayController.setOrientation(fabgl::TFTOrientation::Rotate90);

    pixels.setPixelColor(0, Adafruit_NeoPixel::Color(0, 1, 0));
    pixels.show();

    // ============================================================================================

    // get a font for about 40x14 text screen
    canvas.selectFont(&fabgl::FONT_9x18);

    canvas.setGlyphOptions(GlyphOptions().FillBackground(true));
}

void loop()
{
    static int64_t stime  = esp_timer_get_time();
    static int FPS        = 0;
    static int FPSCounter = 0;
    static int testIndex  = 0;
    static Test * test    = new BallsTest;

    if (test->nextState() == false) {
        delete test;
        ++testIndex;
        switch (testIndex) {
            case 1:
                test = new PolygonsTest;
                break;
            case 2:
                test = new SpritesTest;
                break;
            default:
                testIndex = 0;
                test = new BallsTest;
                break;
        }
    }

    if (esp_timer_get_time() - stime > 1000000) {
        // calculate FPS
        FPS = FPSCounter;
        stime = esp_timer_get_time();
        FPSCounter = 0;
    }
    ++FPSCounter;

    test->update();

    // display test state and FPS
    canvas.setPenColor(Color::Blue);
    canvas.setBrushColor(Color::BrightYellow);
    canvas.drawTextFmt(0, 0, " %d %s at %d FPS ", test->testState(), test->name(), FPS);

    if (DOUBLEBUFFERING)
        canvas.swapBuffers();

}