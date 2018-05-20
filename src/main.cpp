#include <Arduino.h>
#include <IRremote.h>

// лево E0E0A659
// право E0E046B9
// вверх E0E006F9
// вниз E0E08679

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

            
            for (int i = 1; i <= _animFrame; i++)
            {
                analogWrite(R, _rFROM - round(round(_rDELTA / _animFrame) * i));
                analogWrite(G, _gFROM - round(round(_gDELTA / _animFrame) * i));
                analogWrite(B, _bFROM - round(round(_bDELTA / _animFrame) * i));
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
            // digitalWrite(W, LOW);
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

int R = 3;
int G = 5;
int B = 6;
int W = 10;
RGBLed rgb;

Colors ledcolors;

IRrecv irrecv(2);
decode_results results;

void setup()
{

    Serial.begin(9600);
    irrecv.enableIRIn();

    rgb.init(R, G, B, W);

    // rgb.setColor(ledcolors.getColor());
    // delay(2000);
    // rgb.setColor(colors[1]);
    // delay(2000);
    // rgb.setColor(colors[2]);
    // delay(2000);
    // rgb.setColor(colors[3]);
    // rgb.setBright(3);
    // delay(1000);
    // rgb.off();
    // delay(1000);
    // rgb.on();
}

void loop()
{
    if (irrecv.decode(&results))
    { // если данные пришли

        switch (results.value)
        {
        case 0xE0E0A659: // кнопка влево
            rgb.setColor(ledcolors.prevColor());
            break;
        case 0xE0E046B9: // кнока вправо
            rgb.setColor(ledcolors.nextColor());
            break;

        case 0xE0E006F9: // кнока вверх
            rgb.upBright();
            break;

        case 0xE0E08679: // кнока вниз
            rgb.downBright();
            break;
        case 0xE0E016E9:
            rgb.swich();
            break;
        };


        irrecv.resume();                    // принимаем следующую команду
    }
}