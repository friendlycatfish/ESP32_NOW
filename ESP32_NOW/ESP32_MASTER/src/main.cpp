#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h> // <--- BẮT BUỘC thêm để sửa lỗi kênh

typedef struct struct_message {
    char text[32];
    int count;
    float temp;
} struct_message;

struct_message myData;

// MAC của con Slave (ESP32U) - Bạn đã xác nhận đúng
uint8_t broadcastAddress[] = {0x44, 0x1D, 0x64, 0xF4, 0xC2, 0x58};
esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\r\nTrạng thái gửi: ");
    // Nếu status = 0 là thành công, khác 0 là thất bại
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Thành công" : "Thất bại");
}

void setup() {
    Serial.begin(115200);

    // 1. Kích hoạt WiFi Station
    WiFi.mode(WIFI_STA);

    // 2. ÉP MASTER CHẠY KÊNH 1 (Cực kỳ quan trọng để nhận ACK)
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

    if (esp_now_init() != ESP_OK) {
        Serial.println("Lỗi khởi tạo ESP-NOW");
        return;
    }

    esp_now_register_send_cb(OnDataSent);
    
    // 3. Cấu hình Peer
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 1;  // <--- PHẢI LÀ 1 (khớp với kênh WiFi)
    peerInfo.encrypt = false;
    
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
        Serial.println("Không thể thêm Slave");
        return;
    }
    
    randomSeed(analogRead(34));
}

void loop() {
    myData.count = random(0, 101);
    myData.temp = (float)random(200, 451) / 10.0;
    const char* list_msg[] = {"He thong OK", "Canh bao NHIET", "Dang chay...", "Sensor Error"};
    strcpy(myData.text, list_msg[random(0, 4)]);

    // Gửi gói tin
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
     
    if (result == ESP_OK) {
        Serial.printf(">>> Da gui: [%s] | Count: %d | Temp: %.1f C", myData.text, myData.count, myData.temp);
    }
    
    delay(2000); 
}