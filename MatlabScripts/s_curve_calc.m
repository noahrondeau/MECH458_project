
clear;
clc;

max = 20;
accel_ramp = 10;
x = 0:accel_ramp-1;


for growth= 0.5:0.25:1.5
    hold on;
    y = 1000.*(sigmoid(x,accel_ramp,growth, accel_ramp/2 - 1) + (max - accel_ramp));
    y = fliplr(y);
    plot(x, y);
end

hold off
