%%% channel_sounder_src %%%

% This is a supporting file and need not be run on it's own. Please just
% run channel_sounder_matlab.m as this file is already included therein.

% plutoradiosetup();

df = 0.3125; % subcarrier separation in MHz
BW = 20; % BW in MHz
Fs = 20; % sample rate in MHz
f0 = 70; % bandpass frequency in MHz

dF = df/Fs;
N0 = 1/dF;
F0 = f0/Fs;

load optimal_phases64.mat; % theta_opt variable

x = zeros(N0,1);
for idx = -31:32
    x = x + exp(1i*2*pi*dF*idx*(0:N0-1)') * exp(1i*theta_opt(idx + 32));
end

mxm = max(abs(x));
if mxm > 1
    x = x ./ mxm;
end

%% Nathan's edits below

n = (0:N0 - 1);

xl2 = repmat(x, 1000, 1);
xl3 = real(xl2.*exp(1i*2*pi*F0*(0:length(xl2)-1)'));

figure(2);
subplot(311); plot(n,real(x)); grid on;
subplot(312); plot(n,abs(x)); grid on;
subplot(313); plot(n,xl3(1:N0)); grid on;