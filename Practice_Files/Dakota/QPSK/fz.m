function [fx_a,Sum_fx_a] = fz(H,sigmaSquared)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
a = [1+j -1+j -1-j 1-j];
fx_a = zeros(4, 101, 101);
Sum_fx_a = zeros(1,101, 101);
for loop = 1:4
    fx_a(loop,:,:) = (1/(2*pi*sigmaSquared))*exp((-1/(2*sigmaSquared))*abs((x-(H*a(loop))).^2));
    Sum_fx_a(1,:,:) = Sum_fx_a(1,:,:) + fx_a(loop,:,:);
end
end

