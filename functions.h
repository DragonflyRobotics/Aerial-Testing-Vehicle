//#include "config.h"

Controller_Joy get_joystick() {
  double roll, pitch, throttle, yaw;

  roll = analogRead(roll_pin);
  pitch = analogRead(pitch_pin);
  throttle = analogRead(throttle_pin);
  yaw = analogRead(yaw_pin);

  roll = map(roll, 0, 1023, 1023, 0);
  pitch = map(pitch, 0, 1023, 1023, 0);
  throttle = map(throttle, 0, 1023, 1023, 0);
  yaw = map(yaw, 0, 1023, 1023, 0);

  if (DEBUG) {
    Serial.print("Roll: ");
    Serial.print(roll);
    Serial.print(", Pitch: ");
    Serial.print(pitch);
    Serial.print(", Throttle: ");
    Serial.print(throttle);
    Serial.print(", Yaw: ");
    Serial.println(yaw);
  }
  return {roll, pitch, throttle, yaw};
}

Controller_Joy get_full_data() {
  double roll, pitch, throttle, yaw, main_switch, left_button, right_button, bottom_button;

  roll = analogRead(roll_pin);
  pitch = analogRead(pitch_pin);
  throttle = analogRead(throttle_pin);
  yaw = analogRead(yaw_pin);
  main_switch = digitalRead(main_switch_pin);
  left_button = digitalRead(left_button_pin);
  right_button = digitalRead(right_button_pin);
  bottom_button = digitalRead(bottom_button_pin);

  roll = map(roll, 0, 1023, 1023, 0);
  pitch = map(pitch, 0, 1023, 1023, 0);
  throttle = map(throttle, 0, 1023, 1023, 0);
  yaw = map(yaw, 0, 1023, 1023, 0);

  if (DEBUG) {
    Serial.print("Roll: ");
    Serial.print(roll);
    Serial.print(", Pitch: ");
    Serial.print(pitch);
    Serial.print(", Throttle: ");
    Serial.print(throttle);
    Serial.print(", Yaw: ");
    Serial.print(yaw);

    Serial.print(", Main Switch: ");
    Serial.print(main_switch);
    Serial.print(", Right: ");
    Serial.print(right_button);
    Serial.print(", Left: ");
    Serial.print(left_button);
    Serial.print(", Bottom: ");
    Serial.println(bottom_button);
  }
  return {roll, pitch, throttle, yaw, main_switch, left_button, right_button, bottom_button};
}

//================================================================================================================================================
Joy_Tune_Params get_params(Controller_Joy snapshot) {
  double roll_param = abs(snapshot.roll - tuned_center);
  double pitch_param = abs(snapshot.pitch - tuned_center);
  double throttle_param = abs(snapshot.throttle - tuned_center);
  double yaw_param = abs(snapshot.yaw - tuned_center);

  return {roll_param, pitch_param, throttle_param, yaw_param};
}

Controller_Joy get_calibrated_joystick(Joy_Tune_Params params) {
  double roll, pitch, throttle, yaw;

  roll = analogRead(roll_pin);
  pitch = analogRead(pitch_pin);
  throttle = analogRead(throttle_pin);
  yaw = analogRead(yaw_pin);

  roll = map(roll, 0, 1023, 1023, 0);
  pitch = map(pitch, 0, 1023, 1023, 0);
  throttle = map(throttle, 0, 1023, 1023, 0);
  yaw = map(yaw, 0, 1023, 1023, 0);

  roll = roll+params.roll_offset;
  pitch = roll+params.pitch_offset;
  //throttle = roll+params.throttle_offset;
  yaw = roll+params.yaw_offset;

  if (DEBUG) {
    Serial.print("Roll: ");
    Serial.print(roll);
    Serial.print(", Pitch: ");
    Serial.print(pitch);
    Serial.print(", Throttle: ");
    Serial.print(throttle);
    Serial.print(", Yaw: ");
    Serial.println(yaw);
  }
  return {roll, pitch, throttle, yaw};
}

