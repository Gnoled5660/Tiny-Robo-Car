#include <ESP32PWM.h>
#include <esp_camera.h>

#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27

#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

#define Motor_Pin_1 14
#define Motor_Pin_2 13
#define Servo_Pin 2

ESP32PWM servo;

int Servo_Value = 350;

void setup() {
  Serial.begin(115200);
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;

  config.pixel_format = PIXFORMAT_RGB565;
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  pinMode(Motor_Pin_1, OUTPUT);
  pinMode(Motor_Pin_2, OUTPUT);

  servo.attach(Servo_Pin, 500, 2400);

  servo.write(90);

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK){
    return;
  }

}

void loop() {
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb){
    Serial.println("Couldn't get an image");
    return;
  }
  int redPixelCount = 0;
  int greenPixelCount = 0;
  int totalPixels = fb->width * fb->height;
  int width = fb->width;
  int height = fb->height;

  int countLeftRed = 0;
  int countCenterRed = 0;
  int countRightRed = 0;

  int countLeftGreen = 0;
  int countCenterGreen = 0;
  int countRightGreen = 0;

  int step =4;

  for (int y = 0; y < height; y += step){
    for (int x = 0; i < wdith; x += step){
      int index = (y * width + x) * 2;
      uint8_t highByte = fb->buf[index];
      uint8_t lowByte = fb->buf[index+1];

      uint8_t r = (highByte & 0xF8);
      uint8_t g = (highByte & 0x07 << 5) | ((lowByte & 0xE0) >> 3);
      uint8_t b = (lowByte & 0x1F) << 3;

      if (r < 150 && g < 80 && b < 80){
        greenPixelCount++;
        if (x < wdith / 3){
          countLeftRed++;
        }
        else if(x < (wdith * 2)/3){
          countCenterRed;
        }
        else{
          countRightRed++;
        }
      }
      if (r < 80 && g < 150 && b < 80){
        redPixelCount++;
      }
    }
  }

  int threshold = 10;
  if (greenPixelCount > redPixelCount){
    digitalWrite(Motor_Pin_1, HIGH);
    digitalWrite(Motor_Pin_2, LOW);
    if (countLeftGreen > threshold || countCenterGreen > threshold || countRightGreen > threshold){
      if(countLeftGreen > countCenterGreen && countLeftGreen > countRightGreen){
        servo.write(60);
      }
      else if (countCenterGreen > countLeftGreen && countCenterGreen > countRightGreen){
        servo.write(90)
      }
      else if (countRightGreen > countLeftGreen && countRightGreen > countCenterGreen)
      servo.write(120)
    }
  }
  else if(redPixelCount > greenPixelCount && redPixelCount > threshold){
    digitalWrite(Motor_Pin_1, LOW);
    digitalWrite(Motor_Pin_2, LOW;
    servo.write(90);
  }

  else{
    digitalWrite(Motor_Pin_1, LOW);
    digitalWrite(Motor_Pin_2, LOW);
    servo.write(90)
  }
  esp_camera_fb_return(fb);
  delay(50);
}



