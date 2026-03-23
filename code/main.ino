/*
 * Line following robot using 3 IR sensors and PD control
 * to adjust motor speeds for stable path tracking.
 */

// ===== Motor Driver Pins =====
#define enA 3   // Right motor speed (PWM)
#define in1 6   // Right motor direction
#define in2 7

#define enB 9   // Left motor speed (PWM)
#define in3 10  // Left motor direction
#define in4 11

// ===== IR Sensor Pins =====
#define L_S 2   // Left sensor
#define C_S 4   // Center sensor
#define R_S 5   // Right sensor

// ===== PD Controller Constants =====
float Kp = 12.0;
float Kd = 1.8;

// ===== Control Variables =====
int error = 0;
int lastError = 0;
int basePWM = 150;   // Base speed

void setup() {
  Serial.begin(9600);

  // Sensor inputs
  pinMode(L_S, INPUT);
  pinMode(C_S, INPUT);
  pinMode(R_S, INPUT);

  // Motor outputs
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  // Set motors forward direction
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void loop() {

  // Read sensors (1 = line detected, depending on your setup)
  int L = digitalRead(L_S);
  int C = digitalRead(C_S);
  int R = digitalRead(R_S);

  // If all sensors detect line, stop (end or lost condition)
  if (L == 1 && C == 1 && R == 1) {
    Stop();
    return;
  }

  // Determine error based on sensor position
  if (L == 1 && C == 0 && R == 0) {
    error = -1;   // too far left
  }
  else if (R == 1 && C == 0 && L == 0) {
    error = 1;    // too far right
  }
  else {
    error = 0;    // centered
  }

  // ===== PD Control =====
  int P = Kp * error;
  int D = Kd * (error - lastError);
  int control = P + D;

  lastError = error;

  // Adjust motor speeds
  int leftPWM  = basePWM + control;
  int rightPWM = basePWM - control;

  // Keep PWM within safe range
  leftPWM  = constrain(leftPWM, 60, 255);
  rightPWM = constrain(rightPWM, 60, 255);

  // Send values to motors
  driveMotors(leftPWM, rightPWM);
}

// ===== Drive Motors =====
void driveMotors(int leftPWM, int rightPWM) {

  // Forward direction
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);

  // Apply speed
  analogWrite(enA, rightPWM);
  analogWrite(enB, leftPWM);
}

// ===== Stop Robot =====
void Stop() {
  analogWrite(enA, 0);
  analogWrite(enB, 0);

  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}
