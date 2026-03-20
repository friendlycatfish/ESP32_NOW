#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h> // Thư viện bắt buộc để điều khiển kênh WiFi

// 1. Định nghĩa struct (Phải khớp 100% với Master)
typedef struct struct_message {
    char text[32];
    int count;
    float temp;
} struct_message;

struct_message incomingReadings;

// Khai báo Prototype
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);

// 2. Hàm nhận dữ liệu
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
    
    Serial.println("\n======= NHAN DU LIEU =======");
    Serial.print("Tu Master (MAC): ");
    for (int i = 0; i < 6; i++) {
        Serial.printf("%02X%s", mac[i], (i == 5) ? "" : ":");
    }
    
    Serial.printf("\nNoi dung: %s", incomingReadings.text);
    Serial.printf("\nSo thu tu: %d", incomingReadings.count);
    Serial.printf("\nNhiet do: %.2f C\n", incomingReadings.temp);
    Serial.println("============================");
}

void setup() {
    Serial.begin(115200);

    // BƯỚC 1: Kích hoạt WiFi ở chế độ Station
    WiFi.mode(WIFI_STA);

    // BƯỚC 2: Ép Slave luôn ở Kênh 1 (Để Master tìm thấy dễ dàng)
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

    // BƯỚC 3: Khởi tạo ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Loi khoi tao ESP-NOW");
        return;
    }
    
    // Đăng ký callback nhận dữ liệu
    esp_now_register_recv_cb(OnDataRecv);

    Serial.println("Slave (Receiver) da san sang o KENH 1...");
    Serial.print("MAC cua Slave nay: ");
    Serial.println(WiFi.macAddress());
}

void loop() {
    // Slave chỉ đứng đợi, không cần xử lý trong loop
}