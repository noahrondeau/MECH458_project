
clc;
clear;

periods = 2;
L = 220;
restLevel = 1000;
minLevel = 30;
a = (restLevel-minLevel) / ((L/2)^2);
t = linspace(1,periods*L,periods*L);
x = zeros(1,L);

rng('shuffle');

x = floor(   (a.*(t(1:L) - L/2).^2) + minLevel );
x = x + floor(5.*normrnd(0,6,1,L));
x = [ restLevel.*ones(1,(periods-1)*L) x];

[b,a] = butter(3,0.075)

y = filter(b,a,x);

plot(t,x, 'b', t, y, 'r');
fvtool(b,a);