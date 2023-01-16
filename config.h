#ifndef DEBUG
const bool DEBUG = false;
#endif

struct Controller_Joy {
  double roll, pitch, throttle, yaw, main_switch, left_button, right_button, bottom_button;
};

struct Joy_Tune_Params {
  double roll_offset, pitch_offset, throttle_offset, yaw_offset;
};

const int roll_pin = 14; //A0
const int pitch_pin = 15; //A1
const int throttle_pin = 16; //A2
const int yaw_pin = 17; //A3
const int main_switch_pin = 3;
const int left_button_pin = 4;
const int right_button_pin = 5;
const int bottom_button_pin = 6;

const int tuned_center = 512;

float accelX,            accelY,             accelZ,            // units m/s/s i.e. accelZ if often 9.8 (gravity)
      gyroX,             gyroY,              gyroZ,             // units dps (degrees per second)
      gyroDriftX,        gyroDriftY,         gyroDriftZ,        // units dps
      gyroRoll,          gyroPitch,          gyroYaw,           // units degrees (expect major drift)
      gyroCorrectedRoll, gyroCorrectedPitch, gyroCorrectedYaw,  // units degrees (expect minor drift)
      accRoll,           accPitch,           accYaw,            // units degrees (roll and pitch noisy, yaw not possible)
      complementaryRoll, complementaryPitch, complementaryYaw;  // units degrees (excellent roll, pitch, yaw minor drift)

long lastTime;
long lastInterval;
