
clear;
clc;

max = 20;
accel_ramp = 12;
x = 0:accel_ramp-1;

y = 1000.*(sigmoid(x,accel_ramp,1, accel_ramp/2 - 1) + (max - accel_ramp));
y = fliplr(y);
plot(x, y);
