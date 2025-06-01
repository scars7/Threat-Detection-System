#include <WiFi.h>
#include <ESP_Mail_Client.h>
#include <time.h>

// ─── Wi-Fi Configuration (Replace with your own) ─────────────
#define WIFI_SSID         "your_wifi_name"
#define WIFI_PASSWORD     "your_wifi_password"

// ─── Gmail SMTP Configuration (Use App Password) ─────────────
#define SMTP_server       "smtp.gmail.com"
#define SMTP_Port         465
#define sender_email      "your_email@gmail.com"         // e.g., myesp32alert@gmail.com
#define sender_password   "your_email_app_password"      // Use a 16-char app password
#define Recipient_email   "recipient_email@example.com"
#define Recipient_name    "Receiver"

// ─── Component Pins ──────────────────────────────────────────
#define PIR_PIN           33  // PIR sensor OUT
#define BUZZER_PIN        23  // Buzzer +
#define RED_LED_PIN       22  // Red LED (motion detected)
#define GREEN_LED_PIN     21  // Green LED (no motion)

// ─── SMTP and Email Setup ────────────────────────────────────
SMTPSession smtp;
ESP_Mail_Session session;
SMTP_Message message;

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("\n=== ESP32 Threat Detection System Starting ===");

  // Set pin modes for all components
  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);

  // Default LED state: no motion (green ON)
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, HIGH);

  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("\n✅ Wi-Fi connected: " + WiFi.localIP().toString());

  // Set time for email timestamps (Nepal = UTC+5:45)
  configTime(20700, 0, "pool.ntp.org", "time.nist.gov");
  Serial.print("Syncing time");
  while (time(nullptr) < 100000) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\n⏰ Time synced!");

  // Setup SMTP session
  smtp.debug(1);
  MailClient.networkReconnect(true);

  session.server.host_name = SMTP_server;
  session.server.port = SMTP_Port;
  session.login.email = sender_email;
  session.login.password = sender_password;
  session.login.user_domain = "";
  session.time.ntp_server = "pool.ntp.org,time.nist.gov";
  session.time.gmt_offset = 20700;

  // Setup base email content
  message.sender.name = "ESP32 Alert System";
  message.sender.email = sender_email;
  message.subject = "⚠️ Threat Detected: Motion in Restricted Area";
  message.addRecipient(Recipient_name, Recipient_email);
}

void loop() {
  int motion = digitalRead(PIR_PIN);
  Serial.printf("PIR state: %d\n", motion);

  if (motion == HIGH) {
    Serial.println(">>> Motion detected! Triggering alarm...");

    // Blink red LED 3 times (150ms interval)
    for (int i = 0; i < 3; i++) {
      digitalWrite(RED_LED_PIN, HIGH);
      digitalWrite(GREEN_LED_PIN, LOW);
      delay(150);
      digitalWrite(RED_LED_PIN, LOW);
      delay(150);
    }

    // Buzzer sound for 3 seconds
    digitalWrite(BUZZER_PIN, HIGH);
    delay(3000);
    digitalWrite(BUZZER_PIN, LOW);

    // Get current time and format it
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    char timeString[100];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", timeinfo);
    Serial.print("📅 Time of motion: ");
    Serial.println(timeString);

    // Build the email message body
    String htmlBody =
      "<div style=\"font-family: Arial; color: #000;\">"
      "<h2>🚨 Motion Detected!</h2>"
      "<p><strong>Time:</strong> " + String(timeString) + "</p>"
      "<p><strong>Status:</strong> Intrusion detected in the restricted zone.</p>"
      "<p>This alert was sent from your ESP32-based security system.</p>"
      "<hr><p style=\"font-size: 0.9em; color: #555;\">ESP32 Alert System | IoT Demo</p>"
      "</div>";

    message.html.content = htmlBody.c_str();
    message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

    // Send email alert
    if (!smtp.connect(&session)) {
      Serial.println("❌ SMTP connection failed: " + smtp.errorReason());
    } else {
      if (!MailClient.sendMail(&smtp, &message))
        Serial.println("❌ Email send failed: " + smtp.errorReason());
      else
        Serial.println("✅ Email sent successfully!");
      smtp.closeSession();
    }

    delay(5000); // Cooldown to avoid spam
  } else {
    // No motion: green LED ON
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, HIGH);
  }

  delay(200); // Polling interval
}
