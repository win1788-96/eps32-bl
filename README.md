# 🚀 FastTag - ESP32 智慧自動化藍牙標籤

FastTag 是一款基於 **NodeMCU-32S (ESP32)** 開發的低功耗藍牙 (BLE) HID 設備。它能偽裝成藍牙鍵盤，當手機靠近並自動連線時，觸發 Android 的「日常行程」或 iOS 的「捷徑」，實現進門開燈、自動打卡等自動化場景。

## ✨ 核心功能
* **HID 鍵盤偽裝**：模擬 `MEDIA_PLAY_PAUSE` 鍵，確保手機背景連線成功率。
* **強效廣播**：支援調整發射功率 (最高 +9dBm) 與廣播間隔。
* **低功耗管理**：支援 Light Sleep 輕度睡眠模式，延長電池壽命。
* **視覺回饋**：連線時內建 LED (GPIO 2) 自動閃爍，支援亮度調控 (PWM)。
* **中文日誌**：Serial Monitor 提供詳細的中文運作狀態輸出。

## 🛠️ 硬體需求
* **主控板**：NodeMCU-32S (或任何 ESP32-WROOM 開發板)
* **供電**：5V Micro USB 或 3.7V 鋰電池
* **內建燈號**：GPIO 2 (藍燈)

## 📦 必要函式庫 (Libraries)
請在 Arduino IDE 庫管理器中安裝以下函式庫：
1.  `NimBLE-Arduino` (由 h2zero 提供)
2.  `HijelHID_BLEKeyboard` (由 Hijel 提供)

## 🚀 快速上手
1.  **下載代碼**：複製 `FastTag.ino` 到你的專案資料夾。
2.  **設定功率**：在配置區修改 `txPower` 變數以調整訊號強弱。
3.  **燒錄**：選擇開發板 `NodeMCU-32S`，上傳程式碼。
4.  **手機配對**：
    * 打開手機藍牙，搜尋名為 **"FastTag"** 的裝置。
    * 點擊配對（僅需第一次手動操作）。
5.  **自動化設定**：
    * **Android**：設定 -> 日常行程 -> 條件：藍牙連線到 FastTag -> 執行：(你的動作)。

## ⚙️ 參數調整說明
| 參數名稱 | 說明 | 建議值 |
| :--- | :--- | :--- |
| `DEVICE_NAME` | 藍牙顯示名稱 | "FastTag" |
| `SLEEP_TIMEOUT` | 進入睡眠前的等待時間 | 30000 (ms) |
| `txPower` | 發射功率 | `ESP_PWR_LVL_P9` (最強) |
| `lowBrightness` | LED 閃爍亮度 (0-255) | 15 (微亮不刺眼) |

## 📂 資料夾結構
* `/src` : 包含主程式原始碼。
* `README.md` : 專案說明文件。

## 🤝 貢獻
歡迎提交 Pull Request 或回報 Issue。

## 📜 授權
MIT License
