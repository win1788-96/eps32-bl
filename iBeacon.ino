#include "HijelHID_BLEKeyboard.h"
#include "NimBLEDevice.h"
#include "esp_bt.h"
#include "esp_sleep.h"

// ======================== [ 1. 配置區 ] ========================
#define DEVICE_NAME   "FastTag"
#define MANUFACTURER  "ESP32_Auto"
#define SLEEP_TIMEOUT 30000UL 
#define LED_PIN       2          // NodeMCU-32S 內建 LED 通常在 GPIO 2

// 藍牙發射功率設定
// 藍牙發射功率設定變數
// 可選值: ESP_PWR_LVL_N12 (-12dBm), ESP_PWR_LVL_N9, ESP_PWR_LVL_N6, 
//         ESP_PWR_LVL_N3, ESP_PWR_LVL_N0 (預設), ESP_PWR_LVL_P3, 
//         ESP_PWR_LVL_P6, ESP_PWR_LVL_P9 (+9dBm 最強)
esp_power_level_t txPower = ESP_PWR_LVL_P9; 

// ======================== [ 2. 全域變數 ] ========================
HijelHID_BLEKeyboard bleKeyboard(DEVICE_NAME, MANUFACTURER, 100);

bool          wasConnected = false;
bool          triggered    = false;
unsigned long lastConnTime = 0;
unsigned long lastBlinkTime = 0; // 用於記錄上次燈號切換的時間
bool          ledStatus    = false; // 紀錄當前燈號狀態

// ======================== [ 3. 配對回調 ] ========================
void onPairingDone(bool success) {
  if (success) {
    Serial.println(">>> [配對日誌] 配對成功！");
  } else {
    Serial.println(">>> [配對日誌] 配對失敗，清除舊紀錄...");
    bleKeyboard.clearBonds();
  }
}

// ======================== [ 4. 省電模式 ] ========================
void doLightSleep(uint32_t seconds) {
  digitalWrite(LED_PIN, LOW); // 睡覺前一定要關燈，否則會耗電
  Serial.printf(">>> [系統日誌] 進入輕度睡眠 %d 秒...\n", seconds);
  Serial.flush();

  bleKeyboard.beforeSleep();
  esp_sleep_enable_timer_wakeup((uint64_t)seconds * 1000000ULL);
  esp_light_sleep_start();

  bleKeyboard.afterWake();
  Serial.println(">>> [系統日誌] 設備已喚醒。");
}

// ======================== [ 5. 初始化 ] ========================
void setup() {
  Serial.begin(115200);
  delay(500);
  
  // 設定 LED 引腳為輸出模式
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // 初始關閉

  Serial.println("\n=== FastTag 閃爍燈號版啟動 ===");

  esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);

  bleKeyboard.setSecurityMode(BLEKeyboardSecurity::JustWorks);
  bleKeyboard.onPairingComplete(onPairingDone);
  bleKeyboard.begin();

  // --- [新增：修改廣播間隔邏輯] ---
  // 獲取目前的廣告物件指標
  NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();

  /**
   * 設定廣播間隔範圍 (最小值, 最大值)
   * 數值計算公式：毫秒數 / 0.625
   * 範例：
   * 160  => 100ms  (反應極快，耗電高)
   * 800  => 500ms  (平衡點，推薦使用)
   * 1600 => 1000ms (最省電，連線稍慢)
   */
  pAdvertising->setMinInterval(800); // 設為 500ms
  pAdvertising->setMaxInterval(800); // 設為 500ms

  // 重新啟動廣告以套用設定
  pAdvertising->start();
  // -----------------------------

  // ... 後面的功率設定 ...
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, txPower);
  Serial.println(">>> [設定] 廣播間隔已調整為 500ms");
  
  NimBLEDevice::setMTU(23);
  lastConnTime = millis();
}

// ======================== [ 6. 主迴圈 ] ========================
void loop() {

  // --- 狀況 A：手機已連線 ---
  if (bleKeyboard.isConnected()) {

    if (!wasConnected) {
      Serial.println("\n>>> [事件] 手機已連線！");
      wasConnected = true;
      triggered    = false;
    }
    
    lastConnTime = millis();

    // 【新增】連線時的閃爍邏輯 (每 100ms 閃爍一次)
    if (millis() - lastBlinkTime > 100) {
      lastBlinkTime = millis();
      ledStatus = !ledStatus; // 切換狀態
      digitalWrite(LED_PIN, ledStatus ? HIGH : LOW);
    }

    // 發送按鍵觸發 (只發一次)
    if (!triggered) {
      delay(1500); 
      if (bleKeyboard.isConnected()) {
        bleKeyboard.tap(MEDIA_PLAY_PAUSE);
        triggered = true; 
        Serial.println(">>> [動作] 已發送媒體按鍵。");
      }
    }

  } 
  // --- 狀況 B：未連線狀態 ---
  else {
    // 斷線時立刻關燈
    if (wasConnected) {
      Serial.println("\n>>> [事件] 手機已斷開。");
      digitalWrite(LED_PIN, LOW); 
      wasConnected = false;
      triggered    = false;
      lastConnTime = millis();
    }

    // 未連線超過 30 秒進入睡眠
    if (millis() - lastConnTime > SLEEP_TIMEOUT) {
      doLightSleep(10);
      lastConnTime = millis();
    }
  }
  
  // 小延遲讓系統穩定，不建議在連線時用大 delay，否則燈號閃爍會卡頓
  delay(10); 
}