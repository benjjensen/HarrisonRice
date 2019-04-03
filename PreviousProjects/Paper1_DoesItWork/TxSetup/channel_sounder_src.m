%%% channel_sounder_src %%%

% This is a supporting file and need not be run on it's own. Please just
% run channel_sounder_matlab.m as this file is already included therein.

% This file was provided by Dr. Rice

plutoradiosetup();
N = 64;
x = zeros(N,1);
for idx = -31:32
    x = x + exp(1i*2*pi/N*idx*(0:N-1)');
end
mxm = max(abs(x));
if mxm > 1
    x = x ./ mxm;
end
