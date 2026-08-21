#include "LGFX.h"
#include "config.h"
#include "ha_client.h"
#include "ui.h"

#include <Arduino.h>
#include <WiFi.h>
#include <lvgl.h>

LGFX lcd;

#define TFT_BL 2
SPIClass& spi = SPI;
#include "touch.h"

static uint32_t screenWidth;
static uint32_t screenHeight;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t disp_draw_buf[800 * 480 / 15];
static lv_disp_drv_t disp_drv;

void my_disp_flush(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  lcd.pushImageDMA(area->x1, area->y1, w, h, (lgfx::rgb565_t*)&color_p->full);

  lv_disp_flush_ready(disp);
}

void my_touchpad_read(lv_indev_drv_t* indev_driver, lv_indev_data_t* data)
{
  if (touch_has_signal())
  {
    if (touch_touched())
    {
      data->state = LV_INDEV_STATE_PR;
      data->point.x = touch_last_x;
      data->point.y = touch_last_y;
    }
    else if (touch_released())
    {
      data->state = LV_INDEV_STATE_REL;
    }
  }
  else
  {
    data->state = LV_INDEV_STATE_REL;
  }
  delay(15);
}

static void wifi_connect()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(config::wifi_ssid, config::wifi_password);

  Serial.print("Connecting to WiFi");
  unsigned long wifi_start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - wifi_start < 15000)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.status() == WL_CONNECTED ? " connected" : " timeout, will keep retrying");
}

static void poll_ha_sensor()
{
  if (WiFi.status() != WL_CONNECTED)
    return;

  Ha_state sensor = ha_get_state(config::ha_sensor_entity);
  if (!sensor.ok)
    return;

  String text = sensor.state;
  if (sensor.unit.length() > 0)
    text += " " + sensor.unit;
  lv_label_set_text(ui_Label2, text.c_str());
}

void setup()
{
  Serial.begin(115200);

  lcd.begin();
  lv_init();
  touch_init();

  screenWidth = lcd.width();
  screenHeight = lcd.height();

  lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, screenWidth * screenHeight / 15);

  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  ledcSetup(1, 300, 8);
  ledcAttachPin(TFT_BL, 1);
  ledcWrite(1, 255);

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, LOW);
  delay(500);
  digitalWrite(TFT_BL, HIGH);

  ui_init();

  lv_timer_handler();

  wifi_connect();
  poll_ha_sensor();
}

void loop()
{
  lv_timer_handler();
  delay(5);

  static unsigned long last_ha_poll = 0;
  unsigned long now = millis();
  if (now - last_ha_poll >= config::ha_sensor_poll_ms)
  {
    last_ha_poll = now;
    poll_ha_sensor();
  }
}
