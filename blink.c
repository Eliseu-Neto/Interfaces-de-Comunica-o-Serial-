// Bibliotecas necessárias
#include <Arduino.h>
#include <Wire.h>              // Comunicação I2C
#include <Adafruit_GFX.h>      // Biblioteca gráfica
#include <Adafruit_SSD1306.h>  // Controle do display OLED
#include <Adafruit_NeoPixel.h> // Controle dos LEDs WS2812

// Definição de pinos
#define LED_R 11
#define LED_G 12
#define LED_B 13
#define BUTTON_A 5
#define BUTTON_B 6
#define LED_MATRIX_PIN 7
#define DISPLAY_SDA 14
#define DISPLAY_SCL 15

// Definição do display SSD1306
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Definição da matriz de LEDs WS2812
#define NUM_LEDS 25
Adafruit_NeoPixel matrix = Adafruit_NeoPixel(NUM_LEDS, LED_MATRIX_PIN, NEO_GRB + NEO_KHZ800);

// Estados dos LEDs
bool ledGState = false;
bool ledBState = false;

// Protótipos das funções
void setupDisplay();
void setupLEDs();
void setupButtons();
void IRAM_ATTR handleButtonA();
void IRAM_ATTR handleButtonB();
void debounceButton(int pin, void (*callback)());
void displayChar(char c);
void displaySymbol(int num);

void setup() {
    Serial.begin(115200);
    Wire.begin(DISPLAY_SDA, DISPLAY_SCL);
    
    setupDisplay();
    setupLEDs();
    setupButtons();
}

void loop() {
    if (Serial.available() > 0) {
        char inputChar = Serial.read();
        
        if (isdigit(inputChar)) {
            displaySymbol(inputChar - '0');
        } else {
            displayChar(inputChar);
        }
    }
}

// Configuração do display
void setupDisplay() {
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("Falha ao iniciar o display SSD1306");
        while (1);
    }
    display.clearDisplay();
    display.display();
}

// Configuração dos LEDs
void setupLEDs() {
    pinMode(LED_R, OUTPUT);
    pinMode(LED_G, OUTPUT);
    pinMode(LED_B, OUTPUT);
    
    matrix.begin();
    matrix.show();
}

// Configuração dos botões com interrupções
void setupButtons() {
    pinMode(BUTTON_A, INPUT_PULLUP);
    pinMode(BUTTON_B, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_A), handleButtonA, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_B), handleButtonB, FALLING);
}

// Tratamento do botão A
void IRAM_ATTR handleButtonA() {
    debounceButton(BUTTON_A, []() {
        ledGState = !ledGState;
        digitalWrite(LED_G, ledGState);
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("LED Verde: ");
        display.println(ledGState ? "Ligado" : "Desligado");
        display.display();
        Serial.println("Botão A pressionado - LED Verde alternado");
    });
}

// Tratamento do botão B
void IRAM_ATTR handleButtonB() {
    debounceButton(BUTTON_B, []() {
        ledBState = !ledBState;
        digitalWrite(LED_B, ledBState);
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("LED Azul: ");
        display.println(ledBState ? "Ligado" : "Desligado");
        display.display();
        Serial.println("Botão B pressionado - LED Azul alternado");
    });
}

// Implementação do debounce
void debounceButton(int pin, void (*callback)()) {
    static unsigned long lastPress = 0;
    unsigned long currentTime = millis();
    if (currentTime - lastPress > 200) {
        callback();
        lastPress = currentTime;
    }
}

// Exibição de caracteres no display
void displayChar(char c) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Char: ");
    display.print(c);
    display.display();
}

// Exibição de símbolos na matriz WS2812
void displaySymbol(int num) {
    matrix.clear();
    for (int i = 0; i < num; i++) {
        matrix.setPixelColor(i, matrix.Color(0, 255, 0));
    }
    matrix.show();
}
