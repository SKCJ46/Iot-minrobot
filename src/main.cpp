#include <WiFi.h>
#include <ArtronShop_LineMessaging.h>

// WiFi & LINE Configuration
const char* ssid = "CJnaJA";
const char* password = "11223344";
#define LINE_TOKEN "h0VofOYAQtkM28a3/Sacelvef/w4xh+pERmcSZBq6mQ/d2sLP97y65VzsqDMDI6gquqrLYL5KeysL3yyJQK3FMx45yQ9Gi/6qIWWp5PAMUJIjJZi4gUXTGHGMT5r6IIR930Qcpo9A1Qpye2a8NWqqgdB04t89/1O/w1cDnyilFU="
#define LINE_USER_ID "Ca4a063ce3a975b9c49b318956826c16e"

// Task Handles
TaskHandle_t TaskA1Handle = NULL;
TaskHandle_t TaskA2Handle = NULL;

// Global Counter
volatile int a2Count = 0;

// ส่งข้อความผ่าน LINE พร้อมแสดงสถานะ
void sendLineMessage(const String& message) {
  if (::WiFi.status() != WL_CONNECTED) {
    Serial.println("[ERROR] WiFi disconnected. Trying to reconnect...");
    ::WiFi.disconnect();
    ::WiFi.begin(ssid, password);

    unsigned long startAttemptTime = millis();
    while (::WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
      Serial.print(".");
      delay(500);
    }
    Serial.println();
  }

  if (::WiFi.status() == WL_CONNECTED) {
    Serial.println("[INFO] Sending LINE message...");
    if (LINE.send(LINE_USER_ID, message)) {
      Serial.println("[SUCCESS] LINE message sent.");
    } else {
      Serial.printf("[ERROR] LINE send failed. Status Code: %d\n", LINE.status_code);
    }
  } else {
    Serial.println("[FAIL] Cannot reconnect WiFi. LINE message not sent.");
  }
}


// Task A1: ส่งข้อความ A1 ทุก 2 วินาที
void TaskA1(void *pvParameters) {
  while (1) {
    Serial.println("[TaskA1] A1");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

// Task A2: ส่งข้อความ A2 ทุก 5 วินาที และส่ง LINE เมื่อครบ 5 ครั้ง
void TaskA2(void *pvParameters) {
  while (1) {
    Serial.println("[TaskA2] A2");
    a2Count++;
    Serial.printf("[TaskA2] a2Count = %d\n", a2Count);

    if (a2Count >= 5) {
      sendLineMessage("ส่ง A2 ครบ 5 ครั้งแล้ว!");
      a2Count = 0;
    }
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n[SETUP] เริ่มต้นระบบ");

  // เชื่อมต่อ WiFi
  WiFi.begin(ssid, password);
  Serial.print("[WiFi] Connecting to WiFi");
  int retryCount = 0;
  while (WiFi.status() != WL_CONNECTED && retryCount < 20) {
    delay(500);
    Serial.print(".");
    retryCount++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[WiFi] Connected.");
    Serial.print("[WiFi] IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n[ERROR] Failed to connect WiFi. Please check SSID/PASSWORD.");
  }

  // เริ่มต้น LINE
  LINE.begin(LINE_TOKEN);
  Serial.println("[LINE] Ready");

  // สร้าง Task
  xTaskCreatePinnedToCore(TaskA1, "TaskA1", 2048, NULL, 1, &TaskA1Handle, 1);
  xTaskCreatePinnedToCore(TaskA2, "TaskA2", 12288, NULL, 1, &TaskA2Handle, 1);
}

void loop() {
  // ปล่อยว่างไว้
}
