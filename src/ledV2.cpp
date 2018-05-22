#include <Arduino.h>
#include <IRremote.h>


// E0E0F00F Белый

class RGBLed
{
  public:
    // Пины
    int R;
    int G;
    int B;
    int W;

    bool _state;                  // Состояние вкыл-выкл
    int _color[3];                // Текущий цвет
    String _colorHEX = "#9c27b0"; // Текущий цвет в HEX
    int _bright;                  // Яркость
    int _brightMIN = 1;           // Яркость
    int _brightMAX = 5;           // Яркость

    // Анимация
    bool _animStat = 1;  //Включена ли анимация
    int _animFrame = 14; // Количество "кадров". Влияет на плавность
    int _animTime = 600; // Время анимации

    bool _whiteState = 0; // Состояние белого цвета

    // Инициализация
    void init(int pinR, int pinG, int pinB, int pinW)
    {
        R = pinR;
        G = pinG;
        B = pinB;
        W = pinW;
        _state = 1;
        _bright = 5;
    };

    // Включение-выключение
    void swich()
    {
        if (!_state)
        {
            on();
        }
        else
        {
            off();
        }
    }

    void on()
    {
        _state = 1;
        setColor(_colorHEX);
    }

    void off()
    {
        _state = 0;
        setColor("#000000");
    }

    // Установка яркости
    void setBright(int bright)
    {
        _bright = bright;

        setColor(_colorHEX);
    }

    void upBright()
    {

        if (_bright + 1 >= _brightMAX)
        {
            _bright = _brightMAX;
            setBright(_brightMAX);
        }
        else
        {
            _bright = _bright + 1;
            setBright(_bright);
        }
    }

    void downBright()
    {
        if (_bright - 1 <= 1)
        {
            _bright = 1;
            setBright(1);
        }
        else
        {
            _bright = _bright - 1;
            setBright(_bright);
        }
    }

    // Преобразование цвета из HEX в RGB
    void HEXtoRGB(String colorHex)
    {
        if (colorHex != "#000000")
        {
            _colorHEX = colorHex;
        }
        long number = (long)strtol(&colorHex[1], NULL, 16);
        _color[0] = number >> 16;
        _color[1] = number >> 8 & 0xFF;
        _color[2] = number & 0xFF;

        // Установка цвета с учетом яркости;
        _color[0] = (_color[0] / _brightMAX) * _bright;
        _color[1] = (_color[1] / _brightMAX) * _bright;
        _color[2] = (_color[2] / _brightMAX) * _bright;
    }

    // Установка заданого цвета
    void setColor(String color)
    {

        if (_animStat)
        {
            int _rFROM = _color[0];
            int _gFROM = _color[1];
            int _bFROM = _color[2];

            HEXtoRGB(color);

            int _rTO = _color[0];
            int _gTO = _color[1];
            int _bTO = _color[2];

            int _rDELTA = _rFROM - _rTO;
            int _gDELTA = _gFROM - _gTO;
            int _bDELTA = _bFROM - _bTO;

            int _rCHANGE = round(_rDELTA / _animFrame);
            int _gCHANGE = round(_gDELTA / _animFrame);
            int _bCHANGE = round(_bDELTA / _animFrame);

            for (int i = 1; i <= _animFrame; i++)
            {
                analogWrite(R, _rFROM - (_rCHANGE * i));
                analogWrite(G, _gFROM - (_gCHANGE * i));
                analogWrite(B, _bFROM - (_bCHANGE * i));
                delay(_animTime / _animFrame);
            }

            analogWrite(R, _color[0]);
            analogWrite(G, _color[1]);
            analogWrite(B, _color[2]);
        }
        else
        {
            HEXtoRGB(color);
            analogWrite(R, _color[0]);
            analogWrite(G, _color[1]);
            analogWrite(B, _color[2]);
        }
    }

    void setWhite()
    {
        if (_whiteState)
        {
            analogWrite(W, 0);
            _whiteState = false;
        }
        else
        {
            analogWrite(W, 255);
            _whiteState = true;
        }
    }
};

class Colors
{

  public:
    int _index = 0;      // Текущий номер цвета
    int _colorsSize = 4; // Количество цветов
    boolean _loop = 1;

    // Список цветов
    String _colors[4] = {
        "#00ff0d",
        "#b71c1c",
        "#9c27b0",
        "#fbc02d",
    };

    String getColor()
    {
        return _colors[_index];
    }

    String nextColor()
    {
        if (_index + 1 >= _colorsSize)
        {
            if (_loop)
            {
                _index = 0;
            }
            else
            {
                _index = _colorsSize;
            }
            return getColor();
        }
        else
        {
            _index = _index + 1;
            return getColor();
        }
    }

    String prevColor()
    {
        if (_index <= 0)
        {
            if (_loop)
            {
                _index = _colorsSize - 1;
            }
            else
            {
                _index = 0;
            }
            return getColor();
        }
        else
        {
            _index = _index - 1;
            return getColor();
        }
    }
};



/** НАСТРОИКИ ПУЛЬТА**/
#define UP_btn 123;     // Вверх
#define DOWN_btn 123;   // Вниз
#define LEFT_btn 123;   // Влево
#define RIGHT_btn 123;  // Впраов
#define COLOR_SWICH_btn 123; // Вкыл-выкл ленты
#define WHITE_SWICH_btn 123; // Вкыл-выкл белого
/*******************/

/** НАСТРОЙКИ ЛЕНТЫ **/
int R = 9;
int G = 5;
int B = 6;
int W = 4;
RGBLed rgb;
/*****************/

Colors ledcolors;


/** ПУЛЬТ */
IRrecv irrecv(13);
decode_results results;
/*****************/


void setup()
{

    Serial.begin(9600);
    rgb.init(R, G, B, W);

    // rgb.setWhite();
    // delay(2000);
    // rgb.setWhite();
    //  delay(2000);
    // rgb.setWhite();
    // rgb.setColor(ledcolors.getColor());
    // delay(2000);
    // rgb.setColor(ledcolors.nextColor());
    // delay(2000);
    // rgb.setColor(ledcolors.nextColor());
    // delay(2000);
    // rgb.setColor(ledcolors.nextColor());
    // delay(1000);
    // rgb.setBright(3);
    // rgb.off();
    // delay(1000);
    // rgb.on();
}

void loop()
{

}
