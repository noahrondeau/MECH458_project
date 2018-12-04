function y = sigmoid(x,L, k, x0)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
y = L ./ (1 + exp(-k.*(x - x0)));
end

