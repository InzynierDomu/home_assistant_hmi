/**
 * @file ha_client.cpp
 * @brief Implementation of the Home Assistant REST client.
 */
#include "ha_client.h"

#include "config.h"

#include <ArduinoJson.h>
#include <HTTPClient.h>

static String ha_url(const String& path)
{
  return "http://" + String(config::ha_host) + ":" + String(config::ha_port) + path;
}

Ha_state ha_get_state(const char* entity_id)
{
  Ha_state result;

  HTTPClient http;
  http.setConnectTimeout(5000);
  http.setTimeout(5000);
  if (!http.begin(ha_url("/api/states/" + String(entity_id))))
    return result;

  /* Bez tego HTTPClient trzyma keep-alive; przy odstepach ~30s miedzy
   * odpytaniami gniazdo bywa juz zamkniete przez serwer/NAT i nastepny
   * begin() probuje je doczyscic - stad "flush(): fail on fd, errno: 11"
   * w logu (nieszkodliwe, ale halasliwe). Connection: close eliminuje
   * ten wyscig. */
  http.setReuse(false);

  http.addHeader("Authorization", String("Bearer ") + config::ha_token);

  int code = http.GET();
  if (code != HTTP_CODE_OK)
  {
    http.end();
    return result;
  }

  String payload = http.getString();
  http.end();

  JsonDocument doc;
  if (deserializeJson(doc, payload))
    return result;

  result.state = doc["state"] | "";
  result.unit = doc["attributes"]["unit_of_measurement"] | "";
  result.ok = result.state.length() > 0;
  return result;
}

bool ha_call_service(const char* domain, const char* service, const char* entity_id)
{
  HTTPClient http;
  http.setConnectTimeout(5000);
  http.setTimeout(5000);
  if (!http.begin(ha_url("/api/services/" + String(domain) + "/" + String(service))))
    return false;

  http.setReuse(false);

  http.addHeader("Authorization", String("Bearer ") + config::ha_token);
  http.addHeader("Content-Type", "application/json");

  String body = String("{\"entity_id\":\"") + entity_id + "\"}";
  int code = http.POST(body);
  http.end();

  return code == HTTP_CODE_OK;
}

void ha_toggle_light()
{
  ha_call_service("light", "toggle", config::ha_light_entity);
}
